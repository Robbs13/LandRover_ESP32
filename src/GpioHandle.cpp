#include "GpioHandle.h"
#include "Config.h"   // für qRCCom, RcRawFrame etc.
#include <Arduino.h>

// ---------------------------------------------------------
//  Klasse RcCom für die Kommunikation mit dem RC Sender
// ---------------------------------------------------------
GpioHandle::GpioHandle(int n)
: _taskHandle(NULL)   
{

}

// ---------------------------------------------------------
//  UART Initialsierung
// ---------------------------------------------------------
void GpioHandle::begin() {
    // Pins ggf. aus Config nehmen: g_ioPins.pinXlrsRx / pinXlrsTx
    // _crsf.begin(CRSF_BAUDRATE, SERIAL_8N1, 16, 17); 
    //initFromConfig();
    
    Serial.println("CTRLSIM: Begin...");
}

// ---------------------------------------------------------
//  Task erstellen
// ---------------------------------------------------------
bool GpioHandle::startTask(UBaseType_t priority, BaseType_t core, uint8_t time) {
    _cycleTime = time;

    
    BaseType_t res = xTaskCreatePinnedToCore(
        GpioHandle::taskTrampoline,  // statische Entry-Funktion
        "CtrlSim",                // Name (für Debug)
        4096,                   // Stack-Größe (Wörter, nicht Bytes)
        this,                   // this als Parameter übergeben!
        priority,               // Priorität
        &_taskHandle,           // Task-Handle
        core                    // Core 0 oder 1
    );
    return (res == pdPASS);
}
void GpioHandle::taskTrampoline(void *pvParameters) {
    // pvParameters ist der this-Pointer
    GpioHandle *self = static_cast<GpioHandle*>(pvParameters);
    self->taskLoop();   // jetzt sind wir im Instanz-Kontext
}

// ---------------------------------------------------------
//  Task Loop für den Ablauf 
// ---------------------------------------------------------
void GpioHandle::taskLoop() {
    begin();

    const TickType_t period = pdMS_TO_TICKS(_cycleTime);     // Zykluszeit der Taskschleife
    TickType_t lastWake = xTaskGetTickCount();

    gpioTaskInit();

    for (;;)
    { 
        handleGPIOQueue();
        handleGPIOOutput();


        // Task läuft genau alle 5 ms
        vTaskDelayUntil(&lastWake, period);
    }
}

// ---------------------------------------------------------
//  Aktuelle GpioData aus der Queue holen
// ---------------------------------------------------------
void GpioHandle::handleGPIOQueue()
{
    if (!q_Gpio) return;

    // Queue leeren, wir behalten den neuesten
    uint32_t missed = 0;
    
    //uint32_t _lastReceive = _gpioFrame.timestampMs;

    while (xQueueReceive(q_Gpio, &_gpioFrame, 0) == pdTRUE) {
        missed++;
        //_newRcData = true;
        //Serial.println("GPIO Receive");
    }

    // #if defined(DEBUG_CTRLSIM_RC_MISSED)
    //     if (missed > 1) {
    //         uint32_t delta = _rc.timestampMs - _lastReceive;
    //         Serial.print("CTRLSIM: RC Frames verpasst. Anzahl in Buffer: ");
    //         Serial.print(missed);
    //         Serial.print(" - Delta zum letzten Frame: ");
    //         Serial.println(delta);
    //     }
    // #endif
    
    return;
}

// ---------------------------------------------------------
//  Ansteuerungen aus von GpioData abhandeln
// ---------------------------------------------------------
void GpioHandle::handleGPIOOutput()
{
    // -------- _gpioFrame die einzelnen Ansteuerungen abarbeiten --------
    for (uint8_t i = 0; i < _gpioFrame.gpioCount; ++i)
    {
        const uint8_t  pin      = _gpioFrame.gpioCtrl[i].pin;
        const uint16_t rcValue  = _gpioFrame.gpioCtrl[i].value; // 1000..2000

        OutputHardware hw = OutputHardware::Digital;
        bool found = false;

        // -------- Map durchsuchen: pin -> OutputHardware --------
        for (uint8_t idx = 0; idx < MAP_COUNT; ++idx)
        {
            const RcGpioMap& cfg = cfg_rcGpioMap[idx];
            if (pin == static_cast<uint8_t>(cfg.gpioConfig)) {
                hw = cfg.outputHardware;
                found = true;
                break;
            }
        }
        if (!found) continue;

        // -------- Ansteuerung anhand der Hardware Pin Konfiguration --------
        switch (hw)
        {
            case OutputHardware::Motor:
                // TODO
                break;

            case OutputHardware::MotorESC:
            {
                int ch = _pinToCh[pin];
                if (ch == INVALID_CH) break;

                const uint32_t duty = rcToLedcDuty_ServoEsc(rcValue,
                                                           PWM_SERV_MOT_FREQ,
                                                           PWM_SERV_MOT_RES);
                ledcWrite(ch, duty);
                Serial.println("ServoESC ansteuerung:   ");
                break;
            }

            case OutputHardware::Servo:
            {
                int ch = _pinToCh[pin];
                if (ch == INVALID_CH) break;

                const uint32_t duty = rcToLedcDuty_ServoEsc(rcValue,
                                                           PWM_SERV_MOT_FREQ,
                                                           PWM_SERV_MOT_RES);
                ledcWrite(ch, duty);
                Serial.println("ServoESC ansteuerung:   ");
                break;
            }

            case OutputHardware::LED:
            {
                int ch = _pinToCh[pin];
                if (ch == INVALID_CH) break;

                const uint32_t duty = rcToLedcDuty_LedDim(rcValue, PWM_LED_RES);
                ledcWrite(ch, duty);
                break;
            }

            case OutputHardware::Digital:
                digitalWrite(pin, (rcValue >= 1500) ? HIGH : LOW);
                Serial.print("Digital ansteuerung      ");
                Serial.println(pin);
                break;

            case OutputHardware::Sound:
                // TODO
                break;

            default:
                break;
        }
        
    }

}

// ---------------------------------------------------------
//  GPIOs anhand der Config initialisieren - PWM,Digital...
// ---------------------------------------------------------
void GpioHandle::gpioTaskInit()
{
    // -------- _pinToCh auf ungültig setzen --------
    for (int p = 0; p < MAX_GPIO; ++p) _pinToCh[p] = INVALID_CH;

    int pwmChannel = 0;

    // -------- Zeilen der Config abarbeiten --------
    for (size_t i = 0; i < MAP_COUNT; ++i)
    {
        const RcGpioMap& cfg = cfg_rcGpioMap[i];
        const uint8_t pin = static_cast<uint8_t>(cfg.gpioConfig);

        if (pin == 99) continue;
        if (pin >= MAX_GPIO) continue;

        // -------- Anhand der angeschlossenen Hardware werden die Ausgänge initialisiert --------
        switch (cfg.outputHardware)
        {
            case OutputHardware::MotorESC:
            case OutputHardware::Servo:
                initPwmPin(pin, PWM_SERV_MOT_FREQ, PWM_SERV_MOT_RES, pwmChannel);
                break;

            case OutputHardware::LED:
                initPwmPin(pin, PWM_LED_FREQ, PWM_LED_RES, pwmChannel);
                break;

            case OutputHardware::Digital:
                pinMode(pin, OUTPUT);
                break;

            case OutputHardware::Motor:   // TODO
            case OutputHardware::Sound:   // TODO
            default:
                break;
        }
    }
}

// ---------------------------------------------------------
//  LEDC PWM Channels initialisieren
// ---------------------------------------------------------
void GpioHandle::initPwmPin(uint8_t pin, uint32_t freq, uint8_t res, int &pwmChannel)
{
    if (_pinToCh[pin] != INVALID_CH) return;

    if (pwmChannel >= MAX_PIN_PWM) {
        Serial.printf("GPIOHandle: Zu viele PWM Ports (pin=%u)\n", pin);
        return;
    }

    ledcSetup(pwmChannel, freq, res);
    ledcAttachPin(pin, pwmChannel);

    _pinToCh[pin] = (int8_t)pwmChannel;
    pwmChannel++;
}

// ---------------------------------------------------------
//  Umrechnung von RC Werte 1000..2000us zu Servo/MotorESC
// ---------------------------------------------------------
uint32_t GpioHandle::rcToLedcDuty_ServoEsc(uint16_t rc_1000_2000,
                              uint32_t servo_freq_hz,
                              uint8_t  servo_res_bits)
{
    // Sicherheits-Clamp
    rc_1000_2000 = clampU16(rc_1000_2000, 1000, 2000);

    // LEDC duty Bereich
    const uint32_t maxDuty = (1u << servo_res_bits) - 1u;

    // Periode in µs (bei 50 Hz = 20000 µs)
    const uint32_t period_us = 1000000u / servo_freq_hz;

    // rc wird als Pulsbreite in µs interpretiert (1000..2000)
    const uint32_t pulse_us = rc_1000_2000;

    // duty = pulse/period * maxDuty
    return (uint32_t)((pulse_us * (uint64_t)maxDuty) / period_us);
}

// ---------------------------------------------------------
//  Umrechnung von RC Werte 1000..2000us zu LED Dim
// ---------------------------------------------------------
uint32_t GpioHandle::rcToLedcDuty_LedDim(uint16_t rc_1000_2000, uint8_t led_res_bits)
{
    rc_1000_2000 = clampU16(rc_1000_2000, 1000, 2000);

    const uint32_t maxDuty = (1u << led_res_bits) - 1u;

    // linear: 1000->0, 2000->maxDuty
    return (uint32_t)(((rc_1000_2000 - 1000u) * (uint64_t)maxDuty) / 1000u);
}




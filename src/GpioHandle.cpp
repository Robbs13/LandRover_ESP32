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
    initFromConfig();
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

 

    for (;;)
    { 
        handleGPIOQueue();
        handleGPIOOutput();


        // Task läuft genau alle 5 ms
        vTaskDelayUntil(&lastWake, period);
    }
}

// ---------------------------------------------------------
//  Aktuelle RC Inputs aus Queue qRCCom holen
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

void GpioHandle::handleGPIOOutput()
{
    for (uint8_t i = 0; i < _gpioFrame.pwmCount; ++i){
        writePwm(_gpioFrame.pwm[i].pin, _gpioFrame.pwm[i].value);
        // writePwm(23, _gpioFrame.pwm[i].value);
        // Serial.print("PWM: ");
        // Serial.print(_gpioFrame.pwm[i].pin);
        // Serial.print(" -  ");
        // Serial.println(_gpioFrame.pwm[i].value);
    }
    

    for (uint8_t i = 0; i < _gpioFrame.digCount; ++i)
    gpio_set(_gpioFrame.dig[i].pin, _gpioFrame.dig[i].value);

    for (uint8_t i = 0; i < _gpioFrame.dacCount; ++i)
    dac_write(_gpioFrame.dac[i].pin, _gpioFrame.dac[i].value);

    

    return;
}

// void GpioHandle::writePwm(uint8_t pin, uint16_t value)
// {
//     // value: 1000..2000 (µs)
//     // Umrechnen auf Duty (0..65535 bei 16 Bit)
//     uint32_t duty = map(value, 1000, 2000, 0, (1 << PWM_RES) - 1);

//     //uint8_t ch = pwmChannelForPin(pin);

//     // ledcSetup(ch, PWM_FREQ, PWM_RES);
//     // ledcAttachPin(pin, ch);

//     // // PWM setzen
//     // ledcWrite(ch, duty);
    
//     return;
// }

void GpioHandle::gpio_set(uint8_t pin, uint16_t value)
{


    
    return;
}

void GpioHandle::dac_write(uint8_t pin, uint16_t value)
{


    
    return;
}

void GpioHandle::initFromConfig()
{
  // pin->channel Tabelle auf "ungültig" setzen
  for (uint8_t i = 0; i < MAX_GPIO; ++i) pinToCh_[i] = INVALID_CH;
  nextCh_ = 0;

  // Config scannen
  for (size_t i = 0; i < MAP_COUNT; ++i) {
    const auto& m = cfg_rcGpioMap[i];

    if (m.outputType != OutputType::PWM) continue;

    const uint8_t pin = static_cast<uint8_t>(m.gpioConfig);
    if (pin == 99) continue;              // "unused"
    if (pin >= MAX_GPIO) continue;

    // wenn der Pin schon einen Channel hat -> überspringen (doppelte Pins)
    if (pinToCh_[pin] != INVALID_CH) continue;

    // neuen PWM Pin initialisieren
    initPwmPin(pin);
  }
}

void GpioHandle::initPwmPin(uint8_t pin)
{
  if (nextCh_ >= MAX_LEDC_CH) {
    // zu viele PWM Pins -> hier ggf. Fehlerflag setzen / Serial log
    return;
  }

  const uint8_t ch = nextCh_++;
  pinToCh_[pin] = ch;

  ledcSetup(ch, PWM_FREQ, PWM_RES);
  ledcAttachPin(pin, ch);

  // optional: initial neutral/off
  // ledcWrite(ch, 0);
}

void GpioHandle::writePwm(uint8_t pin, uint16_t us)
{
  if (pin >= MAX_GPIO) return;

  const uint8_t ch = pinToCh_[pin];

  //Serial.printf("writePwm pin=%u ch=%u v=%u\n", pin, ch, us);

  if (ch == INVALID_CH) return;

  // value ist immer 1000..2000 (laut deiner Vorgabe)
  // -> auf duty 0..maxDuty mappen
  const uint32_t maxDuty = (1u << PWM_RES) - 1u; // bei 8 Bit: 255

  if (us <= 1000) {
    ledcWrite(ch, 0);
    return;
  }
  if (us >= 2000) {
    ledcWrite(ch, maxDuty);
    return;
  }

  const uint32_t x = static_cast<uint32_t>(us - 1000); // 0..1000
  const uint32_t duty = (x * maxDuty) / 1000u;            // 0..255

  ledcWrite(ch, duty);
  
}
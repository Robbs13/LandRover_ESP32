#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Config.h" 
#include <HardwareSerial.h>


// -------- CRSF Eigenschaften --------
#define CTRL_RC_MIN     1000
#define CTRL_RC_MIDDLE  1500
#define CTRL_RC_MAX     2000
#define CTRL_RC_DEADZONE 50
#define CTRL_3POS_0P 1500
#define CTRL_3POS_1P 1900
#define CTRL_3POS_2P_MAX 1100
#define CTRL_3POS_2P_MIN 900
#define CTRL_POS_2P_ON 1600
#define CTRL_POS_2P_OFF 1400
#define CTRL_FAILSAFE_VALUE 1000

// -------- PWF state --------
enum class PWFStatus : uint8_t {
    Parken ,    // Fahrzeug ohne RC Verbindung
    Wohnen,     // Licht, Verbraucher an, aber kein Fahren
    Fahren      // Antrieb aktiv
};

enum class FailSafe : uint8_t {
    RC_Failure = 0,   // Fahrzeug ohne RC Verbindung
    Sensor_Failure = 1,   // Keine Sensor Daten
};

// -------- Blink state --------
enum class BlinkState : uint8_t {
  Off,
  Left,
  Right,
  Hazard
};
// -------- Blink direction --------
enum class BlinkDir : uint8_t { 
    None, 
    Left, 
    Right 
};
// -------- Lichtsteuerung --------
enum class LightState : uint8_t {
    Off,
    Flash,
    LightOn,
    BeamFlash,
    BeamOn,
};

// -------- Struct für Flicker Overlay --------
struct FlickerState {
    // Laufzeit-Zustand (Gedächtnis)
    uint32_t startMs = 0;     // Startzeit des Effekts (millis)
    uint32_t lastMs  = 0;     // letzte Update-Zeit (millis)
    float phase      = 0.0f;  // Sinus-Phase (wird integriert)

    // Parameter: Frequenz über Zeit (Hz)
    float fStartHz = 1.4f;    // Anfang: schneller
    float fEndHz   = 0.35f;   // Ende: langsamer
    float fTauSec  = 3.0f;    // wie schnell f runterläuft

    // Parameter: Amplitude über Zeit (als Prozent-Faktor 0..1)
    // Multiplikativ ist es am einfachsten, wenn ampNorm 0..1 ist
    float ampNormStart = 0.35f;  // z.B. ±35% am Anfang
    float ampNormEnd   = 0.07f;  // z.B. ±7% am Ende
    float ampNormTauSec= 6.0f;   // wie schnell die Schwankung kleiner wird

    // im aktuellen Loop berechnete Werte (werden von tickFlicker gesetzt)
    float osc     = 0.0f;     // Sinuswert [-1..+1]
    float ampNorm = 0.0f;     // aktuelle Amplitude 0..1
};


class CtrlSim {
public:
    explicit CtrlSim(int n);

    // UART Initialisierung
    void begin(); 

    // Startet den FreeRTOS-Task für diese Instanz
    bool startTask(UBaseType_t priority, BaseType_t core, uint8_t time);

private:
    // Die eigentliche Task-Schleife (instanzbezogen)
    void taskLoop();

    // Statischer „Trampolin“-Entry für FreeRTOS
    static void taskTrampoline(void *pvParameters);

    // Methoden für die Verarbeitung der RC Sender Daten
    void getIdxConfig();
    void handleRcQueue();
    void failSafeCheck();
    void failSafeHandle();
    void handleLights();

    void handleStartButton();
    void handleBlinkLights();

    //void updateBlinkerStateMachine();
    // void handleSim();
    // void handleSensors();


    void publishGPIOQueue();

    static int findFirstMapIndex(FunctionList f);
    BlinkState checkBlinkRequest(bool hazard, bool left, bool right);
    void setPWF(PWFStatus targetPWF);
    void debug();


private:
    TaskHandle_t    _taskHandle;
    RcFrameData         _rc{};
    GpioData _gpioFrame {};

    uint32_t            _lastFailsafeSend;           // Zeitstempel des letzten gültigen Frames
    uint8_t             _cycleTime;   
    bool                _newRcData;               // Zykluszeit von dem Task
    bool                _failsafeActive;            // aktueller Failsafe-Status
    
    uint32_t        _lastDebugMs;                       // Aktuelle Zeit

    PWFStatus         _statePWF = PWFStatus::Parken;
    bool               _newControlData; 

    // -------- Mapping aus Config Array --------
    const RcGpioMap* _mapBlinkLeft      = nullptr;
    const RcGpioMap* _mapBlinkRight     = nullptr;
    const RcGpioMap* _mapHeadLight      = nullptr;
    const RcGpioMap* _mapBeamLight      = nullptr;
    const RcGpioMap* _mapRearLight      = nullptr;
    const RcGpioMap* _mapBrakeLight     = nullptr;
    const RcGpioMap* _mapReverseLight   = nullptr;
    const RcGpioMap* _mapPosLight       = nullptr;
    const RcGpioMap* _mapWorkLight      = nullptr;
    const RcGpioMap* _mapCabinLight     = nullptr;



    // -------- Lichtsteuerung --------
    uint32_t _beamPressStartMs = 0;
    bool     _beamPressedPrev = false;
    bool     _beamLongHandled = false;
    LightState   _base = LightState::Off;
    FlickerState _startFlicker;


    // -------- Blinksteuerung --------
    bool        _blinkOn = false;
    BlinkState  _activeBlink = BlinkState::Off;
    BlinkState  _pendingBlink = BlinkState::Off;
    BlinkDir    _lastDir = BlinkDir::None;
    uint32_t    _lastBlinkMs  = 0;

    // -------- Motorsteuerung --------
  


    // ---------------------------------------------------------
    //  Hilfsprogramme
    // ---------------------------------------------------------


    // -------- Hilfsprogramm prüft ob in GPIO Queue schon der Ausgang angesteuert wird - Overwrite --------
    static inline void addOrUpdate(PinControl* arr, uint8_t& count, uint8_t max,
                               uint8_t pin, uint16_t value)
    {
        for (uint8_t i = 0; i < count; ++i) {
            if (arr[i].pin == pin) {
            arr[i].value = value;
            return;
            }
        }
        if (count < max) {
            arr[count++] = { pin, value };
        }
    }

    // -------- Hilfsprogramm um GPIO Liste für Queue befüllen --------
    static inline void addToGpioQueue(GpioData& f, PinControl in)
    {
    addOrUpdate(f.gpioCtrl, f.gpioCount, MAX_PIN_ESP, in.pin, in.value);
    }

    // static inline void addDigital(GpioData& f, PinControl in)
    // {
    // addOrUpdate(f.dig, f.digCount, MAX_PIN_DIG, in.pin, in.value);
    // }

    // static inline void addDAC(GpioData& f, PinControl in)
    // {
    // addOrUpdate(f.dac, f.dacCount, MAX_PIN_DAC, in.pin, in.value);
    // }

    // -------- Hilfsprogramm für Spannungseinbruch - Flicker --------
    static inline float expEase(float start, float end, float tSec, float tauSec) {
    // Exponentielle Annäherung: bei t=0 -> start, später -> end
    return end + (start - end) * expf(-tSec / tauSec);
    }

    // -------- Hilfsprogramm für Spannungseinbruch - Flicker Reset --------
    static inline void flickerReset(FlickerState& s) {
        // Effekt zurücksetzen (wichtig wenn Motorstart endet)
        s.startMs = 0;
        s.lastMs  = 0;
        s.phase   = 0.0f;
        s.osc     = 0.0f;
        s.ampNorm = 0.0f;
    }

    // -------- Hilfsprogramm für Spannungseinbruch - Flicker Zeitposition von Overlay --------
    static inline void tickFlicker(FlickerState& s, uint32_t nowMs) {   // Nur 1x pro Loop aufrufen, damit alle Lampen synchron bleiben
        
        if (s.startMs == 0) {      // Effekt startet gerade
            s.startMs = nowMs;
            s.lastMs  = nowMs;
            s.phase   = 0.0f;
        }
        const float t  = (nowMs - s.startMs) * 0.001f;                          // Sekunden seit Start
        const float dt = fminf((nowMs - s.lastMs) * 0.001f, 0.05f);             // dt clamp (max 50ms)
        s.lastMs = nowMs;
        const float f = expEase(s.fStartHz, s.fEndHz, t, s.fTauSec);            // Frequenz fällt über Zeit ab (Generator wird "ruhig")
        s.phase += 2.0f * PI * f * dt;                                          // Phase integrieren => sauberer Sinus auch bei variabler Frequenz
        s.ampNorm = expEase(s.ampNormStart, s.ampNormEnd, t, s.ampNormTauSec);  // Amplitude als Normfaktor (0..1), nimmt über Zeit ab
        s.osc = sinf(s.phase);                                                  // Gemeinsamer Sinuswert für alle Lichter (synchron)
    }

    // -------- Hilfsprogramm für Spannungseinbruch - Flicker Value wird mit Overlay belegt --------
    static inline void updateSineFlickerMul(
        const FlickerState& s,
        uint16_t& value,          // aktueller PWM-Wert (z.B. aus FSM), wird modifiziert
        float intensityFactor,    // 0..2 (1.0 = normal, 0.5 halb so stark, 0 = aus)
        uint16_t minV,
        uint16_t maxV
    ){
        if (value == 1000) return;
        // Multiplikativ: value schwankt prozentual um sich selbst
        // mul = 1 ± (ampNorm * intensityFactor)
        const float mul = 1.0f + (s.ampNorm * intensityFactor) * s.osc;

        const float v = (float)value * mul;
        value = (uint16_t)constrain((int)lroundf(v), minV, maxV);
    }



};



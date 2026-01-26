#pragma once

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


// ---------------------------------------------------------
//  Allgemeine Einstellungen:
//  - Fahrzeug
//  - Sendeprotokoll
//  - Remote Control
//  - ESP Controller
//  - Debug 
//
//  *** Hier müssen Anpassungen gemacht werden ***
// ---------------------------------------------------------

#define LANDROVER_TOWTRUCK
#define CRSF_SERIAL
#define POCKET
#define ESP32AZDEVKITCV4    //ESP32: https://www.az-delivery.de/products/esp-32-dev-kit-c-v4

// ---------------------------------------------------------
//  Konfiguration: Debug
// ---------------------------------------------------------
#if defined(CRSF_SERIAL)
    //#define DEBUG_CRSF_IN
    //#define DEBUG_CRSF_OUT
#endif

#define DEBUG_CTRLSIM_RC_MISSED
#define DEBUG_CTRLSIM_TASK_CYCLE

// ---------------------------------------------------------
//  Konfiguration: Fahrzeug
// ---------------------------------------------------------
#if defined(LANDROVER_TOWTRUCK)

    #define LIGHT_OUT 1000
    #define BLINK_TIME 500
    #define BLINK_BRIGHTNESS 2000
    #define HEAD_BRIGHTNESS 1500
    #define BEAM_BRIGHTNESS 2000
    #define REAR_BRIGHTNESS 1500
    #define BRAKE_BRIGHTNESS 2000
    #define POS_BRIGHTNESS 2000
    #define REVERSE_BRIGHTNESS 2000
    #define WORK_BRIGHTNESS 2000
    #define BEAM_LONG_MS 1000


    // ---------------------------------------------------------
    //  Aktive Funktionen
    // ---------------------------------------------------------
    enum class FunctionList : uint8_t {
        handleDrive,
        handleBrake,
        handleSteer,
        handleGear,
        handleWinch,
        handleShaker,
        handleSound1,
        handleSound2,
        handleBlinkLeft,
        handleBlinkRight,
        handleLightHead,
        handleLightBeam,
        handleLightRear,
        handleLightBrake,
        handleLightReverse,
        handleLightPos,
        handleLightWork,
        handleLightCabin,
        count
    };

    struct VehicleSimConfig {
        float massKg;           // Fahrzeugmasse in kg
        float wheelBaseX;       // Spurweite in m
        float wheelBaseY;       // Spurstand in m
        float maxSpeed;         // theoretische Max-Speed in kmh
        float torqueCurve;      // Motorleistung (vereinfacht) nun... noch keine Ahnung
        float dragCoeff;        // 0..1, Luft-/Rollwiderstand
        float tireGrip;         // 0..1, Grip-Faktor
    };

    extern const VehicleSimConfig cfg_vehicleSim;
#endif



/** ---------------------------------------------------------
 *  Beschreibt alle Eigenschaften eines realen Fahrzeugs,
 *  die für die Längsdynamik-Simulation (Drehmoment, Drehzahl, Motorbremse, Begrenzer)
 *  benötigt werden.
 *  ---------------------------------------------------------
*/

// -------- Auswahl der möglichen Fahrzeuge --------
// enum VehicleType {
//     LR_SERIES3_225_DIESEL
//     // später erweiterbar
// };
// constexpr VehicleType SELECTED_VEHICLE = LR_SERIES3_225_DIESEL;

// // ---------------------------------------------------------
// //  Konfiguration: Struct Motor Eigenschaften
// // ---------------------------------------------------------
// struct EngineSpec
// {
//     float idle_rpm;                         // Leerlaufdrehzahl des Motors [U/min]
//     float redline_rpm;                      // Maximale zulässige Drehzahl (Begrenzerbeginn) [U/min]
//     static const int TORQUE_POINTS = 6;     // Anzahl der Stützstellen der Drehmomentkennlinie
//     float rpm[TORQUE_POINTS];               // Drehzahl-Stützstellen der Drehmomentkurve [U/min] - {900, 1300, 1800, 2500, 3200, 3800}
//     float torque[TORQUE_POINTS];            // Normiertes Motordrehmoment bei den jeweiligen Drehzahlen [0.0..1.0]
//     float drag_k1;                          // Grund-Schleppmoment (Motorbremse) bei Leerlaufdrehzahl [0.0..1.0]
//     float drag_k2;                          // Drehzahlabhängiger Anteil des Schleppmoments [0.0..1.0] wie stark der Motor bei hohen Drehzahlen verzögert.
//     float limiter_soft_range;               // Bereich oberhalb der Redline, in dem der Soft-Limiter wirkt [U/min]
// };


// // -------- Land Rover Series III 2.25 Diesel (2286 cc, Type 10J/11J) --------
// const EngineSpec ENGINE_SERIES3_225_DIESEL = {
//     .idle_rpm = 850,
//     .redline_rpm = 3800,

//     .rpm =     { 900, 1300, 1800, 2500, 3200, 3800 },
//     .torque =  { 0.30, 0.70, 1.00, 0.85, 0.60, 0.40 },

//     .drag_k1 = 0.06,     // Grundschleppmoment
//     .drag_k2 = 0.25,     // quadratisch mit Drehzahl

//     .limiter_soft_range = 400
// };

// // -------- Methode um die Motordaten zu erhalten --------
// const inline EngineSpec& getEngineSpec(VehicleType v)
// {
//     switch(v) {
//         case LR_SERIES3_225_DIESEL:
//             return ENGINE_SERIES3_225_DIESEL;

//         // case VEH_SERIES3_225_PETROL:
//         //     return ENGINE_SERIES3_225_PETROL;

//         default:
//             return ENGINE_SERIES3_225_DIESEL;
//     }
// }


// ---------------------------------------------------------
//  Konfiguration: Sendeformat
// ---------------------------------------------------------
#if defined(CRSF_SERIAL)
    // Anzahl der Kanäle 
    #define CRSF_NUM_CHANNELS 16
#endif


// ---------------------------------------------------------
//  Konfiguration:  ESP Controller
// ---------------------------------------------------------
#if defined(ESP32AZDEVKITCV4)
    //#define ESP_PIN_MAX 20
    #define MAX_PIN_PWM 16
    #define MAX_PIN_DIG 20
    #define MAX_PIN_DAC 2
    #define MAX_PIN_ESP 40

    enum class GpioConfig : uint8_t{
        // Antriebe
        pinMotor            = 14,   // PWM - Antriebsmotor
        pinSteer            = 13,   // PWM - Servo Lenkung
        pinGear             = 12,   // PWM - Servo Transmission

        // Sonstige Aktoren
        pinWinch            = 27,   // PWM - Servo Winde
        pinShaker           = 23,   // PWM - Servo Shaker

        // Sound
        pinSoundDAC1        = 25,   // DAC - Sound Kanal 1
        pinSoundDAC2        = 26,   // DAC - Sound Kanal 2

        // Licht
        pinHeadLight        = 3,    // PWM - Licht Scheinwerfer
        // pinBeamLight        = 99,   // PWM - Licht Fernlicht
        pinRearLight        = 4,    // PWM - Licht Rücklicht
        // pinBrakeLight       = 99,   // PWM - Licht Bremslicht
        pinBlinkLeft        = 15,   // PWM - Blinken Links
        pinBlinkRight       = 5,    // PWM - Blinken Rechts
        pinReverseLight     = 2,    // Dig - Licht Rückfahr
        pinPositionLight    = 18,   // Dig - Licht Position
        pinWorkLight        = 0,    // Dig - Licht Arbeitslicht
        pinCabin            = 19,   // Dig - Licht Tacho Innenraum
        // pinPipeFire         = 99,   // PWM - Auspuff

        // Sensoren
        // pinBatAdc           = 99,   // Alg - Batteriespannung
        // pinSpeedSensorFL    = 99,   // Dig - Raddrehzahl
        // pinSpeedSensorFR    = 99,   // Dig - Raddrehzahl
        // pinSpeedSensorRL    = 99,   // Dig - Raddrehzahl
        // pinSpeedSensorRRv   = 99,   // Dig - Raddrehzahl

        // RC Communication  / UART
        // pinCRSFRx           = 99,   // UART - Receive RC Controller
        // pinCRSFTx           = 99,   // UART - Transmit RC Controller

        // Gyro / I2C
        // pinI2CRx            = 99,   // I2C - Receive Gyro
        // pinI2CTx            = 99,   // I2C - Transmit Gyro
    };

#endif



// ---------------------------------------------------------
//  Queue Einstellungen:
//  - Queue
//  - Sendeprotokoll
//  - Remote Control
//  - ESP Controller
//  - Debug 
//
//  *** Hier müssen Anpassungen gemacht werden ***
// ---------------------------------------------------------



// ---------------------------------------------------------
//  Datenstruktur: RC Eingaben zu qRCCom Queue
// ---------------------------------------------------------
enum class RcLinkState : uint8_t {
    UpLink,
    DownLink,       // timeout erkannt
    WrongValues        // Falsche Raw Werte
};

struct RcFrameData {
    uint32_t timestampMs;                 // Zeitpunkt der Messung/Entscheidung
    uint16_t channel[CRSF_NUM_CHANNELS];   // 1000..2000 (geclamped)
    RcLinkState state;              // Link-Status
   };


// ---------------------------------------------------------
//  Datenstruktur: GpioData zu qControl Queue
// ---------------------------------------------------------
struct PinControl {
  uint8_t  pin;     // echte GPIO-Nummer
  uint16_t value;   // RC: 1000..2000, 
};

struct GpioData {
  uint32_t tick;     // optional: xTaskGetTickCount()
  uint8_t  failsafe; // 0/1

  uint8_t gpioCount;
  PinControl gpioCtrl[MAX_PIN_ESP];
};



// ---------------------------------------------------------
//  Datenstruktur: Sensordaten zu qSensor Queue
// ---------------------------------------------------------
struct SensorData {
    float yaw, pitch, roll;
    float batteryVoltage;
    float wheelRpmFL, wheelRpmFR, wheelRpmRL, wheelRpmRR;
    uint32_t timestampMs;
};

// ---------------------------------------------------------
//  Datenstruktur: Telemetrydaten zu qTelemetry Queue
// ---------------------------------------------------------
struct TelemetryData {
    float     speedKph;
    float     batteryVoltage;
    float     yaw;
    uint8_t   mode;
    uint32_t  timestampMs;
};

// ---------------------------------------------------------
//  Benötigte Queues für Datenaustausch zwischen den Tasks
// ---------------------------------------------------------
extern QueueHandle_t q_CRSF;    // RcInputData: RCCom → ControlSimTask
extern QueueHandle_t q_Gpio;  // ControlCommandData: ControlSimTask → ActuatorTask
extern QueueHandle_t q_Sensor;   // SensorData:  SensorTask → ControlSimTask
extern QueueHandle_t q_Telemetry;     // TelemetryData: ControlSimTask → RCCom

void initQueues();              // in setup() aufrufen


// ---------------------------------------------------------
//  Task Einstellungen:
//  - Timing in Millisecond
// ---------------------------------------------------------
struct TaskTimingConfig {
    uint16_t crsfTaskCycleMs;      // RcCom Zyklus
    uint16_t ctrlTaskCycleMs;    // CtrlSim Zyklus
    uint16_t sensorTaskCycleMs;  // SensorTask Zyklus
    uint16_t gpioTaskCycleMs;// AktorTask Zyklus
};
extern const TaskTimingConfig cfg_taskTimings;

// ----------------------------------------------------------
//  RC Kanal Einstellungen:
//  *** Zentrales Mapping Tabelle ist in der Config.cpp
// ----------------------------------------------------------

enum class InputType : uint8_t {
    Analog,        // Stick/Poti
    TwoPos,        // 2-Position
    ThreePos,      // 3-Position
    Momentary,      // Taster (Impuls)
    InFunction
};

enum class OutputType : uint8_t {
  PWM,           // Servo/ESC (1000-2000us)
  Digital,        // GPIO HIGH/LOW
  DAC
};

enum class OutputHardware : uint8_t {
  Motor,           // Servo/ESC (1000-2000us)
  MotorESC,        // GPIO HIGH/LOW
  Servo,
  LED,
  Sound,
  Digital
};

enum class ChannelIndex : uint8_t {
  Channel1 = 0,
  Channel2 = 1,
  Channel3 = 2,
  Channel4 = 3,
  Channel5 = 4,
  Channel6 = 5,
  Channel7 = 6,
  Channel8 = 7,
  Channel9 = 8,
  Channel10 = 9,
  Channel11 = 10,
  Channel12 = 11,
  Channel13 = 12,
  Channel14 = 13,
  Channel15 = 14,
  Channel16 = 15,
  InFunction = 99  
};

struct RcGpioMap {
  FunctionList      functionList;
  ChannelIndex      channelIndex;     // 1..16 (oder 0..15 je nach deinem Indexing)
  InputType         inputType;
  GpioConfig        gpioConfig;
  OutputType        outputType;
  OutputHardware    outputHardware;
};
extern const RcGpioMap cfg_rcGpioMap[];
extern const size_t MAP_COUNT;








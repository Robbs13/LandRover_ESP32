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
#define CRSF
#define POCKET
#define ESP32AZDEVKITCV4    //ESP32: https://www.az-delivery.de/products/esp-32-dev-kit-c-v4

// ---------------------------------------------------------
//  Konfiguration: Debug
// ---------------------------------------------------------
#define DEBUG_RCCOM
#define DEBUG_CTRLSIM

// ---------------------------------------------------------
//  Konfiguration: Fahrzeug
// ---------------------------------------------------------
#if defined(LANDROVER_TOWTRUCK)
    // Struct für die möglichen Lichtfunktionen
    struct VehicleFeatureConfig {
        bool enableLights;
        bool enableHeadBeam;
        bool enableRearBrake;
        bool enableReverse;
        bool enableBlink;
        bool enableSide;
        bool enableWork;
        bool enableCabine;
        bool enablePipeFire;  

        bool enableDrive;
        bool enableSteering;
        bool enableGear;
        bool enableWinch;
        bool enableShaker;
        bool enableSound;

        bool enableUndefined;
    };
    extern const VehicleFeatureConfig cfg_vehicleFeature;
    // extern const VehicleLightFeatureConfig cfg_vehicleLightFeature;

    // // Struct für die möglichen Controlfunktionen
    // struct VehicleControlFeatureConfig {
    //     bool enableDrive;
    //     bool enableSteering;
    //     bool enableGear;
    //     bool enableWinch;
    //     bool enableShaker;
    //     bool enableSound;
    // };
    // extern const VehicleControlFeatureConfig cfg_vehicleControlFeature;

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

// ---------------------------------------------------------
//  Konfiguration: Sendeformat
// ---------------------------------------------------------
#if defined(CRSF)
    // struct RcComConfig {
    //     uint16_t    RC_FAILSAFE_TIMEOUT_MS;
    //     uint16_t    RC_FAILSAFE_CYCLE;
    // };
    // extern const RcComConfig cfg_RcCom;

    // ----------------------------------------------------------
    //  RC Wertebereiche (zentrale Standardwerte)
    // ----------------------------------------------------------
    constexpr int CRSF_BAUDRATE            = 420000;
    constexpr uint8_t CRSF_PACKET_LEN          = 24;
    constexpr uint8_t CRSF_CHANNEL             = 16;

    constexpr uint16_t RC_MIN                   = 1000;     //CRFS Raw Values 172
    constexpr uint16_t RC_MAX                   = 2000;     //CRFS Raw Values 1810
    constexpr uint16_t RC_MIDDLE                 = 1500;    //CRFS Raw Values 992

    constexpr uint16_t RC_FAILSAFE_TIMEOUT_MS   = 300;
    constexpr uint16_t RC_FAILSAFE_CYCLE        = 1000;
#endif

// ---------------------------------------------------------
//  Konfiguration: Remote Control
// ---------------------------------------------------------
#if defined(POCKET)
    // ----------------------------------------------------------
    //  RC Input Typen (Signalart)
    // ----------------------------------------------------------
    enum class RcInputType : uint8_t
    {
        Analog,        // Stick / Potentiometer
        Switch2Pos,    // 2-Position Schalter
        Switch3Pos,    // 3-Position Schalter
        Momentary,     // Taster
        Unused
    };
    
#endif

// ---------------------------------------------------------
//  Konfiguration:  ESP Controller
// ---------------------------------------------------------
#if defined(ESP32AZDEVKITCV4)
    struct GpioConfig {
        // Antriebe
        uint8_t pinMotorPwm;        // PWM - Antriebsmotor
        uint8_t pinSteerPwm;        // PWM - Servo Lenkung
        uint8_t pinGearPwm;         // PWM - Servo Transmission

        // Sonstige Aktoren
        uint8_t pinWinchPwm;        // PWM - Servo Winde
        uint8_t pinShakerPwm;       // PWM - Servo Shaker

        // Sound
        uint8_t pinSoundDAC1;       // DAC - Sound Kanal 1
        uint8_t pinSoundDAC2;       // DAC - Sound Kanal 2

        // Licht
        int8_t pinHeadLightPwm;     // PWM - Licht Scheinwerfer
        int8_t pinBeamLightPwm;     // PWM - Licht Fernlicht
        uint8_t pinRearLightPwm;    // PWM - Licht Rücklicht
        uint8_t pinBrakeLightPwm;   // PWM - Licht Bremslicht
        uint8_t pinBlinkLeft;       // PWM - Blinken Links
        uint8_t pinBlinkRight;      // PWM - Blinken Rechts
        uint8_t pinReverseLight;    // Dig - Licht Rückfahr
        uint8_t pinPositionLight;   // Dig - Licht Position
        uint8_t pinWorkLight;       // Dig - Licht Arbeitslicht
        uint8_t pinTachoInt;        // Dig - Licht Tacho Innenraum
        uint8_t pinPipeFire;        // PWM - Auspuff

        // Sensoren
        uint8_t pinBatAdc;          // Alg - Batteriespannung
        uint8_t pinSpeedSensorFL;   // Dig - Raddrehzahl
        uint8_t pinSpeedSensorFR;   // Dig - Raddrehzahl
        uint8_t pinSpeedSensorRL;   // Dig - Raddrehzahl
        uint8_t pinSpeedSensorRR;   // Dig - Raddrehzahl

        // RC Communication  / UART
        uint8_t pinCRSFRx;          // UART - Receive RC Controller
        uint8_t pinCRSFTx;          // UART - Transmit RC Controller

        // Gyro / I2C
        uint8_t pinI2CRx;           // I2C - Receive Gyro
        uint8_t pinI2CTx;           // I2C - Transmit Gyro

    };

    extern const GpioConfig cfg_GPIO;
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
struct RcInputData {
    bool FailSafeRC;
    uint16_t channel[CRSF_CHANNEL];   // Anzahl der Kanäle 
    uint32_t timestampMs;   // Timestamp der Nachricht
    };

// ---------------------------------------------------------
//  Datenstruktur: Controldaten zu qControl Queue
// ---------------------------------------------------------
struct ControlCommandData {
    bool        blinkenRightAct;
    bool        blinkenLeftAct;
    bool        lightHeadAct;
    bool        lightBeamAct;
    bool        lightRearAct;
    bool        lightBrakeAct;
    bool        lightReverseAct;
    bool        lightPosAct;
    bool        lightWorkAct;
    bool        lightTachoAct;
    uint16_t    ctrlDrive;
    uint16_t    ctrlSteer;
    uint16_t    ctrlTransmission;
    uint16_t    ctrlWinch;
    uint16_t    ctrlShaker;
    bool        ctrlSound;
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
extern QueueHandle_t qRCCom;    // RcInputData: RCCom → ControlSimTask
extern QueueHandle_t qControl;  // ControlCommandData: ControlSimTask → ActuatorTask
extern QueueHandle_t qSensor;   // SensorData:  SensorTask → ControlSimTask
extern QueueHandle_t qTelemetry;     // TelemetryData: ControlSimTask → RCCom

void initQueues();              // in setup() aufrufen


// ---------------------------------------------------------
//  Task Einstellungen:
//  - Timing in Millisecond
// ---------------------------------------------------------
struct TaskTimingConfig {
    uint16_t rcTaskCycleMs;      // RcCom Zyklus
    uint16_t ctrlTaskCycleMs;    // CtrlSim Zyklus
    uint16_t sensorTaskCycleMs;  // SensorTask Zyklus
    uint16_t actuatorTaskCycleMs;// AktorTask Zyklus
};
extern const TaskTimingConfig cfg_taskTimings;

// ----------------------------------------------------------
//  RC Kanal Einstellungen:
//  - zentrale Mappings Tabelle
// ----------------------------------------------------------
struct RcChannelConfig
{
    uint8_t     channelId;   // CRSF channel index (0..15)
    RcInputType type;        // Signalart
    uint16_t    min;         // Min-Wert
    uint16_t    max;         // Max-Wert
    uint16_t    center;      // Mittelstellung

    //VehicleFeatureConfig  function;    // Funktionszuordnung
};
extern const RcChannelConfig cfg_rcChannels[];
extern const size_t          cfg_rcChannelCount;

const RcChannelConfig* getRcChannelConfig(uint8_t channelId);





// Hilfsfunktionen (optional)
// const RcChannelConfig* getRcChannelConfig(uint8_t channelId);

// ---------------------------------------------------------
//  Crossfire CRSF Verbindung zu RC Sender (Pocket):
//  Hier stehen die benötigten Daten
//  11 Bit: 0..2047 pro Kanal
// ---------------------------------------------------------

// #define CRSF_BAUDRATE 420000
// #define CRSF_PACKET_LEN 24
// #define CRSF_CHANNEL 16                 // Anzahl der Kanäle
// #define RC_FAILSAFE_TIMEOUT_MS 300      // nach ..ms ohne Frame => Failsafe
// #define RC_FAILSAFE_CYCLE 1000      // nach ..ms ohne Frame => Failsafe
// #define RC_MAX 2000
// #define RC_MIN 0
// #define RC_MIDDLE 1000
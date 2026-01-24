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
    #define MAX_PIN_PWM 18
    #define MAX_PIN_DIG 20
    #define MAX_PIN_DAC 2

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
// struct ControlCommandData {
//     bool FailSafeCtrl;
//     uint32_t timestampMs;   // Timestamp der Nachricht
//     //FunctionList funcList;
//     uint16_t pinOut[static_cast<size_t>(FunctionList::count)];
//     uint16_t Value[static_cast<size_t>(FunctionList::count)];
// };

struct PinControl {
  uint8_t  pin;     // echte GPIO-Nummer
  uint16_t value;   // RC: 1000..2000, 
};

struct GpioData {
  uint32_t tick;     // optional: xTaskGetTickCount()
  uint8_t  failsafe; // 0/1

  uint8_t pwmCount;
  uint8_t digCount;
  uint8_t dacCount;

  PinControl pwm[MAX_PIN_PWM];
  PinControl dig[MAX_PIN_DIG];
  PinControl dac[MAX_PIN_DAC];
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
  FunctionList  functionList;
  ChannelIndex  channelIndex;     // 1..16 (oder 0..15 je nach deinem Indexing)
  InputType     inputType;
  GpioConfig    gpioConfig;
  OutputType    outputType;
};
extern const RcGpioMap cfg_rcGpioMap[];
extern const size_t MAP_COUNT;








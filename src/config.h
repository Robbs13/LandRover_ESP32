#pragma once

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


// ---------------------------------------------------------
//  Fahrzeug Features:
//  Hier werden die möglichen Funktionen aktiviert
//
//  *** Hier müssen Anpassungen gemacht werden ***
// ---------------------------------------------------------
 
//*** Licht Funktionen
//  #define FUNC_HEADLIGHTS
#define FUNC_HEADLIGHTS_BEAM
//  #define FUNC_REARLIGHTS
#define FUNC_REARLIGHTS_BRAKE
//  #define FUNC_BRAKELIGHTS
#define FUNC_REVERSELIGHT
#define FUNC_BLINK
#define FUNC_POSLIGHTS
#define FUNC_WORKLIGHT
#define FUNC_TACHOLIGHT
#define FUNC_PIPEFIRE

//*** Fahr Funktionen
#define FUNC_THROTTLE_SIM
#define FUNC_STEERING_SIM
#define FUNC_TRANSMISSION

//*** Weitere Funktionen
#define FUNC_WINCH
#define FUNC_SHAKER
#define FUNC_SOUND


// ---------------------------------------------------------
//  Fahrzeug IO:
//  Hier werden die möglichen In und Outputs aktiviert
// ---------------------------------------------------------

//*** Inputs
#define Gyro
// #define SensRPM
#define BattStatus


// ---------------------------------------------------------
//  Crossfire CRSF Verbindung zu RC Sender (Pocket):
//  Hier stehen die benötigten Daten
//  11 Bit: 0..2047 pro Kanal
// ---------------------------------------------------------

#define CRSF_BAUDRATE 420000
#define CRSF_PACKET_LEN 24
#define CRSF_CHANNEL 16                 // Anzahl der Kanäle
#define RC_FAILSAFE_TIMEOUT_MS 300      // nach ..ms ohne Frame => Failsafe
#define RC_FAILSAFE_CYCLE 1000      // nach ..ms ohne Frame => Failsafe

// ---------------------------------------------------------
//  Aktivieren der Debug Ausgaben
// ---------------------------------------------------------
#define DEBUG_RCCOM

// ---------------------------------------------------------
//  Simulationsparameter Fahrzeug:
//  Werte vom realen Fahrzeug
// ---------------------------------------------------------

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



// ---------------------------------------------------------
//  RC-Channel-Konfiguration:
//  Auswahl RC und Zuordnung der Kanäle
//
//  *** Hier müssen Anpassungen gemacht werden ***
// ---------------------------------------------------------

#define POCKET
//  #define Baert3000

#if defined(POCKET)

    enum class RcInputMapper : uint8_t {
        ctlSteering,        // CH1 - Analog Stick - Right X
        ch2,                // CH2 - Analog Stick - Right Y
        ch3,                // CH3 - Analog Stick - Left X
        ctlDrive,           // CH4 - Analog Stick - Left Y
        ctlLight,           // CH5 - Schalter - SA
        ctlBlink,           // CH6 - 3 Wege Schalter - SB
        ctlTransmission,    // CH7 - 3 Wege Schalter - SC
        ctlWorkLight,       // CH8 - Schalter - SD
        ctlBeam,            // CH9 - Taster - SE 
        ctlWinch,           // CH10 - Potentiometer - S1
        ch11,   // CH11 - Nicht definiert
        ch12,   // CH12 - Nicht definiert
        ch13,   // CH13 - Nicht definiert
        ch14,   // CH14 - Nicht definiert
        ch15,   // CH15 - Nicht definiert
        ch16    // CH16 - Nicht definiert
    };

    enum class RcInputType : uint8_t {
        Analog,         // Stick, Potentiometer
        Switch2,        // 2-Stufen-Schalter
        Switch3,        // 3-Stufen-Schalter
        Taster,         // Taster
        Potentiometer,  // Potentiometer
        Undefinied      // Undefiniert
    };

    // enum class RcInputType : uint8_t {
    //     Analog,         // CH1 - Analog Stick - Right X
    //     Analog,         // CH2 - Analog Stick - Right Y
    //     Analog,         // CH3 - Analog Stick - Left X
    //     Analog,         // CH4 - Analog Stick - Left Y
    //     Switch2,        // CH5 - Schalter - SA
    //     Switch3,        // CH6 - 3 Wege Schalter - SB
    //     Switch3,        // CH7 - 3 Wege Schalter - SC
    //     Switch2,        // CH8 - Schalter - SD
    //     Taster,         // CH9 - Taster - SE 
    //     Potentiometer,  // CH10 - Potentiometer - S1
    //     Undefinied,     // CH11 - Nicht definiert
    //     Undefinied,     // CH12 - Nicht definiert
    //     Undefinied,     // CH13 - Nicht definiert
    //     Undefinied,     // CH14 - Nicht definiert
    //     Undefinied,     // CH15 - Nicht definiert
    //     Undefinied      // CH16 - Nicht definiert
    // };

#endif





// ---------------------------------------------------------
//  IO-Belegung ESP32:
//  Zuordnung der einzelnen Input/Output
// ---------------------------------------------------------

#define ESP32AZDEVKITCV4    //https://www.az-delivery.de/products/esp-32-dev-kit-c-v4

#if defined(ESP32AZDEVKITCV4)
    struct GpioConfig {
        // Antriebe
        int pinMotorPwm;        // PWM - Antriebsmotor
        int pinSteerPwm;        // PWM - Servo Lenkung
        int pinTransmissionPwm; // PWM - Servo Transmission

        // Sonstige Aktoren
        int pinWinchPwm;        // PWM - Servo Winde
        int pinShakerPwm;       // PWM - Servo Shaker

        // Sound
        int pinSoundDAC1;       // DAC - Sound Kanal 1
        int pinSoundDAC2;       // DAC - Sound Kanal 2

        // Licht
        int pinHeadLightPwm;    // PWM - Licht Scheinwerfer
        // int pinBeamLightPwm;    // PWM - Licht Fernlicht
        int pinRearLightPwm;    // PWM - Licht Rücklicht
        // int pinBrakeLightPwm;   // PWM - Licht Bremslicht
        int pinBlinkLeft;       // PWM - Blinken Links
        int pinBlinkRight;      // PWM - Blinken Rechts
        int pinReverseLight;    // Dig - Licht Rückfahr
        int pinPositionLight;   // Dig - Licht Position
        int pinWorkLight;       // Dig - Licht Arbeitslicht
        int pinTachoInt;        // Dig - Licht Tacho Innenraum
        // int pinPipeFire;        // PWM - Auspuff

        // Sensoren
        int pinBatAdc;          // Alg - Batteriespannung
        int pinSpeedSensorFL;   // Dig - Raddrehzahl
        int pinSpeedSensorFR;   // Dig - Raddrehzahl
        int pinSpeedSensorRL;   // Dig - Raddrehzahl
        int pinSpeedSensorRR;   // Dig - Raddrehzahl

        // RC Communication  / UART
        int pinXlrsRx;      // UART - Receive RC Controller
        int pinXlrsTx;      // UART - Transmit RC Controller

        // Gyro / I2C
        int pinI2CRx;       // I2C - Receive Gyro
        int pinI2CTx;       // I2C - Transmit Gyro

    };

    extern const GpioConfig cfg_ioPins;

#endif
// ---------------------------------------------------------
// Gemeinsame Datenstrukturen für Queues
// ---------------------------------------------------------

//*** Eingabe Datenstruktur vom RC Sender
struct RcInputData {
    bool FailSafeRC;
    uint16_t channel[CRSF_CHANNEL];   // Anzahl der Kanäle 
    uint32_t timestampMs;   // Timestamp der Nachricht
    };

//*** Sensordatenstruktur
struct SensorData {
    float yaw, pitch, roll;
    float batteryVoltage;
    float wheelRpmFL, wheelRpmFR, wheelRpmRL, wheelRpmRR;
    uint32_t timestampMs;
};

//*** Telemetriedatenstruktur
struct TelemetryData {
    float     speedKph;
    float     batteryVoltage;
    float     yaw;
    uint8_t   mode;
    uint32_t  timestampMs;
};

//*** Telemetriedatenstruktur
struct ControlCommandData {
    float     speedKph;
    float     batteryVoltage;
    float     yaw;
    uint8_t   mode;
    uint32_t  timestampMs;
};

// Queues werden in Config.cpp erzeugt
extern QueueHandle_t qRCCom;    // RcInputData: RCCom → ControlSimTask
extern QueueHandle_t qSensor;   // SensorData:  SensorTask → ControlSimTask
extern QueueHandle_t qControl;  // ControlCommandData: ControlSimTask → ActuatorTask
extern QueueHandle_t qTelemetry;     // TelemetryData: ControlSimTask → RCCom

void initQueues();              // in setup() aufrufen

// 
struct RcChannelConfig {
    uint8_t     channelId;   // XLRS-Kanalnummer / Index
    RcInputType type;
    int16_t     minRaw;      // z.B. 1000 µs / 0
    int16_t     maxRaw;      // z.B. 2000 µs / 1023
    int16_t     centerRaw;   // Mittelstellung (bei Sticks)
};

// Liste aller verwendeten RC-Kanäle
extern const RcChannelConfig g_rcChannels[];
// extern const size_t          g_rcChannelCount;

// Hilfsfunktionen (optional)
// const RcChannelConfig* getRcChannelConfig(uint8_t channelId);
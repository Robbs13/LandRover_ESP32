#include "Config.h"


// ---------------------------------------------------------
//  Konfiguration: Fahrzeug
//
//  *** Hier müssen Anpassungen gemacht werden ***
// ---------------------------------------------------------

#if defined(LANDROVER_TOWTRUCK)

    const VehicleFeatureConfig cfg_vehicleFeature = {
        .enableLights = true,
        .enableHeadBeam = true,
        .enableRearBrake = true,
        .enableReverse = true,
        .enableBlink = true,
        .enableSide = true,  
        .enableWork = true,
        .enableCabine = true,
        .enablePipeFire = true,

        .enableDrive = true,
        .enableSteering = true,
        .enableGear = true,
        .enableWinch = true,
        .enableShaker = true,
        .enableSound = true,

        .enableUndefined = false,
    };

    // const VehicleControlFeatureConfig cfg_vehicleControlFeature = {
    //     .enableDrive = true,
    //     .enableSteering = true,
    //     .enableGear = true,
    //     .enableWinch = true,
    //     .enableShaker = true,
    //     .enableSound = true,
    // };

    const VehicleSimConfig cfg_vehicleSim = {
        .massKg = 1.0,
        .wheelBaseX = 1.0,
        .wheelBaseY = 1.0,
        .maxSpeed = 1.0,
        .torqueCurve = 1.0,
        .dragCoeff = 1.0,
        .tireGrip = 1.0,
    };
#endif

// ---------------------------------------------------------
//  Konfiguration: Sendeformat
//
//  *** Hier müssen Anpassungen gemacht werden ***
// ---------------------------------------------------------
// #if defined(CRSF)
//     const RcComConfig cfg_RcCom = {
//         .RC_FAILSAFE_TIMEOUT_MS = 300,     // nach ..ms ohne Frame => Failsafe
//         .RC_FAILSAFE_CYCLE = 1000,      // nach ..ms ohne Frame => Failsafe
//     };
// #endif




// ---------------------------------------------------------
//  Konfiguration:  ESP Controller
//
//  *** Hier müssen Anpassungen gemacht werden ***
// ---------------------------------------------------------

#if defined(ESP32AZDEVKITCV4)
const GpioConfig cfg_GPIO = {
    // Antriebe
    .pinMotorPwm        = 14,    // PWM - Antriebsmotor
    .pinSteerPwm        = 13,    // PWM - Servo Lenkung
    .pinGearPwm         = 12,    // PWM - Servo Transmission

    // Sonstige Aktoren
    .pinWinchPwm        = 27,    // PWM - Servo Winde
    .pinShakerPwm       = 23,    // PWM - Servo Shaker

    // Sound
    .pinSoundDAC1       = 25,    // DAC - Sound Kanal 1
    .pinSoundDAC2       = 26,    // DAC - Sound Kanal 2

    // Licht
    .pinHeadLightPwm    = 3,     // PWM - Licht Scheinwerfer
    .pinBeamLightPwm    = 99,    // PWM - Licht Fernlicht
    .pinRearLightPwm    = 4,     // PWM - Licht Rücklicht
    .pinBrakeLightPwm   = 99,    // PWM - Licht Bremslicht
    .pinBlinkLeft       = 15,    // PWM - Blinken Links
    .pinBlinkRight      = 5,     // PWM - Blinken Rechts
    .pinReverseLight    = 2,     // Dig - Licht Rückfahr
    .pinPositionLight   = 18,    // Dig - Licht Position
    .pinWorkLight       = 0,     // Dig - Licht Arbeitslicht
    .pinTachoInt        = 19,    // Dig - Licht Tacho Innenraum
    .pinPipeFire        = 99,    // PWM - Auspuff

    // Sensoren
    .pinBatAdc          = 99,    // Alg - Batteriespannung
    .pinSpeedSensorFL   = 99,    // Dig - Raddrehzahl
    .pinSpeedSensorFR   = 99,    // Dig - Raddrehzahl
    .pinSpeedSensorRL   = 99,    // Dig - Raddrehzahl
    .pinSpeedSensorRR   = 99,    // Dig - Raddrehzahl

    // RC Communication  / UART
    .pinCRSFRx          = 17,    // UART - Receive RC Controller
    .pinCRSFTx          = 16,    // UART - Transmit RC Controller

    // Gyro / I2C
    .pinI2CRx           = 22,    // I2C - Receive Gyro
    .pinI2CTx           = 21,    // I2C - Transmit Gyro
    };
#endif

// ---------------------------------------------------------
//  Queues
// ---------------------------------------------------------

QueueHandle_t q_CRSF   = nullptr;
QueueHandle_t q_Sensor = nullptr;
QueueHandle_t q_Control= nullptr;
QueueHandle_t q_Telemetry   = nullptr;

void initQueues() {
    // latest-only für Steuerung und Sensorik
    q_CRSF      = xQueueCreate(1, sizeof(RcFrameData));
    q_Sensor     = xQueueCreate(1, sizeof(SensorData));

    // Control & Telemetry dürfen etwas gepuffert werden
    q_Control    = xQueueCreate(5, sizeof(ControlCommandData));
    q_Telemetry  = xQueueCreate(5, sizeof(TelemetryData));
}

// ---------------------------------------------------------
//  Task Einstellungen:
//  - Timing in Millisecond
//
//  *** Hier müssen Anpassungen gemacht werden ***
// ---------------------------------------------------------
const TaskTimingConfig cfg_taskTimings = {
    .crsfTaskCycleMs       = 5,
    .ctrlTaskCycleMs     = 10,
    .sensorTaskCycleMs   = 10,
    .actuatorTaskCycleMs = 10
};

// ---------------------------------------------------------
//  RC Kanal Einstellungen:
//  - zentrale Mappings Tabelle
//
//  *** Hier müssen Anpassungen gemacht werden ***
// ---------------------------------------------------------

// #if defined(POCKET)
//     const RcChannelConfig cfg_rcChannels[] = {
//         // channelId, RcInputType,    min,     max,   cnt, Funktion
//         { 1, RcInputType::Analog,       RC_MIN,  RC_MAX,  RC_MIDDLE},  
//         { 2, RcInputType::Analog,       RC_MIN,  RC_MAX,  RC_MIDDLE},    
//         { 3, RcInputType::Analog,       RC_MIN,  RC_MAX,  RC_MIDDLE},   
//         { 4, RcInputType::Analog,       RC_MIN,  RC_MAX,  RC_MIDDLE},     
//         { 5, RcInputType::Switch2Pos,   RC_MIN,  RC_MAX,  RC_MIDDLE},         
//         { 6, RcInputType::Switch3Pos,   RC_MIN,  RC_MAX,  RC_MIDDLE},         
//         { 7, RcInputType::Switch3Pos,   RC_MIN,  RC_MAX,  RC_MIDDLE},
//         { 8, RcInputType::Switch2Pos,   RC_MIN,  RC_MAX,  RC_MIDDLE},
//         { 9, RcInputType::Momentary,    RC_MIN,  RC_MAX,  RC_MIDDLE},
//         {10, RcInputType::Analog,       RC_MIN,  RC_MAX,  RC_MIDDLE},
//         {11, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE},
//         {12, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE},
//         {13, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE},
//         {14, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE},
//         {15, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE},
//         {16, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE}
//     };

    // const RcChannelConfig cfg_rcChannels[] = {
    //     // channelId, RcInputType,    min,     max,   cnt, Funktion
    //     { ctrlLights,   ch1,    RcInputType::Analog},  
    //     { ctrlHeadBeam, ch4,    RcInputType::Analog},    
    //     { ctrlRearBrake, RcInputType::Analog,       RC_MIN,  RC_MAX,  RC_MIDDLE},   
    //     { ctrlReverse, RcInputType::Analog,       RC_MIN,  RC_MAX,  RC_MIDDLE},     
    //     { 5, RcInputType::Switch2Pos,   RC_MIN,  RC_MAX,  RC_MIDDLE},         
    //     { 6, RcInputType::Switch3Pos,   RC_MIN,  RC_MAX,  RC_MIDDLE},         
    //     { 7, RcInputType::Switch3Pos,   RC_MIN,  RC_MAX,  RC_MIDDLE},
    //     { 8, RcInputType::Switch2Pos,   RC_MIN,  RC_MAX,  RC_MIDDLE},
    //     { 9, RcInputType::Momentary,    RC_MIN,  RC_MAX,  RC_MIDDLE},
    //     {10, RcInputType::Analog,       RC_MIN,  RC_MAX,  RC_MIDDLE},
    //     {11, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE},
    //     {12, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE},
    //     {13, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE},
    //     {14, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE},
    //     {15, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE},
    //     {16, RcInputType::Unused,       RC_MIN,  RC_MAX,  RC_MIDDLE}
    // };
//#endif

// const size_t g_rcChannelCount =
//     sizeof(cfg_rcChannels) / sizeof(cfg_rcChannels[0]);

// const RcChannelConfig* getRcChannelConfig(uint8_t channelId) {
//     for (size_t i = 0; i < g_rcChannelCount; ++i) {
//         if (cfg_rcChannels[i].channelId == channelId) {
//             return &cfg_rcChannels[i];
//         }
//     }
//     return nullptr;
//}




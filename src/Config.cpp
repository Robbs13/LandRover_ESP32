#include "Config.h"


// ---------------------------------------------------------
//  Simulationsparameter Fahrzeug:
//  Werte vom realen Fahrzeug
//
//  *** Hier müssen Anpassungen gemacht werden ***
// ---------------------------------------------------------

const VehicleSimConfig cfg_vehicleSim = {
    .massKg         = 999.0f,   // Fahrzeugmasse in kg
    .wheelBaseX     = 99.0f,    // Spurweite in m
    .wheelBaseY     = 199.0f,   // Spurstand in m
    .maxSpeed       = 99.0f,    // theoretische Max-Speed in kmh
    .torqueCurve    = 99.0f,    // Motorleistung (vereinfacht) nun... noch keine Ahnung
    .dragCoeff      = 0.9f,     // 0..1, Luft-/Rollwiderstand
    .tireGrip       = 1.0f,     // 0..1, Grip-Faktor
};


// ---------------------------------------------------------
//  IO-Belegung ESP32:
//  Zuordnung der einzelnen Input/Output
// ---------------------------------------------------------


#if defined(ESP32AZDEVKITCV4)
const GpioConfig cfg_ioPins = {
    // Antriebe
    .pinMotorPwm        = 14,    // PWM - Antriebsmotor
    .pinSteerPwm        = 13,    // PWM - Servo Lenkung
    .pinTransmissionPwm = 12,    // PWM - Servo Transmission

    // Sonstige Aktoren
    .pinWinchPwm        = 27,    // PWM - Servo Winde
    .pinShakerPwm        = 23,    // PWM - Servo Shaker

    // Sound
    .pinSoundDAC1       = 25,    // DAC - Sound Kanal 1
    .pinSoundDAC2       = 26,    // DAC - Sound Kanal 2

    // Licht
    .pinHeadLightPwm    = 3,     // PWM - Licht Scheinwerfer
    // int pinBeamLightPwm;    // PWM - Licht Fernlicht
    .pinRearLightPwm    = 4,     // PWM - Licht Rücklicht
    // int pinBrakeLightPwm;   // PWM - Licht Bremslicht
    .pinBlinkLeft       = 15,    // PWM - Blinken Links
    .pinBlinkRight      = 5,     // PWM - Blinken Rechts
    .pinReverseLight    = 2,     // Dig - Licht Rückfahr
    .pinPositionLight   = 18,    // Dig - Licht Position
    .pinWorkLight       = 0,    // Dig - Licht Arbeitslicht
    .pinTachoInt        = 19,    // Dig - Licht Tacho Innenraum
    // int pinPipeFire;        // PWM - Auspuff

    // Sensoren
    .pinBatAdc          = 00,    // Alg - Batteriespannung
    .pinSpeedSensorFL   = 00,    // Dig - Raddrehzahl
    .pinSpeedSensorFR   = 00,    // Dig - Raddrehzahl
    .pinSpeedSensorRL   = 00,   // Dig - Raddrehzahl
    .pinSpeedSensorRR   = 00,    // Dig - Raddrehzahl

    // RC Communication  / UART
    .pinXlrsRx      = 17,    // UART - Receive RC Controller
    .pinXlrsTx      = 16,    // UART - Transmit RC Controller

    // Gyro / I2C
    .pinI2CRx       = 22,    // I2C - Receive Gyro
    .pinI2CTx       = 21,    // I2C - Transmit Gyro

};

#endif

// ---------------------------------------------------------
//  Queues
// ---------------------------------------------------------

QueueHandle_t qRCCom   = nullptr;
QueueHandle_t qSensor = nullptr;
QueueHandle_t qControl= nullptr;
QueueHandle_t qTelemetry   = nullptr;

void initQueues() {
    // latest-only für Steuerung und Sensorik
    qRCCom      = xQueueCreate(1, sizeof(RcInputData));
    qSensor     = xQueueCreate(1, sizeof(SensorData));

    // Control & Telemetry dürfen etwas gepuffert werden
    qControl    = xQueueCreate(5, sizeof(ControlCommandData));
    qTelemetry  = xQueueCreate(5, sizeof(TelemetryData));
}

// ---------------------------------------------------------
//  RC-Channel-Konfiguration
// ---------------------------------------------------------

const RcChannelConfig g_rcChannels[] = {
    // channelId, RcInputType,          min,   max,   center
    { 1, RcInputType::Analog,          1000,  2000,  1500 }, // Gas
    { 2, RcInputType::Analog,          1000,  2000,  1500 }, // Lenkung
    { 3, RcInputType::Switch3,         1000,  2000,  1500 }, // Fahrmodus (3-stufig)
    { 4, RcInputType::Taster,          1000,  2000,  1500 }, // Winde
    { 5, RcInputType::Switch2,         1000,  2000,  1000 }, // Licht an/aus
    { 6, RcInputType::Switch2,         1000,  2000,  1000 }, // Getriebe High/Low
};

const size_t g_rcChannelCount =
    sizeof(g_rcChannels) / sizeof(g_rcChannels[0]);

const RcChannelConfig* getRcChannelConfig(uint8_t channelId) {
    for (size_t i = 0; i < g_rcChannelCount; ++i) {
        if (g_rcChannels[i].channelId == channelId) {
            return &g_rcChannels[i];
        }
    }
    return nullptr;
}




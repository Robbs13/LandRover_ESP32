#include "Config.h"


/** ---------------------------------------------------------
 * Konfiguration: Fahrzeug
 *      - Vehicle Simulations Werte
 *      - zentrale Mappings Tabelle
 * 
 *  *** Hier müssen Anpassungen gemacht werden ***
 */

#if defined(LANDROVER_TOWTRUCK)

    const VehicleSimConfig cfg_vehicleSim = {
        .massKg = 1.0,
        .wheelBaseX = 1.0,
        .wheelBaseY = 1.0,
        .maxSpeed = 1.0,
        .torqueCurve = 1.0,
        .dragCoeff = 1.0,
        .tireGrip = 1.0,
    };

    const RcGpioMap cfg_rcGpioMap[] = {
        // Function                         RC Kanal                    RC Input Typ            GPIO PIN                        GPIO Ouput Typ
        {FunctionList::handleDrive,         ChannelIndex::Channel3,     InputType::Analog,      GpioConfig::pinMotor,           OutputType::PWM,    OutputHardware::MotorESC},
        {FunctionList::handleSteer,         ChannelIndex::Channel1,     InputType::Analog,      GpioConfig::pinSteer,           OutputType::PWM,    OutputHardware::Servo},
        {FunctionList::handleGear,          ChannelIndex::Channel7,     InputType::ThreePos,    GpioConfig::pinGear,            OutputType::PWM,    OutputHardware::Servo},
        {FunctionList::handleWinch,         ChannelIndex::Channel10,    InputType::Analog,      GpioConfig::pinWinch,           OutputType::PWM,    OutputHardware::Servo},
        {FunctionList::handleShaker,        ChannelIndex::InFunction,   InputType::InFunction,  GpioConfig::pinShaker,          OutputType::PWM,    OutputHardware::MotorESC},
        {FunctionList::handleSound1,        ChannelIndex::InFunction,   InputType::InFunction,  GpioConfig::pinSoundDAC1,       OutputType::DAC,    OutputHardware::Sound},
        {FunctionList::handleSound2,        ChannelIndex::InFunction,   InputType::InFunction,  GpioConfig::pinSoundDAC2,       OutputType::DAC,    OutputHardware::Sound},
        {FunctionList::handleBlinkLeft,     ChannelIndex::Channel6,     InputType::ThreePos,    GpioConfig::pinBlinkLeft,       OutputType::PWM,    OutputHardware::LED},
        {FunctionList::handleBlinkRight,    ChannelIndex::Channel6,     InputType::ThreePos,    GpioConfig::pinBlinkRight,      OutputType::PWM,    OutputHardware::LED},
        {FunctionList::handleLightHead,     ChannelIndex::Channel5,     InputType::TwoPos,      GpioConfig::pinHeadLight,       OutputType::PWM,    OutputHardware::LED},
        {FunctionList::handleLightBeam,     ChannelIndex::Channel9,     InputType::Momentary,   GpioConfig::pinHeadLight,       OutputType::PWM,    OutputHardware::LED},
        {FunctionList::handleLightRear,     ChannelIndex::Channel5,     InputType::TwoPos,      GpioConfig::pinRearLight,       OutputType::PWM,    OutputHardware::LED},
        {FunctionList::handleLightBrake,    ChannelIndex::InFunction,   InputType::InFunction,  GpioConfig::pinRearLight,       OutputType::PWM,    OutputHardware::LED},
        {FunctionList::handleLightReverse,  ChannelIndex::InFunction,   InputType::InFunction,  GpioConfig::pinReverseLight,    OutputType::PWM,    OutputHardware::LED},
        {FunctionList::handleLightPos,      ChannelIndex::Channel5,     InputType::TwoPos,      GpioConfig::pinPositionLight,   OutputType::PWM,    OutputHardware::LED},
        {FunctionList::handleLightWork,     ChannelIndex::Channel8,     InputType::TwoPos,      GpioConfig::pinWorkLight,       OutputType::PWM,    OutputHardware::LED},
        {FunctionList::handleLightCabin,    ChannelIndex::Channel5,     InputType::TwoPos,      GpioConfig::pinCabin,           OutputType::PWM,    OutputHardware::LED}

    };
#endif
constexpr size_t MAP_COUNT = sizeof(cfg_rcGpioMap) / sizeof(cfg_rcGpioMap[0]);



// ---------------------------------------------------------
//  Queues
// ---------------------------------------------------------

QueueHandle_t q_CRSF   = nullptr;
QueueHandle_t q_Sensor = nullptr;
QueueHandle_t q_Gpio = nullptr;
QueueHandle_t q_Telemetry   = nullptr;

void initQueues() {
    // latest-only für Steuerung und Sensorik
    //q_CRSF      = xQueueCreate(1, sizeof(RcFrameData));
    q_CRSF      = xQueueCreate(1, sizeof(RcFrameData));
    q_Sensor     = xQueueCreate(1, sizeof(SensorData));

    // Control & Telemetry dürfen etwas gepuffert werden
    q_Gpio    = xQueueCreate(1, sizeof(GpioData));
    q_Telemetry  = xQueueCreate(5, sizeof(TelemetryData));
}

// ---------------------------------------------------------
//  Task Einstellungen:
//  - Timing in Millisecond
//
//  *** Hier müssen Anpassungen gemacht werden ***
//  Bereich von 5 - 255ms möglich
// ---------------------------------------------------------
const TaskTimingConfig cfg_taskTimings = {
    .crsfTaskCycleMs     = 5,
    .ctrlTaskCycleMs     = 10,
    .sensorTaskCycleMs   = 10,
    .gpioTaskCycleMs = 10
};





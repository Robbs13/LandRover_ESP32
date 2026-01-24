#include <Arduino.h>
#include "Config.h"
#include "CRSF_serial.h"
#include "CtrlSim.h"
#include "GpioHandle.h"



CRSF_serial m_crsf(1);
CtrlSim m_ctrlSim(0);
GpioHandle m_gpio(0);

void setup() {
// Debug-Serielle (optional)
    Serial.begin(115200);

    initQueues();


    // Tasks starten – Logik steckt in den Klassen
    m_crsf.startTask(3, 0, cfg_taskTimings.crsfTaskCycleMs);        // prio 3, Core 0, 
    m_ctrlSim.startTask(4, 1, cfg_taskTimings.ctrlTaskCycleMs);   // prio 4, Core 1 (Regelung)
    // m_sensorCom.startTask(3, 0, cfg_taskTimings.sensorTaskCycleMs);    // prio 3, Core 0
    m_gpio.startTask(2, 0, cfg_taskTimings.gpioTaskCycleMs);     // prio 2, Core 1
    Serial.println("System ready");
}

void loop() {

    
    vTaskDelay(portMAX_DELAY); // nichts zu tun
}


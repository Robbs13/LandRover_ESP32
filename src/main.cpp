#include <Arduino.h>
#include "Config.h"
#include "RcCom.h"       // Klasse RcCom



RcCom g_rcCom(1);

void setup() {
// Debug-Serielle (optional)
    Serial.begin(115200);

    initQueues();


    // Tasks starten – Logik steckt in den Klassen
    g_rcCom.startTask(3, 0, 5);        // prio 3, Core 0, 
    // g_sensorCom.startTask(3, 0);    // prio 3, Core 0
    // g_controlSim.startTask(4, 1);   // prio 4, Core 1 (Regelung)
    // g_actuator.startTask(2, 1);     // prio 2, Core 1
    Serial.println("System ready");
}

void loop() {
    vTaskDelay(portMAX_DELAY); // nichts zu tun
}

// void vRCComTask(void *pvParameters);
// void vSensorTask(void *pvParameters);
// void vControlTask(void *pvParameters);
// void vActuatorTask(void *pvParameters);

// RcCom      g_rcCom;

// void setup() {
// Debug-Serielle (optional)
    // Serial.begin(115200);
    // initQueues();


    // Tasks starten – Logik steckt in den Klassen
    // g_rcCom.startTask(3, 0);        // prio 3, Core 0
    // g_sensorCom.startTask(3, 0);    // prio 3, Core 0
    // g_controlSim.startTask(4, 1);   // prio 4, Core 1 (Regelung)
    // g_actuator.startTask(2, 1);     // prio 2, Core 1


    // while (!Serial) {
    //     // auf USB-Serielle warten (bei Bedarf)
    //     delay(10);
    // }
    // Serial.println();
    // Serial.println("=== RC Vehicle Controller starting... ===");

    // // 1) Queues anlegen (in Config.cpp implementiert)
    // initQueues();

    // // Optional: prüfen, ob Queues gültig sind
    // if (qRCCom == nullptr || qSensor == nullptr || qControl == nullptr || qTelemetry == nullptr) {
    //     Serial.println("ERROR: Failed to create one or more queues!");
    //     // Hier könntest du in eine Fehler-LED-Schleife gehen
    // }

    // // 2) FreeRTOS-Tasks starten
    // // RC/XLRS + Sensoren auf Core 0
    // BaseType_t res;

    // res = xTaskCreatePinnedToCore(
    //     vRCComTask,           // Task-Funktion
    //     "RCCom",              // Name (für Debug)
    //     4096,                 // Stack-Größe (Wörter, nicht Bytes)
    //     nullptr,              // Parameter (nicht benötigt)
    //     3,                    // Priorität
    //     nullptr,              // Task-Handle (nicht benötigt)
    //     0                     // Core 0
    // );
    // if (res != pdPASS) {
    //     Serial.println("ERROR: Failed to create RCComTask");
    // }

    // res = xTaskCreatePinnedToCore(
    //     vSensorTask,
    //     "Sensor",
    //     4096,
    //     nullptr,
    //     3,
    //     nullptr,
    //     0                     // Core 0
    // );
    // if (res != pdPASS) {
    //     Serial.println("ERROR: Failed to create SensorTask");
    // }

    // // Control + Aktoren auf Core 1
    // res = xTaskCreatePinnedToCore(
    //     vControlTask,
    //     "Control",
    //     8192,
    //     nullptr,
    //     4,                    // höhere Priorität (Regelung/Simulation)
    //     nullptr,
    //     1                     // Core 1
    // );
    // if (res != pdPASS) {
    //     Serial.println("ERROR: Failed to create ControlTask");
    // }

    // res = xTaskCreatePinnedToCore(
    //     vActuatorTask,
    //     "Actuator",
    //     4096,
    //     nullptr,
    //     2,                    // etwas niedriger als Control
    //     nullptr,
    //     1                     // Core 1
    // );
    // if (res != pdPASS) {
    //     Serial.println("ERROR: Failed to create ActuatorTask");
    // }

    // Serial.println("Tasks started.");


//     Serial.println("System ready");
// }

// void loop() {
//     vTaskDelay(portMAX_DELAY); // nichts zu tun
// }


// void vRCComTask(void *pvParameters)
// {
//     // Initialisierung der RC/XLRS-Kommunikation
//     g_rcCom.begin();

//     const TickType_t period = pdMS_TO_TICKS(5);   // z.B. alle 5 ms Input checken
//     TickType_t lastWake = xTaskGetTickCount();

//     for (;;)
//     {
//         g_rcCom.update();        // XLRS lesen, Roh-/RC-Daten in qRCCom / qRCIn schreiben
//         vTaskDelayUntil(&lastWake, period);
//     }
// }

// // 2) Sensor-Task
// void vSensorTask(void *pvParameters)
// {
//     g_sensorCom.begin();

//     const TickType_t period = pdMS_TO_TICKS(10);  // z.B. 100 Hz Sensor-Update
//     TickType_t lastWake = xTaskGetTickCount();

//     for (;;)
//     {
//         g_sensorCom.update();    // Sensoren lesen, SensorData in qSensor schreiben
//         vTaskDelayUntil(&lastWake, period);
//     }
// }

// // 3) Control + Simulation Task
// void vControlTask(void *pvParameters)
// {
//     g_controlSim.begin();

//     const TickType_t period = pdMS_TO_TICKS(10);  // z.B. 100 Hz Regel-/Sim-Loop
//     TickType_t lastWake = xTaskGetTickCount();

//     for (;;)
//     {
//         g_controlSim.loopStep(); // aus qRCCom + qSensor lesen, Simulation, Control, qControl + qTelemetry schreiben
//         vTaskDelayUntil(&lastWake, period);
//     }
// }

// // 4) Aktor-Task (PWM, Licht, Sound)
// void vActuatorTask(void *pvParameters)
// {
//     g_actuator.begin();

//     // Diese Task kann blockierend auf neue ControlCommands warten
//     for (;;)
//     {
//         g_actuator.loop();   // intern: xQueueReceive(qControl, ...) & apply()
//         // loop() kann selbst xQueueReceive blockierend aufrufen,
//         // daher braucht es hier kein vTaskDelay.
//     }
// }
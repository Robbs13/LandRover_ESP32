#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <HardwareSerial.h>

#define CRSF_BAUDRATE 420000
#define CRSF_PACKET_LEN 24
#define RC_FAILSAFE_TIMEOUT_MS 300     // nach 300ms ohne Frame => Failsafe


//extern HardwareSerial crsf;

class RcCom {
public:
    explicit RcCom(int uartNum = 1);

    void begin();  // z.B. UART initialisieren

    // Startet den FreeRTOS-Task für diese Instanz
    bool startTask(UBaseType_t priority, BaseType_t core);

private:
    // Die eigentliche Task-Schleife (instanzbezogen)
    void taskLoop();

    // Statischer „Trampolin“-Entry für FreeRTOS
    static void taskTrampoline(void *pvParameters);

    // CRSF-Paket verarbeiten (Channel-Frame)
    void handleCrsfPacket();

private:
    TaskHandle_t _taskHandle;
    uint8_t buffer[CRSF_PACKET_LEN];
    int channel[16];
    HardwareSerial crsf;

    uint32_t _lastPacketMs;        // Zeitstempel des letzten gültigen Frames
    bool     _failsafeActive;      // aktueller Failsafe-Status
    
};

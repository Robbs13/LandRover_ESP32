#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Config.h" 
#include <HardwareSerial.h>


class RcCom {
public:
    explicit RcCom(int uartNum = 1);

    // UART Initialisierung
    void begin(); 

    // Startet den FreeRTOS-Task für diese Instanz
    bool startTask(UBaseType_t priority, BaseType_t core, uint8_t time);

private:
    // Die eigentliche Task-Schleife (instanzbezogen)
    void taskLoop();

    // Statischer „Trampolin“-Entry für FreeRTOS
    static void taskTrampoline(void *pvParameters);

    // CRSF-Paket verarbeiten (Channel-Frame)
    void handleCrsfPacket();
    void checkCrsfPacket();
    void failSafe();
    void debug();


private:
    TaskHandle_t    _taskHandle;
    uint8_t         _buffer[CRSF_PACKET_LEN];   // Buffer für die CRSF Data
    int             _rcChannel[CRSF_CHANNEL];     // Input der RC Eingaben aufgeteilt nach Channel
    HardwareSerial  _crsf;                      // UART Verbindung
    uint32_t        _lastPacketMs;              // Zeitstempel des letzten gültigen Frames
    bool            _failsafeActive;            // aktueller Failsafe-Status
    uint8_t         _cycleTime;                 // Zykluszeit von dem Task
    
};

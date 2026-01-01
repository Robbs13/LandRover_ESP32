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

    // Methoden für die Verarbeitung der RC Sender Daten
    bool checkCrsfPacket();
    void decodeCrsfPacket();
    void failSafeActive();
    void publishRcDataQueue();
    void debug(bool newFrame);


private:
    TaskHandle_t    _taskHandle;
    uint8_t         _buffer[CRSF_PACKET_LEN];   // Buffer für die CRSF Data
    int             _rcChannel[CRSF_CHANNEL];   // Input der RC Eingaben aufgeteilt nach Channel
    HardwareSerial  _crsf;                      // UART Verbindung
    uint32_t        _lastPacketMs;              // Zeitstempel des letzten gültigen Frames
    bool            _failsafeActive;            // aktueller Failsafe-Status
    uint8_t         _cycleTime;                 // Zykluszeit von dem Task
    uint32_t        _lastDebugMs;               // Zeitstempel der letzten Debug-Ausgabe
    bool            _newChannelData;            // Geparste RC Daten, bereit für Queue
    uint32_t        _now;                       // Aktuelle Zeit
    uint32_t        _lastFailsafeSend;           // Zeitstempel des letzten gültigen Frames
    
};

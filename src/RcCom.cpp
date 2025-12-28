#include "RcCom.h"
#include "Config.h"   // für qRCCom, RcRawFrame etc.

// ---------------------------------------------------------
//  Klasse RcCom für die Kommunikation mit dem RC Sender
// ---------------------------------------------------------

RcCom::RcCom(int uartNum)
: _taskHandle(NULL),
  crsf(uartNum),
  _lastPacketMs(0),
  _failsafeActive(true)    // beim Start: noch kein Link
{
    // Kanäle auf Mittelstellung setzen
    for (int i = 0; i < 16; ++i) {
        channel[i] = 1024;
    }
}

void RcCom::begin() {
    // Pins ggf. aus Config nehmen: g_ioPins.pinXlrsRx / pinXlrsTx
    crsf.begin(CRSF_BAUDRATE, SERIAL_8N1, 16, 17); 
    Serial.println("CRSF Begin...");
}

// ---------------------------------------------------------
//  Task erstellen
// ---------------------------------------------------------
bool RcCom::startTask(UBaseType_t priority, BaseType_t core) {
    BaseType_t res = xTaskCreatePinnedToCore(
        RcCom::taskTrampoline,  // statische Entry-Funktion
        "RCCom",                // Name (für Debug)
        4096,                   // Stack-Größe (Wörter, nicht Bytes)
        this,                   // this als Parameter übergeben!
        priority,               // Priorität
        &_taskHandle,           // Task-Handle
        core                    // Core 0 oder 1
    );
    return (res == pdPASS);
}

void RcCom::taskTrampoline(void *pvParameters) {
    // pvParameters ist der this-Pointer
    RcCom *self = static_cast<RcCom*>(pvParameters);
    self->taskLoop();   // jetzt sind wir im Instanz-Kontext
}

// ---------------------------------------------------------
//  CRSF-Paket verarbeiten
// ---------------------------------------------------------
void RcCom::handleCrsfPacket()
{
    // Wir gehen davon aus: buffer wurde bereits komplett mit CRSF_PACKET_LEN Bytes gefüllt
    // buffer[0] = Address (0xC8)
    // buffer[1] = Length (typisch 24 für RC-Channel-Frame total payload+type+crc)
    // buffer[2] = Type   (0x16 = RC Channels)

    // Sanity-Check: RC-Channel-Frame?
    if (buffer[2] != 0x16) {
        return; // anderes Paket (z.B. Link-Stats, GPS, etc.)
    }

    // Failsafe: erst mal alle Kanäle auf Mittelstellung setzen
    for (int i = 0; i < 16; i++) {
        channel[i] = 1024;
    }

    uint32_t bitBuffer = 0;
    uint8_t  bitCount  = 0;
    uint8_t  dataIndex = 3;  // ab hier liegen die Kanalbits an

    for (int ch = 0; ch < 16; ch++)
    {
        while (bitCount < 11)
        {
            bitBuffer |= ((uint32_t)buffer[dataIndex++]) << bitCount;
            bitCount += 8;
        }

        channel[ch] = bitBuffer & 0x07FF;   // 11 Bit → 0..2047
        bitBuffer >>= 11;
        bitCount   -= 11;
    }

    // Zeitstempel für Failsafe-Überwachung aktualisieren
    _lastPacketMs = millis();

    // Falls Failsafe bisher aktiv war: Link ist wieder da
    if (_failsafeActive) {
        _failsafeActive = false;
        Serial.println("CRSF OK: Link wiederhergestellt");
    }

    // In Queue schreiben (latest-only)
    if (qRCCom != NULL) {
        RcInputData frame{};
        frame.timestampMs = _lastPacketMs;
        for (int i = 0; i < 16; ++i) {
            frame.channel[i] = channel[i];
        }
        xQueueOverwrite(qRCCom, &frame);
    }

    // Debug-Ausgabe passiert in taskLoop(), nicht hier
}

// ---------------------------------------------------------
//  Task Loop für den Ablauf 
// ---------------------------------------------------------
void RcCom::taskLoop() {
    begin();  // sicherstellen, dass HW initialisiert ist

    const TickType_t period = pdMS_TO_TICKS(5);     // Zykluszeit von 5ms
    TickType_t lastWake = xTaskGetTickCount();

    uint32_t lastDebugMs = 0;   // für gedrosselte Debug-Ausgabe

    for (;;)
    {
        bool newFrame = false;  // wurde in diesem Zyklus ein neues Paket dekodiert?

        // -------------------------------------------------
        // 1) CRSF-Datenverarbeitung
        // -------------------------------------------------
        while (crsf.available() >= CRSF_PACKET_LEN)
        {
            // Sync auf Frame-Start (Adresse 0xC8)
            if (crsf.peek() != 0xC8) {
                // falsches Byte → verwerfen & weitersuchen
                crsf.read();
                continue;
            }

            // Jetzt 24 Bytes einlesen
            size_t n = crsf.readBytes(buffer, CRSF_PACKET_LEN);
            if (n != CRSF_PACKET_LEN) {
                // unvollständiges Paket → Schleife verlassen,
                // beim nächsten Durchlauf geht's weiter
                break;
            }

            // Optional: minimale Längenprüfung (CRSF length-Field grob prüfen)
            // buffer[1] ist "Length" (Type + Payload + CRC)
            if (buffer[1] < 5 || buffer[1] > 32) {
                // Unplausible Länge → Paket ignorieren
                continue;
            }

            // Paket verarbeiten (Kanäle dekodieren + in Queue schreiben)
            handleCrsfPacket();
            newFrame = true;
        }

        // -------------------------------------------------
        // 2) Failsafe-Überwachung
        // -------------------------------------------------
        uint32_t now = millis();

        if (!_failsafeActive) {
            // Wenn wir schon einmal ein gültiges Paket hatten
            if (_lastPacketMs > 0 &&
                (now - _lastPacketMs) > RC_FAILSAFE_TIMEOUT_MS)
            {
                _failsafeActive = true;
                Serial.println("CRSF FAILSAFE: keine RC-Frames mehr");

                // Optional: Failsafe-Neutralwerte in Queue schieben
                if (qRCCom != NULL) {
                    RcInputData fsFrame{};
                    fsFrame.timestampMs = now;
                    for (int i = 0; i < 16; ++i) {
                        fsFrame.channel[i] = 1024; // Mittelstellung / safe
                    }
                    xQueueOverwrite(qRCCom, &fsFrame);
                }
            }
        }

        // -------------------------------------------------
        // 3) Debug-Ausgabe NUR bei neuen Frames & ohne Failsafe
        // -------------------------------------------------
        if (newFrame && !_failsafeActive && (now - lastDebugMs >= 100)) {
            lastDebugMs = now;

            Serial.print("CRSF Channels: ");
            for (int i = 0; i < 16; i++) {
                Serial.print(channel[i]);
                Serial.print(i < 15 ? ", " : "\n");
            }
        }

        // Task läuft genau alle 5 ms
        vTaskDelayUntil(&lastWake, period);
    }
}

#include "RcCom.h"
#include "Config.h"   // für qRCCom, RcRawFrame etc.

// ---------------------------------------------------------
//  Klasse RcCom für die Kommunikation mit dem RC Sender
// ---------------------------------------------------------
RcCom::RcCom(int uartNum)
: _taskHandle(NULL),
  _crsf(uartNum),
  _lastPacketMs(0),
  _failsafeActive(true),    // beim Start: noch kein Link
  _lastDebugMs(0),
  _lastFailsafeSend(0)
{
    // Kanäle auf Mittelstellung setzen
    for (int i = 0; i < CRSF_CHANNEL; ++i) {
        _rcChannel[i] = RC_MIDDLE;
    }
}

// ---------------------------------------------------------
//  UART Initialsierung
// ---------------------------------------------------------
void RcCom::begin() {
    // Pins ggf. aus Config nehmen: g_ioPins.pinXlrsRx / pinXlrsTx
    _crsf.begin(CRSF_BAUDRATE, SERIAL_8N1, 16, 17); 
    Serial.println("RCCOM: CRSF Begin...");
}

// ---------------------------------------------------------
//  Task erstellen
// ---------------------------------------------------------
bool RcCom::startTask(UBaseType_t priority, BaseType_t core, uint8_t time) {
    _cycleTime = time;
    
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
//  Task Loop für den Ablauf 
// ---------------------------------------------------------
void RcCom::taskLoop() {
    begin();  // UART Initialisierung

    const TickType_t period = pdMS_TO_TICKS(_cycleTime);     // Zykluszeit der Taskschleife
    TickType_t lastWake = xTaskGetTickCount();

    // Queue wird mit Failsafe Daten befüllt, bis erste gültige Nachricht empfangen wird
    //_lastFailsafeSend = 0;

    // Task Schleife
    for (;;)
    {   
        // Keine neu-geparsten RC Daten
        _newChannelData = false;

        // UART / Check ob komplettes CRSF Packet zur Verfügung steht
        bool newFrame = checkCrsfPacket(); 

        // UART / CRSF Packete parsen und Channel Werte zuordnen
        if (newFrame) decodeCrsfPacket();
        
        // Aktuelle Zeit speichern
        _now = millis();

        // FailSafe aktiv bei fehlenden Packete innerhalb einer konfigurierten Zeit
        failSafeActive();

        // Channel Daten in die RCInputData Queue ablegen
        publishRcDataQueue();

        // Wenn Debug Schnittstelle aktiviert wurde, werden die einzelne Rohwerte der Kanäle ausgegeben
        #if defined(DEBUG_RCCOM)
            debug(newFrame);
        #endif
        

        // Task läuft genau alle 5 ms
        vTaskDelayUntil(&lastWake, period);
    }
}


// ---------------------------------------------------------
//  UART / Check ob komplettes CRSF Packet zur Verfügung steht
// ---------------------------------------------------------
bool RcCom::checkCrsfPacket()
{
    bool foundFrame = false;

    while (_crsf.available() >= CRSF_PACKET_LEN)
    {
        // Sync auf Frame-Start (Adresse 0xC8)
        if (_crsf.peek() != 0xC8) {
            _crsf.read();
            continue;   // falsches Byte → verwerfen & weitersuchen
        }

        // Startbyte korrekt - 24 Bytes einlesen
        size_t n = _crsf.readBytes(_buffer, CRSF_PACKET_LEN);
        if (n != CRSF_PACKET_LEN) {
            break;  // unvollständiges Paket → Schleife verlassen,
        }

        // Optional: minimale Längenprüfung (CRSF length-Field grob prüfen)
        // buffer[1] ist "Length" (Type + Payload + CRC)
        if (_buffer[1] < 5 || _buffer[1] > 32) {
            // Unplausible Länge → Paket ignorieren
            continue;
        }


        foundFrame = true;
    }
    return foundFrame;
}  

// ---------------------------------------------------------
//  UART / CRSF Packete parsen und Channel Werte zuordnen
// ---------------------------------------------------------
void RcCom::decodeCrsfPacket()
{
    // Sanity-Check: RC-Channel-Frame?
    if (_buffer[2] != 0x16) {
        //Serial.println("RCCOM: CRSF falsches Packet. Ungleich 0x16...");
        return; // anderes Paket (z.B. Link-Stats, GPS, etc.)
    }

    // Failsafe: erst mal alle Kanäle auf Mittelstellung setzen
    for (int i = 0; i < CRSF_CHANNEL; i++) {
        _rcChannel[i] = RC_MIDDLE;
    }

    uint32_t bitBuffer = 0;
    uint8_t  bitCount  = 0;
    uint8_t  dataIndex = 3;  // ab hier liegen die Kanalbits an

    // Schleife für jeweils 11 Bits pro Kanal den einzelnen Kanälen zuordnen
    for (int ch = 0; ch < CRSF_CHANNEL; ch++)
    {
        while (bitCount < 11)
        {
            bitBuffer |= ((uint32_t)_buffer[dataIndex++]) << bitCount;
            bitCount += 8;
        }

        _rcChannel[ch] = bitBuffer & 0x07FF;   // 11 Bit → 0..2047
        bitBuffer >>= 11;
        bitCount   -= 11;
    }

    // Zeitstempel für Failsafe-Überwachung aktualisieren
    _lastPacketMs = millis();

    // Falls Failsafe bisher aktiv war: Link ist wieder da
    if (_failsafeActive) {
        _failsafeActive = false;
        Serial.println("RCCOM: CRSF OK - Link wiederhergestellt");
    }

    _newChannelData = true;
}

// ---------------------------------------------------------
//  FailSafe aktiv bei fehlenden Packete innerhalb einer konfigurierten Zeit
// ---------------------------------------------------------
void RcCom::failSafeActive()
{
    // Bereits im Failsafe -> nichts zu tun
    if (_failsafeActive) return;

    // Keine neuen Packete, aber noch innerhalb Timeout -> nichts tun
    if (_now - _lastPacketMs <= RC_FAILSAFE_TIMEOUT_MS) return;

    // Ab hier: Failsafe aktivieren
    _failsafeActive = true;
    Serial.println("RCCOM: CRSF FAILSAFE - Keine RC-Frames mehr");
    
}

// ---------------------------------------------------------
//  Debug Schnittstelle. Gibt alle RC Sender Kanäle als Rohwerte aus
// ---------------------------------------------------------
void RcCom::publishRcDataQueue()
{
    if (qRCCom != NULL) {
        // Neu geparste Channel Daten werden an die Queue übergeben
        if(_newChannelData){            
            RcInputData frame{};
            frame.timestampMs = _lastPacketMs;
            for (int i = 0; i < CRSF_CHANNEL; ++i) {
                frame.channel[i] = _rcChannel[i];
            }
            frame.FailSafeRC = _failsafeActive;
            xQueueOverwrite(qRCCom, &frame);

            _lastFailsafeSend = 0;
            //Serial.println("RCCOM: CRSF Data - New Data in Queue");               
        }

        if(_failsafeActive){
            // Sobald Failsafe, Nachricht schicken - Dannach alle RC_FAILSAFE_CYCLE in ms
            if((_now - _lastFailsafeSend  >= RC_FAILSAFE_CYCLE) || _lastFailsafeSend == 0){
            
                RcInputData fsFrame{};
                fsFrame.timestampMs = _now;
                for (int i = 0; i < CRSF_CHANNEL; ++i) {
                    fsFrame.channel[i] = 992; // Mittelstellung / safe
                }
                fsFrame.FailSafeRC = _failsafeActive;
                xQueueOverwrite(qRCCom, &fsFrame);

                _lastFailsafeSend = _now;
                //Serial.println("RCCOM: CRSF FAILSAFE - Nachricht in Queue");
                
            }
        }    
    }
}

// ---------------------------------------------------------
//  Debug Schnittstelle. Gibt alle RC Sender Kanäle als Rohwerte aus
// ---------------------------------------------------------
void RcCom::debug(bool newFrame)
{
    if (!newFrame)        return;          // keine neuen Daten
    if (_failsafeActive)  return;          // im Failsafe → nicht spammen
    if (_now - _lastDebugMs < 100) return;  // nur alle 100ms

    _lastDebugMs = _now;

    Serial.print("RCCOM: CRSF Channels: ");
    for (int i = 0; i < CRSF_CHANNEL; i++) {
        Serial.print(_rcChannel[i]);
        Serial.print(i < (CRSF_CHANNEL - 1) ? ", " : "\n");
    }
}

// ---------------------------------------------------------
//  Hilfsrechnung: Übersetzt die CRSF Raw Daten in PWM übliche 1000 - 2000
// ---------------------------------------------------------
static inline uint16_t crsfToRc(uint16_t v)
{
    if (v < 172)  v = 172;
    if (v > 1811) v = 1811;

    return 1000 + (uint32_t)(v - 172) * 1000 / (1811 - 172);
}


// ---------------------------------------------------------
//  CRSF-Paket verarbeiten
//
// Wir gehen davon aus: buffer wurde bereits komplett mit CRSF_PACKET_LEN Bytes gefüllt
// buffer[0] = Address (0xC8)
// buffer[1] = Length (typisch 24 für RC-Channel-Frame total payload+type+crc)
// buffer[2] = Type   (0x16 = RC Channels)
//
//  [ Address | Length | Type | Payload ... | CRC ]
//
// Byte	    Name	                Wert / Bedeutung
// 0	    0xC8	                Device Address → Empfänger → Controller
// 1	    0x18 (24)	            Länge (inkl. CRC)
// 2	    0x16	                Frame Type: "RC Channels Packed"
// 3–24	    22 Bytes Kanal-Payload	16 Kanäle × 11 Bit (gepackt)
// 25	    CRC	                    Prüfsumme (XOR-basiert, 8 Bit)
//
// Bereich	        Beschreibung
// 22 Bytes         Payload	enthält 16 Kanäle à 11 Bit
// Bitordnung	    Little-Endian (niedrigste Bits zuerst)
// Reihenfolge	    CH1, CH2, CH3, … CH16
// Wertebereich	    0–2047 (11 Bit)
// Mapping (µs)	    172–1811 → 1000–2000 µs
// CRC	            1 Byte am Ende (8 Bit, Polynomial 0xD5)
//
// ---------------------------------------------------------
/**
 * @file    CRSF_serial.cpp
 * @brief   CRSF UART-Kommunikation und RC-Channel-Parser (ESP32/Arduino)
 *
 * @details Implementiert:
 *          - UART Initialisierung für CRSF (420k Baud)
 *          - FreeRTOS Task zum zyklischen Einlesen/Parsen
 *          - Paket-Parser (Length/CRC) für CRSF Frames
 *          - Entpacken der RC-Channel-Daten (11-bit packed) in Rohwert + µs
 */

#include "CRSF_serial.h"
#include "Config.h"   // für qRCCom, RcRawFrame etc.

/**
 * @brief Konstruktor der CRSF_serial-Klasse.
 *
 * @details Initialisiert den Task-Handle und erstellt das HardwareSerial-Objekt
 *          für den gewünschten UART-Port.
 *
 * @param uartNum UART-Nummer (plattformabhängig, z. B. Serial1/Serial2 beim ESP32)
 */
CRSF_serial::CRSF_serial(int uartNum)
: _taskHandle(NULL),
  _port(uartNum)
{
    // Optional: Kanäle initial auf Mittelstellung setzen (falls benötigt)
    // for (int i = 0; i < CRSF_CHANNEL; ++i) {
    //     _rcChannel[i] = RC_MIDDLE;
    // }
}

/**
 * @brief Initialisiert die UART-Schnittstelle für CRSF.
 *
 * @details Startet den UART-Port mit CRSF_BAUDRATE und setzt Datenformat/Pinbelegung.
 *          Die Pins (hier RX=16, TX=17) können bei Bedarf aus Config kommen.
 */
void CRSF_serial::begin() {
    // Pins ggf. aus Config nehmen: g_ioPins.pinXlrsRx / pinXlrsTx
    _port.begin(CRSF_BAUDRATE, SERIAL_8N1, 16, 17);
    Serial.println("RCCOM: CRSF Begin...");
}

/**
 * @brief Erstellt und startet eine FreeRTOS Task zur CRSF-Verarbeitung.
 *
 * @details Die Task ruft intern taskLoop() auf und läuft gepinnt auf einem Core.
 *          Die Zykluszeit (time) wird als _cycleTime gespeichert und bestimmt,
 *          wie häufig die Task-Schleife läuft (vTaskDelayUntil).
 *
 * @param priority Task-Priorität (FreeRTOS).
 * @param core     CPU-Core auf dem die Task laufen soll (ESP32 typischerweise 0 oder 1).
 * @param time     Zykluszeit in Millisekunden (wird in pdMS_TO_TICKS umgerechnet).
 *
 * @return true wenn die Task erfolgreich erstellt wurde (pdPASS), sonst false.
 */
bool CRSF_serial::startTask(UBaseType_t priority, BaseType_t core, uint8_t time) {
    _cycleTime = time;

    BaseType_t res = xTaskCreatePinnedToCore(
        CRSF_serial::taskTrampoline,  // statische Entry-Funktion
        "CRSF_serial",                // Name (für Debug)
        4096,                         // Stack-Größe (Wörter, nicht Bytes)
        this,                         // this als Parameter übergeben
        priority,                     // Priorität
        &_taskHandle,                 // Task-Handle
        core                          // Core 0 oder 1
    );
    return (res == pdPASS);
}

/**
 * @brief Statischer Task-Wrapper (Trampolin) für FreeRTOS.
 *
 * @details FreeRTOS erwartet eine C-Funktion als Entry-Point. Diese Funktion
 *          castet pvParameters zurück auf das Objekt und ruft dann taskLoop()
 *          im Instanz-Kontext auf.
 *
 * @param pvParameters Zeiger auf die Instanz (this), der beim Task-Start übergeben wurde.
 */
void CRSF_serial::taskTrampoline(void *pvParameters) {
    CRSF_serial *self = static_cast<CRSF_serial*>(pvParameters);
    self->taskLoop();
}

/**
 * @brief Hauptschleife der CRSF-Task.
 *
 * @details Initialisiert UART, verarbeitet zyklisch eingehende CRSF-Daten und
 *          gibt optional Debug-Ausgaben aus:
 *          - Beim ersten LinkUp: alle Channels ausgeben
 *          - Danach: nur geänderte Channels (über DELTA_US) alle ~100ms
 *
 *          Die Task wird über vTaskDelayUntil auf eine feste Periode synchronisiert.
 */
void CRSF_serial::taskLoop() {
    begin();  // UART Initialisierung

    const TickType_t period = pdMS_TO_TICKS(_cycleTime);  ///< Zykluszeit der Taskschleife
    TickType_t lastWake = xTaskGetTickCount();

    Serial.println("\n========================================");
    Serial.println("CRSF Serial - RC Kanal Datenlese-Demo");
    Serial.println("========================================\n");

    Serial.println("CRSF Serial gestartet...");
    Serial.println("Warte auf Kanaldaten (0x16)...\n");

    // Task Schleife
    for (;;)
    {
        //Start der Bytestream Bearbeitung
        handleSerialIn();

        // Ausgabe der RAW Kanal Werte
        #if defined(DEBUG_CRSF_IN)
            debugIn();
        #endif

        // Task läuft zyklisch (z. B. alle 5ms bei _cycleTime=5)
        vTaskDelayUntil(&lastWake, period);
    }
}

/**
 * @brief Liest alle verfügbaren Bytes vom UART und füttert den CRSF-Paketparser.
 *
 * @details
 * - Liest Bytes per _port.read()
 * - Wenn Passthrough aktiv ist, werden Bytes als OOB weitergereicht
 * - Sonst werden Bytes in _rxBuf gespeichert und handleByteReceived() getriggert
 * - Danach werden Timeout und LinkDown geprüft
 */
void CRSF_serial::handleSerialIn()
{
    while (_port.available())
    {
        uint8_t b = _port.read();
        _lastReceive = millis();

        if (getPassthroughMode())
        {
            if (onOobData)
                onOobData(b);
            continue;
        }

        _rxBuf[_rxBufPos++] = b;

        // 
        handleByteReceived();

        if (_rxBufPos == (sizeof(_rxBuf)/sizeof(_rxBuf[0])))
        {
            // RX-Puffer ist voll, ohne dass ein gültiges Paket gefunden wurde -> verwerfen
            _rxBufPos = 0;
        }
    }

    checkPacketTimeout();
    checkLinkDown();
}


/**
 * @brief Prüft den RX-Puffer auf vollständige CRSF-Pakete und verarbeitet sie.
 *
 * @details
 * - Erwartet: Byte[1] enthält die CRSF-Länge (Type+Payload+CRC)
 * - Sanity-Check der Länge
 * - Wenn genug Bytes vorhanden: CRC prüfen
 * - Bei gültigem CRC: processPacketIn() aufrufen und Paket aus Puffer entfernen
 * - Bei Fehler: byteweise schieben (Resync)
 */
void CRSF_serial::handleByteReceived()
{
    bool reprocess;
    do
    {
        reprocess = false;
        if (_rxBufPos > 1)
        {
            uint8_t len = _rxBuf[1];

            // Länge plausibilisieren: min 3 (type+1 payload + crc), max payload+2 (type+crc)
            if (len < 3 || len > (CRSF_MAX_PAYLOAD_LEN + 2))
            {
                shiftRxBuffer(1);
                reprocess = true;
            }
            else if (_rxBufPos >= (len + 2))
            {
                // Komplettes Paket vorhanden: [sync][len] + len Bytes folgen
                uint8_t inCrc = _rxBuf[2 + len - 1];
                uint8_t crc = _crc.calc(&_rxBuf[2], len - 1);

                if (crc == inCrc)
                {
                    // Komplettes Packet wird zur weiteren Verabeitung übergeben
                    processPacketIn(len);
                    shiftRxBuffer(len + 2);
                    reprocess = true;
                }
                else
                {
                    // CRC falsch -> ein Byte verwerfen und neu versuchen
                    shiftRxBuffer(1);
                    reprocess = true;
                }
            }
        }
    } while (reprocess);
}

/**
 * @brief Dispatch-Funktion: verarbeitet ein gültiges CRSF-Paket anhand des Frame-Typs.
 *
 * @param len Länge aus dem CRSF-Length-Byte (Type+Payload+CRC), aktuell nur als Info.
 */
void CRSF_serial::processPacketIn(uint8_t len)
{
    (void)len; ///< aktuell ungenutzt (verhindert Warnung)

    const crsf_header_t *hdr = (crsf_header_t *)_rxBuf;
    switch (hdr->type)
    {
    case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
        //Frame Typ RC Kanäle wird zur weiteren Bearbeitung übergeben
        packetChannelsPacked(hdr);
        break;
    default:
        // Andere Frame-Typen werden hier derzeit nicht verarbeitet
        break;
    }
}

/**
 * @brief Entpackt CRSF RC-Channel-Daten (11 Bit pro Kanal) aus einem packed Frame.
 *
 * @details
 * CRSF sendet die 16 RC-Channels bitgepackt mit 11 Bit pro Channel.
 * Diese Funktion:
 * - extrahiert für jeden Channel den 11-bit Rohwert
 * - speichert den Rohwert in _rawChannels[]
 * - rechnet in µs um und speichert in _channels[]
 * - aktualisiert Link-Status und Zeitstempel (_lastChannelsPacket)
 *
 * @param p Zeiger auf CRSF-Header des empfangenen Frames (p->data enthält Payload).
 */
void CRSF_serial::packetChannelsPacked(const crsf_header_t *p)
{
    if (p->frame_size != CRSF_MAX_PAYLOAD_CHANNEL_LEN) {   // 22 payload + type + crc
        Serial.println("packetChannelsPacked falsch Länge");
        return;                  // nicht plausibel -> ignorieren
    }

    constexpr unsigned inputMask = (1 << CRSF_BITS_PER_CHANNEL) - 1; ///< Maske für 11 Bit
    const uint8_t *buf = p->data;                                    ///< Pointer auf Payload-Bytes

    unsigned scratch = 0;        ///< Bit-Zwischenspeicher für das Entpacken
    unsigned bitsInScratch = 0;  ///< Anzahl gültiger Bits im scratch
    
    RcFrameData frame{};
    frame.state = RcLinkState::UpLink;

    for (unsigned ch = 0; ch < CRSF_NUM_CHANNELS; ++ch)
    {
        // Scratch solange mit Bytes auffüllen, bis mindestens 11 Bits vorhanden sind
        while (bitsInScratch < CRSF_BITS_PER_CHANNEL)
        {
            scratch |= (*buf++) << bitsInScratch;
            bitsInScratch += 8;
        }

        // Rohwert extrahieren und umrechnen
        unsigned raw = scratch & inputMask;
        _rawChannels[ch] = uint16_t(raw);

        // Umrechnung in RC Format 1000-2000
        UsResult usResult = CRSF_to_US((int)ch, (int)raw);
        frame.ch_us[ch] = usResult.us;

        //Kompletter Frame wird als ungültig markiert, Values außerhalb Wertebereich
        if (!usResult.valid){
            frame.state = RcLinkState::WrongValues;
        }

        // Nächsten Channel vorbereiten
        scratch >>= CRSF_BITS_PER_CHANNEL;
        bitsInScratch -= CRSF_BITS_PER_CHANNEL;
    }

    // Link-Status aktualisieren
    if (!_linkIsUp) {
        Serial.println("*** CRSF-Verbindung aufgebaut ***");
    }
    _linkIsUp = true;
    _lastChannelsPacket = millis();
    frame.timestampMs = _lastChannelsPacket;
 
    publishRcChannelToQueue(frame);
}

/**
 * @brief Übergibt einen RCFrame der Queue
 *
 * @details RC Frame mit Status WrongValues werden blockiert
 *         
 */
void CRSF_serial::publishRcChannelToQueue(const RcFrameData& frame)
{
  
    if (!q_CRSF) return;

    if (frame.state != RcLinkState::WrongValues) {
        xQueueOverwrite(q_CRSF, &frame);

        // Ausgabe der RC Frame Werte
        #if defined(DEBUG_CRSF_OUT)
            debugOut(frame);
        #endif

        if (frame.state == RcLinkState::DownLink){
            Serial.print("*** LinkState: ");
            Serial.println("DownLink");
        }
    
    }
    // *** Debug ***
    else{
        if (frame.state == RcLinkState::WrongValues){
            Serial.print("*** LinkState: ");
            Serial.println("WrongValues");
        }
    }
}

/**
 * @brief Prüft, ob der RX-Puffer wegen Inaktivität geleert werden soll.
 *
 * @details Wenn länger als CRSF_PACKET_TIMEOUT_MS keine Daten empfangen wurden,
 *          wird der Puffer byteweise geleert (shiftRxBuffer(1)), um ggf. OOB-Bytes
 *          weiterzugeben und den Parser zu resynchronisieren.
 */
void CRSF_serial::checkPacketTimeout()
{
    if (_rxBufPos > 0 && millis() - _lastReceive > CRSF_PACKET_TIMEOUT_MS)
        while (_rxBufPos)
            shiftRxBuffer(1);
}

/**
 * @brief Prüft, ob die CRSF-Verbindung als "down" markiert werden muss und geht in Failsafe
 *
 * @details Wenn länger als CRSF_FAILSAFE_STAGE1_MS kein Channel-Paket empfangen wurde,
 *          wird der Link als unterbrochen betrachtet und _linkIsUp auf false gesetzt.
 *          RC Frame mit Failsafe Daten und Status DownLink wird an die Queue übergeben
 */
void CRSF_serial::checkLinkDown()
{
    if (_linkIsUp && millis() - _lastChannelsPacket > CRSF_FAILSAFE_STAGE1_MS)
    {
        Serial.println("*** CRSF-Verbindung unterbrochen ***");
        // ToDo: FAILSAFE aufrufen 
        _linkIsUp = false;
        RcFrameData frame{};
        frame.timestampMs = millis();
        for (int i = 0; i < CRSF_NUM_CHANNELS; ++i) {
            
            frame.ch_us[i] = (uint16_t)1500;
        }
        frame.state = RcLinkState::DownLink;

        // Frame Übergabe an Queue
        publishRcChannelToQueue(frame);
    }
}

/**
 * @brief Entfernt cnt Bytes vom Anfang des RX-Puffers und schiebt den Rest nach vorne.
 *
 * @details
 * - cnt >= _rxBufPos: Puffer leeren
 * - cnt == 1: optional das entfernte Byte als OOB melden (onOobData)
 * - ansonsten: Speicherbereich manuell verschieben
 *
 * @param cnt Anzahl der zu entfernenden Bytes.
 */
void CRSF_serial::shiftRxBuffer(uint8_t cnt)
{
    if (cnt >= _rxBufPos)
    {
        _rxBufPos = 0;
        return;
    }

    if (cnt == 1 && onOobData)
        onOobData(_rxBuf[0]);

    uint8_t *src = &_rxBuf[cnt];
    uint8_t *dst = &_rxBuf[0];
    _rxBufPos -= cnt;

    uint8_t left = _rxBufPos;
    while (left--)
        *dst++ = *src++;
}

/**
 * @brief Wenn DEBUG_CRSF_IN definiert wurde, werden Live Kanal Werte aus dem Input im Terminal ausgegeben
 *
 * @details
 * - Werte wurden umgerechnet
 */
void CRSF_serial::debugIn()
{
    // Debug-Ausgabe: alle 100ms nur Änderungen (oder initial alle Kanäle)
    static unsigned long lastChannelOutput = 0;
    if (millis() - lastChannelOutput > 100) {
        lastChannelOutput = millis();

        if (_linkIsUp) {
            const int DELTA_US = 10;                 ///< Schwellwert zum Erkennen von Änderungen (µs)
            static int prev[CRSF_NUM_CHANNELS];      ///< Letzte ausgegebene Kanalwerte (µs)
            static bool initialized = false;         ///< true, sobald prev[] initialisiert wurde

            // Beim ersten Durchlauf initialisieren und alles ausgeben
            if (!initialized) {
                for (int i = 0; i < CRSF_NUM_CHANNELS; ++i)
                    //prev[i] = _channels[i];
                    prev[i] = _rawChannels[i];

                initialized = true;

                Serial.print("["); Serial.print(millis()); Serial.print("ms]");
                Serial.print(" CH(all): ");
                for (int ch = 1; ch <= CRSF_NUM_CHANNELS; ch++) {
                    Serial.print(ch); Serial.print(":"); Serial.print(prev[ch-1]);
                    if (ch < CRSF_NUM_CHANNELS) Serial.print(" | ");
                }
                Serial.println();
            } else {
                // Sammle nur veränderte Kanäle
                bool any = false;
                String out;

                for (int ch = 0; ch < CRSF_NUM_CHANNELS; ++ch) {
                    int cur = _rawChannels[ch];
                    //int cur = _rawChannels[ch];
                    if (abs(cur - prev[ch]) >= DELTA_US) {
                        if (any) out += " | ";

                        out += String(ch + 1);
                        out += ":";
                        out += " (";
                        out += String(cur);
                        out += ")";

                        prev[ch] = cur;
                        any = true;
                    }
                }

                if (any) {
                    Serial.print("["); Serial.print(millis()); Serial.print("ms] CH(changed): ");
                    Serial.println(out);
                }
            }

            
            
        }
    }
    
}

 /**
 * @brief Wenn DEBUG_CRSF_Out definiert wurde, werden bearbeitete Kanal Werte aus der Queue im Terminal ausgegeben
 *
 * @details
 * - Werte wurden umgerechnet
 */
void CRSF_serial::debugOut(const RcFrameData& frame)
{
    // Debug-Ausgabe: alle 100ms nur Änderungen (oder initial alle Kanäle)
    static unsigned long lastChannelOutput = 0;
    if (millis() - lastChannelOutput > 100) {
        lastChannelOutput = millis();

        if (q_CRSF) {
            const int DELTA_US = 10;                 ///< Schwellwert zum Erkennen von Änderungen (µs)
            static int prev[CRSF_NUM_CHANNELS];      ///< Letzte ausgegebene Kanalwerte (µs)
            static bool initialized = false;         ///< true, sobald prev[] initialisiert wurde

            // Beim ersten Durchlauf initialisieren und alles ausgeben
            if (!initialized) {
                for (int i = 0; i < CRSF_NUM_CHANNELS; ++i)
                    //prev[i] = _channels[i];
                    prev[i] = frame.ch_us[i];

                initialized = true;

                Serial.print("["); Serial.print(millis()); Serial.print("ms]");
                Serial.print(" Queue RC Message: ");

                switch (frame.state)
                {
                case RcLinkState::UpLink:
                    //Frame Typ RC Kanäle wird zur weiteren Bearbeitung übergeben
                    Serial.print(" +UpLink+ ");
                    break;
                case RcLinkState::DownLink:
                    //Frame Typ RC Kanäle wird zur weiteren Bearbeitung übergeben
                    Serial.print(" +DownLink+ ");
                    break;
                case RcLinkState::WrongValues:
                //Frame Typ RC Kanäle wird zur weiteren Bearbeitung übergeben
                Serial.print(" +WrongValues+ ");
                break;
                default:
                    // Andere Frame-Typen werden hier derzeit nicht verarbeitet
                    break;
                }

                //Serial.print(frame.state);
                for (int ch = 1; ch <= CRSF_NUM_CHANNELS; ch++) {
                    Serial.print(ch); Serial.print(":"); Serial.print(prev[ch-1]);
                    if (ch < CRSF_NUM_CHANNELS) Serial.print(" | ");
                }
                Serial.println();
            } else {
                // Sammle nur veränderte Kanäle
                bool change = false;
                String out;

                for (int ch = 0; ch < CRSF_NUM_CHANNELS; ++ch) {
                    int cur = frame.ch_us[ch];

                    if (abs(cur - prev[ch]) >= DELTA_US) { 
                        change = true;
                    }
                }
                // Ausgabe wenn sich ein Input im Kanal geändert hat
                if (change == true){
                    for (int i = 0; i < CRSF_NUM_CHANNELS; ++i)
                    //prev[i] = _channels[i];
                        prev[i] = frame.ch_us[i];

                    Serial.print("["); Serial.print(millis()); Serial.print("ms]");
                    Serial.print(" Queue RC Message: ");

                    switch (frame.state)
                    {
                    case RcLinkState::UpLink:
                        Serial.print(" +UpLink+ ");
                        break;
                    case RcLinkState::DownLink:
                        Serial.print(" +DownLink+ ");
                        break;
                    case RcLinkState::WrongValues:
                    Serial.print(" +WrongValues+ ");
                    break;
                    default:
                        break;
                    }
                    for (int ch = 1; ch <= CRSF_NUM_CHANNELS; ch++) {
                        Serial.print(ch); Serial.print(":"); Serial.print(prev[ch-1]);
                        if (ch < CRSF_NUM_CHANNELS) Serial.print(" | ");
                    }
                    Serial.println();

                }
            }
        }
    }  
}

/**
 * @brief CRSF Wertebereich 172-992-1810 Umrechnen in RC Standard 1000-1500-2000us
 *
 * @details
 * - Werte werden umgerechnet und dann auf 1000-2000 gekappt
 */
UsResult CRSF_serial::CRSF_to_US(int ch, int crsf)
{
    UsResult usResult;
    usResult.valid = true;
    int us;
    // Raw Werte als invalid taggen und hart begrenzen
    if (crsf < CRSF_VALUE_MIN) {
        Serial.print("*** Raw Wert kleiner als 172: ");
        Serial.print(crsf);
        Serial.print("  Channel: ");
        Serial.println(ch+1);
        //crsf = CRSF_VALUE_MIN;
        us = 0;
        usResult.valid = false;
        return usResult;
    }
    if (crsf > CRSF_VALUE_MAX) {
        Serial.print("*** Raw Wert groesser als 1810: ");
        Serial.print(crsf);
        Serial.print("  Channel: ");
        Serial.println(ch+1);
        // crsf = CRSF_VALUE_MAX;
        us = 0;
        usResult.valid = false;
        return usResult;
    }

    // Umrechnung von auf RC Wertebereich
    if (crsf <= CRSF_VALUE_MID) {
        us = 1500 - (int)(((uint32_t)(CRSF_VALUE_MID - crsf) * 500u + 410u) / 820u);
    } else {
        us = 1500 + (int)(((uint32_t)(crsf - CRSF_VALUE_MID) * 500u + 409u) / 818u);
    }

    // Ausgang hart auf 1000–2000 clampen, falls Umrechnungsformel nicht passt
    if (us < 1000) {
        Serial.print("*** US wurde geclampt auf 1000: ");
        Serial.println(us);
        us = 1000;
    }
    if (us > 2000) {
        Serial.print("*** US wurde geclampt auf 2000: ");
        Serial.println(us);
        us = 2000;
    }

    usResult.us = (uint16_t)us;

    return usResult;
}



  

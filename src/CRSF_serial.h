#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Config.h" 
#include <HardwareSerial.h>
#include "crc8.h"

// CRSF Protokol Eigenschaften
#define CRSF_BAUDRATE 420000
#define CRSF_MAX_PACKET_SIZE 64 // max declared len is 62+DEST+LEN on top of that = 64
#define CRSF_MAX_PAYLOAD_LEN (CRSF_MAX_PACKET_SIZE - 4) // Max size of payload in [dest] [len] [type] [payload] [crc8]
#define CRSF_MAX_PAYLOAD_CHANNEL_LEN 24
#define CRSF_BITS_PER_CHANNEL   11
#define CRSF_VALUE_MIN  172
#define CRSF_VALUE_MID  992
#define CRSF_VALUE_MAX  1810
#define CRSF_FAILSAFE 1500
#define CRSF_PACKET_TIMEOUT_MS 100
#define CRSF_FAILSAFE_STAGE1_MS 300

// PACKED: verhindert Compiler-Padding in Strukturen,
// damit das Speicherlayout exakt dem CRSF-Protokoll entspricht
#if !defined(PACKED)
#define PACKED __attribute__((packed))
#endif

/**
 * @brief CRSF Frame Header
 *
 * @details
 * Jedes CRSF-Paket beginnt mit diesem Header.
 * Das Payload beginnt direkt bei data[0] und hat
 * eine Länge von (frame_size - 2) Bytes.
 *
 * PACKED verhindert Padding, damit das Layout
 * exakt dem CRSF-Protokoll entspricht.
 */
typedef struct crsf_header_s
{
    uint8_t sync_byte;   // CRSF_SYNC_BYTE
    uint8_t frame_size;  // counts size after this byte, so it must be the payload size + 2 (type and crc)
    uint8_t type;        // from crsf_frame_type_e
    uint8_t data[0];
} PACKED crsf_header_t;


// CRSF Packet Typen
typedef enum
{
    // CRSF_FRAMETYPE_GPS = 0x02,
    // CRSF_FRAMETYPE_BATTERY_SENSOR = 0x08,
    // CRSF_FRAMETYPE_AIRSPEED = 0x0A,
    // CRSF_FRAMETYPE_RPM = 0x0C,
    // CRSF_FRAMETYPE_TEMP = 0x0D,
    // CRSF_FRAMETYPE_CELLS = 0x0E,
    // CRSF_FRAMETYPE_OPENTX_SYNC = 0x10,
    // CRSF_FRAMETYPE_LINK_STATISTICS = 0x14,
    CRSF_FRAMETYPE_RC_CHANNELS_PACKED = 0x16,           // Type für RC Kanäle
    // CRSF_FRAMETYPE_ATTITUDE = 0x1E,
    // CRSF_FRAMETYPE_FLIGHT_MODE = 0x21,
    // // Extended Header Frames, range: 0x28 to 0x96
    // CRSF_FRAMETYPE_DEVICE_PING = 0x28,
    // CRSF_FRAMETYPE_DEVICE_INFO = 0x29,
    // CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY = 0x2B,
    // CRSF_FRAMETYPE_PARAMETER_READ = 0x2C,
    // CRSF_FRAMETYPE_PARAMETER_WRITE = 0x2D,
    // CRSF_FRAMETYPE_COMMAND = 0x32,
    // CRSF_FRAMETYPE_RADIO_ID = 0x3A,
    // // MSP commands
    // CRSF_FRAMETYPE_MSP_REQ = 0x7A,   // response request using msp sequence as command
    // CRSF_FRAMETYPE_MSP_RESP = 0x7B,  // reply with 58 byte chunked binary
    // CRSF_FRAMETYPE_MSP_WRITE = 0x7C, // write with 8 byte chunked binary (OpenTX outbound telemetry buffer limit)
} crsf_frame_type_e;

// Struct für Berechnung us in RC Werte und Bewertung ob outOfRange
struct UsResult {
    uint16_t us;
    bool valid;
};





class CRSF_serial {
public:
    explicit CRSF_serial(int uartNum = 1);

    // UART Initialisierung
    void begin(); 
    // Startet den FreeRTOS-Task für diese Instanz
    bool startTask(UBaseType_t priority, BaseType_t core, uint8_t time);


private:
    // Die eigentliche Task-Schleife (instanzbezogen)
    void taskLoop();

    // Statischer „Trampolin“-Entry für FreeRTOS
    static void taskTrampoline(void *pvParameters);

    // Funktionen zum Bearbeiten des Bytestreams
    void handleSerialIn();
    void handleByteReceived();
    void processPacketIn(uint8_t len);
    void packetChannelsPacked(const crsf_header_t *p);
    void publishRcChannelToQueue(const RcFrameData& frame);
    
    // Hilfsfunktionen
    void checkPacketTimeout();
    void checkLinkDown();
    void shiftRxBuffer(uint8_t cnt);
    bool getPassthroughMode() const { return _passthroughBaud != 0; }
    UsResult CRSF_to_US(int channel, int crsf);
    void (*onOobData)(uint8_t b);   // OobData is any byte which is not CRSF, including passthrough
    void debugIn();
    void debugOut(const RcFrameData& frame);
    
    // void packetLinkStatistics(const crsf_header_t *p);
    // void packetGps(const crsf_header_t *p);


    // Task Variablen
    TaskHandle_t    _taskHandle;
    uint8_t         _cycleTime;                 // Zykluszeit von dem Task
    HardwareSerial  _port;

    // Bearbeitung Bytestream
    Crc8            _crc;  
    uint32_t        _lastReceive;
    uint32_t        _lastChannelsPacket; 
    uint32_t        _passthroughBaud;
    uint8_t         _rxBuf[CRSF_MAX_PACKET_SIZE];
    uint8_t         _rxBufPos;
    uint16_t        _channels[CRSF_NUM_CHANNELS];
    uint16_t        _rawChannels[CRSF_NUM_CHANNELS];
    bool            _linkIsUp;

};

#ifndef CRC8_H
#define CRC8_H

#include <stdint.h>
#include <stddef.h>

// ---------------------------------------------------------
//  Nicht klar, ob das hier funktioniert!!!!!...!!!!
// ---------------------------------------------------------


// CRC8 Klasse für CRSF Protokoll
class Crc8
{
public:
    Crc8(uint8_t poly = 0xD5) : _poly(poly), _crc(0) {}
    
    /**
     * @brief Berechnet CRC8 Checksumme für Daten
     * @param data Pointer auf die zu checksummierende Daten
     * @param len Länge der Daten in Bytes
     * @return CRC8 Checksumme
     */
    uint8_t calc(const uint8_t* data, size_t len) {
        uint8_t crc = 0;
        for (size_t i = 0; i < len; i++) {
            crc = update(crc, data[i]);
        }
        return crc;
    }
    
    /**
     * @brief Inkrementelle CRC8 Berechnung
     * @param byte Nächstes Datenbyte
     * @return Aktualisierte CRC8 Checksumme
     */
    uint8_t update(uint8_t byte) {
        return _crc = update(_crc, byte);
    }

private:
    uint8_t _poly;
    uint8_t _crc;
    
    uint8_t update(uint8_t crc, uint8_t byte) {
        crc ^= byte;
        for (int i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ _poly;
            } else {
                crc = crc << 1;
            }
        }
        return crc;
    }
};

#endif // CRC8_H

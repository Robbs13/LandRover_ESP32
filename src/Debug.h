#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

    // Klasse für Debuglevel
    enum class DebugLevel : uint8_t {
        None = 0,
        Error = 1,
        Info = 2,
        Debug = 3
    };

    class Debug {
        private:
            // Konstruktor privat -> niemand kann von außen ein Objekt erzeugen
            Debug();

            // Kopieren und Zuweisung verbieten
            Debug(const Debug&) = delete;
            Debug& operator=(const Debug&) = delete;

            //Variablen
            uint8_t debugController = 0;
            uint8_t debugLight = 0;
            uint8_t debugSound = 0;
            uint8_t debugDrive = 0;
            uint8_t debugFunction = 0;

        public:
            //Debug();

            // Zugriff auf die einzige Instanz
            static Debug& getInstance();

            void setController(DebugLevel level);
            void setLight(DebugLevel level);        
            void setSound(DebugLevel level);        
            void setDrive(DebugLevel level);
            void setFunction(DebugLevel level);

            void controller(DebugLevel level, String& s);
            void light(DebugLevel level, String& s);
            void sound(DebugLevel level, String& s);
            void drive(DebugLevel level, String& s);
            void function(DebugLevel level, String& s);
    };

#endif



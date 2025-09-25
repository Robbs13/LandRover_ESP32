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

    // Klasse für das Anzeigen von Debugnachrichten, anhand von Debugleveln. Klasse wird einmalig erstellt und ist dann als instance aufrufbar, 
    // die den debug.h inkludiert haben.
    // Klasse kann überall mit getInstance und Methode verwendet werden: Debug::getInstance().setController(DebugLevel::Info);
    class Debug {
        private:
            // Konstruktor privat -> niemand kann von außen ein Objekt erzeugen
            Debug();

            //Destruktor
            ~Debug();  

            // Kopieren und Zuweisung verbieten
            Debug(const Debug&) = delete;
            Debug& operator=(const Debug&) = delete;

            // Variablen für Debugbereiche
            uint8_t debugController = 0;
            uint8_t debugLight = 0;
            uint8_t debugSound = 0;
            uint8_t debugDrive = 0;
            uint8_t debugFunction = 0;

        public:
            // Zugriff auf die einzige Instanz
            static Debug& getInstance();

            // Debuglevel für die einzelne Bereiche setzen
            void setController(DebugLevel level);
            void setLight(DebugLevel level);        
            void setSound(DebugLevel level);        
            void setDrive(DebugLevel level);
            void setFunction(DebugLevel level);

            // Nachrichten mit den Debuglevel schicken. 
            void controller(DebugLevel level, const String& s);
            void light(DebugLevel level, const String& s);
            void sound(DebugLevel level, const String& s);
            void drive(DebugLevel level, const String& s);
            void function(DebugLevel level, const String& s);
    };

#endif



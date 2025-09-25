/*
 * File: ControllerWrapper.h
 *
 * Description:
 *      Singleton-Klasse für Controller Verarbeiung - PS4.
 *      Es wird nur eine Instanz für diese Klasse erzeugt und dann über den inkludierten Header weitergegeben
 *
 * Usage:
 *      PS4Controller.h einbinden
 *      In den Programmen mit ControllerWrapper::getInstance().### aufrufen
 *
 * Notes:
 *      - Nutzt Meyers Singleton
 *      - Debug-Level konfigurierbar via config.h
 */

#ifndef CONTROLLERWRAPPER_H
#define CONTROLLERWRAPPER_H

#include "PS4Controller.h"

class ControllerWrapper {
    public:
        // Zugriff auf die einzige Instanz
        static ControllerWrapper& getInstance();

        typedef void (*Callback)();
        typedef void (*TriggerCallback)(uint8_t value);


        void begin();
        void setEventDownButtonOnCross(Callback cb);
        void setEventDownButtonOnSquare(Callback cb);
        void setEventDownButtonOnTriangle(Callback cb);
        void setEventDownButtonOnCircle(Callback cb);
        void setEventDownButtonOnDown(Callback cb);
        void setEventDownButtonOnLeft(Callback cb);
        void setEventDownButtonOnUp(Callback cb);
        void setEventDownButtonOnRight(Callback cb);
        void setEventDownButtonOnR1(Callback cb);
        void setEventDownButtonOnL1(Callback cb);
        void setEventDownButtonOnR3(Callback cb);
        void setEventDownButtonOnL3(Callback cb);
        void setEventDownButtonOnShare(Callback cb);
        void setEventDownButtonOnPS(Callback cb);
        void setEventDownButtonOnOptions(Callback cb);


    private:
        // Konstruktor privat -> niemand kann von außen ein Objekt erzeugen
        ControllerWrapper();

        // Destruktor
        ~ControllerWrapper();  

        // Kopieren und Zuweisung verbieten
        ControllerWrapper(const ControllerWrapper&) = delete;
        ControllerWrapper& operator=(const ControllerWrapper&) = delete;

        // Methoden
        static void notify();
        static void onConnect();
        static void onDisconnect();

        // Digital Button Callback zum Übergeben an die Funktion
        static Callback onCross;
        static Callback onSquare;
        static Callback onTriangle;
        static Callback onCircle;
        static Callback onDown;
        static Callback onLeft;
        static Callback onUp;
        static Callback onRight;
        static Callback onR1;
        static Callback onL1;
        static Callback onR3;
        static Callback onL3;
        static Callback onShare;
        static Callback onPS;
        static Callback onOptions;

        // Analog Sticks Callback 
           

        

        //Serielle Debug Ausgaben aktiv
        bool debugEnabled = false;

    };
#endif
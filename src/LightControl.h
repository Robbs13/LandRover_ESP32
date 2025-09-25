#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

#include <Arduino.h>



class LightControl {
    private:
        // Konstruktor privat -> niemand kann von außen ein Objekt erzeugen
        LightControl();

        //Destruktor
        ~LightControl();  

        // Kopieren und Zuweisung verbieten
        LightControl(const LightControl&) = delete;
        LightControl& operator=(const LightControl&) = delete;

        

        //Serielle Debug Ausgaben aktiv
        // boolean debugEnabled = false;

        boolean headLightActive = false;
        boolean rearLightActive = false;



    public:
        // Zugriff auf die einzige Instanz
        static LightControl& getInstance();

        void begin();
        void lightOn();

        // void onCrossPressed() {
        //     lightOn();   // Licht einschalten
        //     }
        
        // statische Callback-Methode
        static void onCrossPressed() {
            getInstance().lightOn();
        }


};
#endif
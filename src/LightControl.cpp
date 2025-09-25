#include "LightControl.h"
#include "ControllerWrapper.h"



// Zugriffsmethode für die einzige Instanz
    LightControl& LightControl::getInstance() {
        static LightControl instance;   // wird nur einmal erzeugt
    return instance;
    }

    //Konstruktor
    LightControl::LightControl(){
    // if(debugEnabled){Serial.println("Controller Verbindung gestartet");};
    }

    //Destruktor
    LightControl::~LightControl() {
        
        Serial.println("Serielle Schnittstelle beendet - Debug Instanz wurde zerstört");
    }


    void LightControl::lightOn(){
        if (!headLightActive){
            Serial.println("HeadLight an");
            Serial.println("RearLight an");
            Serial.println("PosLight an");
            headLightActive = true;
        }
        else{
            Serial.println("HeadLight aus");
            Serial.println("RearLight aus");
            Serial.println("PosLight aus");
            headLightActive = false;
        }
        
        
    }

    /*void onCrossPressed() {
    light.on();   // Licht einschalten
}*/
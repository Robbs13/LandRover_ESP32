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
            digitalWrite(2, HIGH); 
        }
        else{
            Serial.println("HeadLight aus");
            Serial.println("RearLight aus");
            Serial.println("PosLight aus");
            digitalWrite(2, LOW); 
        }    
        headLightActive = !headLightActive;   
        
    }

    void LightControl::headLightBeam(){
        if (!headLightBeamActive){
            Serial.println("Fernlicht an");  
    
        }
        else{
            Serial.println("Fernlicht aus");
            
        }
        headLightBeamActive = !headLightBeamActive;
    }

    void LightControl::blinkLeft(){
        if (!blinkLeftActive){
            Serial.println("Blinken links an");  
    
        }
        else{
            Serial.println("Blinken links aus");
            
        }
        blinkLeftActive = !blinkLeftActive;
    }

    void LightControl::blinkRight(){
        if (!blinkRightActive){
            Serial.println("Blinken rechts an");  
    
        }
        else{
            Serial.println("Blinken rechts aus");
            
        }
        blinkRightActive = !blinkRightActive;
    }

    void LightControl::workLight(){
        if (!workLightActive){
            Serial.println("Arbeitsleuchte an");  
    
        }
        else{
            Serial.println("Arbeitsleuchte aus");
            
        }
        workLightActive = !workLightActive;
    }

    

    /*void onCrossPressed() {
    light.on();   // Licht einschalten
}*/
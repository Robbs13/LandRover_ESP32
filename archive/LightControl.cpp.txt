#include "LightControl.h"
//#include "ControllerWrapper.h"
#include "TaskQueueFunction.h"
#include "04_Controller.h"



// Zugriffsmethode für die einzige Instanz
    LightControl& LightControl::getInstance() {
        static LightControl instance;   // wird nur einmal erzeugt
    return instance;
    }

    //Konstruktor
    LightControl::LightControl(){
    // lightJobIdLeft = taskFunc.addJob([&]() { LightControl::getInstance().jobBlinkLeft(); });
    // taskFunc.pauseJob(lightJobIdLeft);
    // lightJobIdRight = taskFunc.addJob([&]() { LightControl::getInstance().jobBlinkLeft(); });
    // taskFunc.pauseJob(lightJobIdRight);
    blinkJob = taskFunc.addJob([&]() { LightControl::getInstance().jobBlink(); });
    taskFunc.pauseJob(blinkJob);

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
            //digitalWrite(2, HIGH); 
        }
        else{
            Serial.println("HeadLight aus");
            Serial.println("RearLight aus");
            Serial.println("PosLight aus");
            //digitalWrite(2, LOW); 
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
            //digitalWrite(PIN_BLINKLEFT, HIGH);
            taskFunc.resumeJob(blinkJob);
            
        }
        if (blinkLeftActive){
            Serial.println("Blinken links aus");
            stopRequested = true;
            //blinkLeftActive = false;
        }
        blinkLeftActive = !blinkLeftActive;
    }

    void LightControl::blinkRight(){
        if (!blinkRightActive){
            Serial.println("Blinken rechts an");  
            //digitalWrite(PIN_BLINKRIGHT, HIGH);
            taskFunc.resumeJob(blinkJob);
            
        }
        if (blinkRightActive){
            Serial.println("Blinken rechts aus");
            stopRequested = true;
            //blinkRightActive = false;‚
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

    // void LightControl::jobBlinkLeft(){
        
    //     unsigned long now = millis();
    //     if (now - lastToggle >= interval) {
    //         state = !state;
    //         digitalWrite(PIN_BLINKLEFT, state ? HIGH : LOW);
    //         lastToggle = now;

    //         // Zyklusende prüfen
    //         if (stopRequested && state == false) {  
    //             taskFunc.pauseJob(lightJobIdLeft);  // nur wenn LED gerade aus ist
    //             stopRequested = false;
    //             Serial.println("Blinker gestoppt (Zyklusende erreicht)");
    //         }
    //     }

    // }

    // void LightControl::jobBlinkRight(){
        
    //     unsigned long now = millis();
    //     if (now - lastToggle >= interval) {
    //         state = !state;
    //         digitalWrite(PIN_BLINKRIGHT, state ? HIGH : LOW);
    //         lastToggle = now;

    //         // Zyklusende prüfen
    //         if (stopRequested && state == false) {  
    //             taskFunc.pauseJob(lightJobIdRight);  // nur wenn LED gerade aus ist
    //             stopRequested = false;
    //             Serial.println("Blinker gestoppt (Zyklusende erreicht)");
    //         }
    //     }

    // }
    void LightControl::jobBlink(){

        
        unsigned long now = millis();
        if (now - lastToggle >= interval) {
            state = !state;
            
            digitalWrite(PIN_BLINKLEFT, state ? HIGH : LOW);
                //Serial.println(state);
            
            
            digitalWrite(PIN_BLINKRIGHT, state ? HIGH : LOW);
            
            lastToggle = now;

            // Zyklusende prüfen
            if (stopRequested && state == false) {  
                taskFunc.pauseJob(blinkJob);  // nur wenn LED gerade aus ist
                Serial.println("Pause Job");
                stopRequested = false;
            }
        }

    }


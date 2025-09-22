#include "Debug.h"
#include <Arduino.h>

    // Zugriffsmethode
    Debug& Debug::getInstance() {
        static Debug instance;   // wird nur einmal erzeugt
    return instance;
}

    Debug::Debug(){
        Serial.begin(115200);
    }

    void Debug::setController(DebugLevel level){
        debugController = static_cast<uint8_t>(level);
    }

    void Debug::setLight(DebugLevel level){
        debugLight = static_cast<uint8_t>(level);
    }

    void Debug::setSound(DebugLevel level){
        debugSound = static_cast<uint8_t>(level);
    }
    
    void Debug::setDrive(DebugLevel level){
        debugDrive = static_cast<uint8_t>(level);
    }

    void Debug::setFunction(DebugLevel level){
        debugFunction = static_cast<uint8_t>(level);
    }

    void Debug::controller(DebugLevel level, const String& s){
          if (static_cast<uint8_t>(debugController) >= static_cast<uint8_t>(level)){
          Serial.println(s);
          }
    }

    void Debug::light(DebugLevel level, const String& s){
          if (static_cast<uint8_t>(debugLight) >= static_cast<uint8_t>(level)){
          Serial.println(s);
          }
    }

    void Debug::sound(DebugLevel level, const String& s){
          if (static_cast<uint8_t>(debugSound) >= static_cast<uint8_t>(level)){
          Serial.println(s);
          }
    }

    void Debug::drive(DebugLevel level, const String& s){
          if (static_cast<uint8_t>(debugDrive) >= static_cast<uint8_t>(level)){
          Serial.println(s);
          }
    }

    void Debug::function(DebugLevel level, const String& s){
          if (static_cast<uint8_t>(debugFunction) >= static_cast<uint8_t>(level)){
          Serial.println(s);
          }
    }
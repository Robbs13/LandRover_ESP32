#include "MapperFunction.h"
#include "ControllerWrapper.h"
#include "LightControl.h"
#include "05_Function.h"
#include "03_Remote.h"



    // Zugriffsmethode für die einzige Instanz
    MapperFunction& MapperFunction::getInstance() {
        static MapperFunction instance;   // wird nur einmal erzeugt
    return instance;
    }

    //Konstruktor
    MapperFunction::MapperFunction(){}

    //Destruktor
    MapperFunction::~MapperFunction(){}

    #ifdef ControllerPS4

    void MapperFunction::begin(){
        Serial.println("MapperFunction PS4 begin.");

        #ifdef FUNC_HEADLIGHTS
            ControllerWrapper::getInstance().setEventDownButtonOnTriangle(LightControl::getInstance().onLightSwitch);
        #endif

        ControllerWrapper::getInstance().setEventDownButtonOnSquare(LightControl::getInstance().onBlinkLeft);
        ControllerWrapper::getInstance().setEventDownButtonOnCircle(LightControl::getInstance().onBlinkRight);
        ControllerWrapper::getInstance().setEventDownButtonOnUp(LightControl::getInstance().onWorkLight);

        //Light mapping
        
        }
    #else
        Serial.println("Kein RemoteController in Remote Header ausgewählt.")
    #endif
        
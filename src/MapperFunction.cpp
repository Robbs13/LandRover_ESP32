#include "MapperFunction.h"
#include "ControllerWrapper.h"
#include "LightControl.h"



    // Zugriffsmethode für die einzige Instanz
    MapperFunction& MapperFunction::getInstance() {
        static MapperFunction instance;   // wird nur einmal erzeugt
    return instance;
    }

    //Konstruktor
    MapperFunction::MapperFunction(){}

    //Destruktor
    MapperFunction::~MapperFunction(){}


    void MapperFunction::begin(){
        Serial.println("MapperFunction begin.");

        //Light mapping
        ControllerWrapper::getInstance().setOnCross(LightControl::getInstance().onCrossPressed);
        }
        
        
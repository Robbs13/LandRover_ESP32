#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

#include <Arduino.h>
#include "TaskQueueFunction.h"



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
        boolean headLightBeamActive = false;
        boolean blinkRightActive = false;
        boolean blinkLeftActive = false;
        boolean workLightActive = false;

        TaskQueueFunction taskFunc;
        int lightJobIdLeft;
        int lightJobIdRight;
        int blinkJob;



    public:
        // Zugriff auf die einzige Instanz
        static LightControl& getInstance();

        void begin();
        void lightOn();
        void headLightBeam();
        void rearLightBrake();
        void blinkLeft();
        void blinkRight();
        void reverseLight();
        void workLight();

        // Jobs die an einen FreeRtos Task übergeben werden
        // void jobBlinkLeft();
        // void jobBlinkRight();
        void jobBlink();

        bool stateBlinkLeft = false;
        bool stateBlinkRight = false;
        bool state = false;
        unsigned long lastToggle = 0;
        const unsigned long interval = 500; // 0,5s
        bool stopRequestedBlinkLeft = false;
        bool stopRequestedBlinkRight = false;
         bool stopRequested = false;

      


        
        // statische Callback-Methode
        static void onLightSwitch() {getInstance().lightOn();}
        static void onBeamSwitch() {getInstance().headLightBeam();}
        static void onBlinkRight() {getInstance().blinkRight();}
        static void onBlinkLeft() {getInstance().blinkLeft();}
        static void onWorkLight() {getInstance().workLight();}


};
#endif
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
        boolean headLightBeamActive = false;
        boolean blinkRightActive = false;
        boolean blinkLeftActive = false;
        boolean workLightActive = false;



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


        
        // statische Callback-Methode
        static void onLightSwitch() {getInstance().lightOn();}
        static void onBeamSwitch() {getInstance().headLightBeam();}
        static void onBlinkRight() {getInstance().blinkRight();}
        static void onBlinkLeft() {getInstance().blinkLeft();}
        static void onWorkLight() {getInstance().workLight();}


};
#endif
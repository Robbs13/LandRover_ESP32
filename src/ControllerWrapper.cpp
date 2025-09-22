#include "ControllerWrapper.h"
#include <Arduino.h>

ControllerWrapper* ControllerWrapper::instance = nullptr;
unsigned long lastTimeStamp = 0;

void ControllerWrapper::begin() {
    instance = this;
    
    PS4.attach(notify);
    PS4.attachOnConnect(onConnect);
    PS4.attachOnDisconnect(onDisconnect);
    PS4.begin(); 
    while (PS4.isConnected() == false) 
    { 
        Serial.println("PS Controller not found");
        delay(1000);
    }
      Serial.begin(115200);
    PS4.begin();
    Serial.println("Ready.");
    Serial.printf("Battery Level : %d\n", PS4.Battery());
    
    Serial.println("Ready.");
}

// Setter
void ControllerWrapper::setOnDpadUp(Callback cb)    { onDpadUp = cb; }
void ControllerWrapper::setOnDpadDown(Callback cb)  { onDpadDown = cb; }
void ControllerWrapper::setOnDpadLeft(Callback cb)  { onDpadLeft = cb; }
void ControllerWrapper::setOnDpadRight(Callback cb) { onDpadRight = cb; }

void ControllerWrapper::setOnCross(Callback cb)     { onCross = cb; }
void ControllerWrapper::setOnCircle(Callback cb)    { onCircle = cb; }
void ControllerWrapper::setOnSquare(Callback cb)    { onSquare = cb; }
void ControllerWrapper::setOnTriangle(Callback cb)  { onTriangle = cb; }
void ControllerWrapper::setOnL1(Callback cb)        { onL1 = cb; }
void ControllerWrapper::setOnR1(Callback cb)        { onR1 = cb; }

void ControllerWrapper::setOnLStickMove(Callback cb) { onLStickMove = cb; }
void ControllerWrapper::setOnRStickMove(Callback cb) { onRStickMove = cb; }

void ControllerWrapper::setOnL2(TriggerCallback cb) { onL2 = cb; }
void ControllerWrapper::setOnR2(TriggerCallback cb) { onR2 = cb; }


bool ControllerWrapper::isConnected() const { return PS4.isConnected(); }

void ControllerWrapper::notify() {
    if (!instance) return;

    




    

    //Only needed to print the message properly on serial monitor. Else we dont need it.
    if ((millis() - lastTimeStamp) > 1000)
    {
        u_int8_t *test = PS4.data.latestPacket;
        Serial.println('test');
        lastTimeStamp = millis();
    }
    
  
    
     // D-Pad
    if (PS4.event.button_down.up && instance->onDpadUp)       instance->onDpadUp();
    if (PS4.event.button_down.down && instance->onDpadDown)   instance->onDpadDown();
    if (PS4.event.button_down.left && instance->onDpadLeft)   instance->onDpadLeft();
    if (PS4.event.button_down.right && instance->onDpadRight) instance->onDpadRight();

    // Buttons
    if (PS4.event.button_down.cross && instance->onCross)     instance->onCross();
    if (PS4.event.button_down.circle && instance->onCircle)   instance->onCircle();
    if (PS4.event.button_down.square && instance->onSquare)   instance->onSquare();
    if (PS4.event.button_down.triangle && instance->onTriangle) instance->onTriangle();
    if (PS4.event.button_down.l1 && instance->onL1)           instance->onL1();
    if (PS4.event.button_down.r1 && instance->onR1)           instance->onR1();

    // Sticks (nur Bewegungen)
    if ((PS4.event.analog_move.stick.lx || PS4.event.analog_move.stick.ly) && instance->onLStickMove)
        instance->onLStickMove();
    if ((PS4.event.analog_move.stick.rx || PS4.event.analog_move.stick.ry) && instance->onRStickMove)
        instance->onRStickMove();

    // Trigger (analog)
    if (instance->onL2) instance->onL2(PS4.L2Value());
    if (instance->onR2) instance->onR2(PS4.R2Value());
}

void ControllerWrapper::onConnect(){ 
    Serial.println("Controller verbunden!"); 
    uint8_t batteryStatus = PS4.data.status.battery;
    Serial.println(batteryStatus);
    uint8_t battery = PS4.Battery();
    Serial.println(battery);

}
void ControllerWrapper::onDisconnect() { Serial.println("Controller getrennt!"); }

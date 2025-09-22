#ifndef CONTROLLERWRAPPER_H
#define CONTROLLERWRAPPER_H

#include "PS4Controller.h"

class ControllerWrapper {
public:
    typedef void (*Callback)();
    typedef void (*TriggerCallback)(uint8_t value);

    void begin();
    
    // D-Pad
    void setOnDpadUp(Callback cb);
    void setOnDpadDown(Callback cb);
    void setOnDpadLeft(Callback cb);
    void setOnDpadRight(Callback cb);

    // Buttons
    void setOnCross(Callback cb);
    void setOnCircle(Callback cb);
    void setOnSquare(Callback cb);
    void setOnTriangle(Callback cb);
    void setOnL1(Callback cb);
    void setOnR1(Callback cb);

    // Sticks
    void setOnLStickMove(Callback cb);
    void setOnRStickMove(Callback cb);

    // Trigger
    void setOnL2(TriggerCallback cb);
    void setOnR2(TriggerCallback cb);

    bool isConnected() const;

private:
    static ControllerWrapper* instance;

    // D-Pad Callbacks
    Callback onDpadUp = nullptr;
    Callback onDpadDown = nullptr;
    Callback onDpadLeft = nullptr;
    Callback onDpadRight = nullptr;

    // Buttons
    Callback onCross = nullptr;
    Callback onCircle = nullptr;
    Callback onSquare = nullptr;
    Callback onTriangle = nullptr;
    Callback onL1 = nullptr;
    Callback onR1 = nullptr;

    // Sticks
    Callback onLStickMove = nullptr;
    Callback onRStickMove = nullptr;

    // Trigger
    TriggerCallback onL2 = nullptr;
    TriggerCallback onR2 = nullptr;

    static void notify();
    static void onConnect();
    static void onDisconnect();
};

#endif

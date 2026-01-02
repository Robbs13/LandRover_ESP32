#include "CtrlSim.h"
#include "Config.h"   // für qRCCom, RcRawFrame etc.

// ---------------------------------------------------------
//  Klasse RcCom für die Kommunikation mit dem RC Sender
// ---------------------------------------------------------
CtrlSim::CtrlSim(int n)
: _taskHandle(NULL)
{

}

// ---------------------------------------------------------
//  UART Initialsierung
// ---------------------------------------------------------
void CtrlSim::begin() {
    // Pins ggf. aus Config nehmen: g_ioPins.pinXlrsRx / pinXlrsTx
    // _crsf.begin(CRSF_BAUDRATE, SERIAL_8N1, 16, 17); 
    Serial.println("CTRLSIM: Begin...");
}

// ---------------------------------------------------------
//  Task erstellen
// ---------------------------------------------------------
bool CtrlSim::startTask(UBaseType_t priority, BaseType_t core, uint8_t time) {
    _cycleTime = time;
    
    BaseType_t res = xTaskCreatePinnedToCore(
        CtrlSim::taskTrampoline,  // statische Entry-Funktion
        "CtrlSim",                // Name (für Debug)
        4096,                   // Stack-Größe (Wörter, nicht Bytes)
        this,                   // this als Parameter übergeben!
        priority,               // Priorität
        &_taskHandle,           // Task-Handle
        core                    // Core 0 oder 1
    );
    return (res == pdPASS);
}
void CtrlSim::taskTrampoline(void *pvParameters) {
    // pvParameters ist der this-Pointer
    CtrlSim *self = static_cast<CtrlSim*>(pvParameters);
    self->taskLoop();   // jetzt sind wir im Instanz-Kontext
}



// ---------------------------------------------------------
//  Task Loop für den Ablauf 
// ---------------------------------------------------------
void CtrlSim::taskLoop() {
    begin();
    //
    //ControlCommandData cc{};

    const TickType_t period = pdMS_TO_TICKS(_cycleTime);     // Zykluszeit der Taskschleife
    TickType_t lastWake = xTaskGetTickCount();

    _failsafeActive = false;

    // Control Datenstruktur mit Failsafe vorbelegen
    _cc.blinkenRightAct = false;
    _cc.blinkenLeftAct = false;
    _cc.lightHeadAct = false;
    _cc.lightBeamAct = false;
    _cc.lightRearAct = false;
    _cc.lightBrakeAct = false;
    _cc.lightReverseAct = false;
    _cc.lightPosAct = false;
    _cc.lightWorkAct = false;
    _cc.lightTachoAct = false;
    _cc.ctrlDrive = RC_MIDDLE;
    _cc.ctrlSteer = RC_MIDDLE;
    _cc.ctrlTransmission = RC_MIDDLE;
    _cc.ctrlWinch = RC_MIDDLE;
    _cc.ctrlShaker = RC_MIDDLE;

    // Queue wird mit Failsafe Daten befüllt, bis erste gültige Nachricht empfangen wird
    // _lastFailsafeSend = 0;

    // Task Schleife
    for (;;)
    {   
        bool newChannelData = false;
        newChannelData = fetchLatestRcInput();

        // failSafeActive();

        // if (!_failsafeActive) handleLight();

        // // if (!_failsafeActive) handleSim();

        // publishCtrlQueue();


        // if (_failsafeActive){
        //     //Blinken und Stillstand
        //     Serial.println("CTRLSIM: RC FAILSAFE aktiv");
        // }
        // else if (newChannelData){
        //     Serial.println("CTRLSIM: New RC Data");
        // }
        // else{
        //     Serial.println("CTRLSIM: Kein Failsafe, aber auch keine neuen RC Daten");
        // }
        

        // Wenn Debug Schnittstelle aktiviert wurde, werden die einzelne Rohwerte der Kanäle ausgegeben
        // #if defined(DEBUG_CTRLSIM)
        //     debug();
        // #endif
        

        // Task läuft genau alle 5 ms
        vTaskDelayUntil(&lastWake, period);
    }
}


// ---------------------------------------------------------
//  Aktuelle RC Inputs aus Queue qRCCom holen
// ---------------------------------------------------------
bool CtrlSim::fetchLatestRcInput()
{
    if (!qRCCom) return false;

    bool gotRC = false;

    // Queue leeren, wir behalten den neuesten
    while (xQueueReceive(qRCCom, &_rc, 0) == pdTRUE)
        gotRC = true;

    return gotRC;
}

// ---------------------------------------------------------
//  Failsafe check und Datenstruktur anpassen
// ---------------------------------------------------------
void CtrlSim::failSafeActive()
{
    if (_rc.FailSafeRC){
        _failsafeActive = true;

        // Control Datenstruktur mit Failsafe vorbelegen
        _cc.blinkenRightAct = true;
        _cc.blinkenLeftAct = true;
        _cc.lightHeadAct = false;
        _cc.lightBeamAct = false;
        _cc.lightRearAct = false;
        _cc.lightBrakeAct = true;
        _cc.lightReverseAct = false;
        _cc.lightPosAct = false;
        _cc.lightWorkAct = false;
        _cc.lightTachoAct = false;
        _cc.ctrlDrive = RC_MIDDLE;
        _cc.ctrlSteer = RC_MIDDLE;
        _cc.ctrlTransmission = RC_MIDDLE;
        _cc.ctrlWinch = RC_MIDDLE;
        _cc.ctrlShaker = RC_MIDDLE;

    }


    return;
}

// ---------------------------------------------------------
//  Methode für die Lichtsteuerung
// ---------------------------------------------------------
void CtrlSim::handleLight()
{
    #if defined(FUNC_HEADLIGHTS_BEAM)

    #endif


    return;
}

// ---------------------------------------------------------
//  Control Data an Queue qControl übergeben
// ---------------------------------------------------------
void CtrlSim::publishCtrlQueue()
{
    // if (qRCCom != NULL) {
    //     // Neu geparste Channel Daten werden an die Queue übergeben
    //     if(_newChannelData){            
    //         RcInputData frame{};
    //         frame.timestampMs = _lastPacketMs;
    //         for (int i = 0; i < CRSF_CHANNEL; ++i) {
    //             frame.channel[i] = _rcChannel[i];
    //         }
    //         frame.FailSafeRC = _failsafeActive;
    //         xQueueOverwrite(qRCCom, &frame);

    //         _lastFailsafeSend = 0;
    //         //Serial.println("RCCOM: CRSF Data - New Data in Queue");               
    //     }

    //     if(_failsafeActive){
    //         // Sobald Failsafe, Nachricht schicken - Dannach alle RC_FAILSAFE_CYCLE in ms
    //         if((_now - _lastFailsafeSend  >= RC_FAILSAFE_CYCLE) || _lastFailsafeSend == 0){
            
    //             RcInputData fsFrame{};
    //             fsFrame.timestampMs = _now;
    //             for (int i = 0; i < CRSF_CHANNEL; ++i) {
    //                 fsFrame.channel[i] = 992; // Mittelstellung / safe
    //             }
    //             fsFrame.FailSafeRC = _failsafeActive;
    //             xQueueOverwrite(qRCCom, &fsFrame);

    //             _lastFailsafeSend = _now;
    //             //Serial.println("RCCOM: CRSF FAILSAFE - Nachricht in Queue");
                
    //         }
    //     }    
    // }
}
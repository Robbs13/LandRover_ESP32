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

    pinMode(15, OUTPUT);
    //
    // Zustand Parken nach Aufstart
    //setPWF(PWFStatus::Parken);

    const TickType_t period = pdMS_TO_TICKS(_cycleTime);     // Zykluszeit der Taskschleife
    TickType_t lastWake = xTaskGetTickCount();


    _newRcData      = false;
    _failsafeActive = true;
    _newControlData = false;

    getIdxConfig();
    
    setPWF(PWFStatus::Parken);


    // Queue wird mit Failsafe Daten befüllt, bis erste gültige Nachricht empfangen wird
    // _lastFailsafeSend = 0;

    // Task Schleife
    for (;;)
    {   
         // Aktuelle Zeit speichern
        u_int32_t startTaskTime= millis();
        _newRcData      = false;

        _gpioFrame.gpioCount = 0;

        

        //bool newChannelData = false;
        handleRcQueue();

        failSafeCheck();       
        
        //if(_failsafeActive) setPWF(PWFStatus::Parken);

        // Switch Case für PWF - Ist Case Fahren aktiv, wird durch fallthrough auch Wohnen bearbeitet
        switch (_statePWF) {
            case PWFStatus::Fahren:
                // handleStartButton();  
                // handleGear;
                // handleDrive;
                // handleSteer;
                // 
                // handleWinch;
                // 
                // handleSound;
                // handleShaker;
                handleLights();
                handleBlinkLights();
                break;

            case PWFStatus::Wohnen:
                //handleStartButton();
                handleLights();
                handleBlinkLights();
                break;

            case PWFStatus::Parken:
                failSafeHandle();
                //handleBlinkLights();
                break;
        }
        _newControlData = false;


        publishGPIOQueue();



        //Wenn Debug Schnittstelle aktiviert wurde, werden die einzelne Rohwerte der Kanäle ausgegeben
        #if defined(DEBUG_CTRLSIM)
            debug();
        #endif
        //Serial.println(_failsafeActive);

        _newControlData = false;
        

        #if defined(DEBUG_CTRLSIM_TASK_CYCLE)
            //vTaskDelay(pdMS_TO_TICKS(12));
            u_int8_t timeTask = millis() - startTaskTime;
            if (_cycleTime <= timeTask){
                Serial.print("CTRLSIM: Aktuelle Task Abarbeitung = Max Cycle Time ");
                Serial.println(timeTask);
            }
            // Serial.print("CTRLSIM: TaskTime - ");
            // Serial.println(timeTask);
        #endif

        // Task läuft genau alle 5 ms
        vTaskDelayUntil(&lastWake, period);
    }
}

// ---------------------------------------------------------
//  Aktuelle RC Daten aus Config mappen
// ---------------------------------------------------------
void CtrlSim::getIdxConfig(){

    int _idxBlinkLeft       = -2;
    int _idxBlinkRight      = -2;
    int _idxHeadLight       = -2;
    int _idxBeamLight       = -2;
    int _idxRearLight       = -2;
    int _idxBrakeLight      = -2;
    int _idxReverseLight    = -2;
    int _idxPosLight        = -2;
    int _idxWorkLight       = -2;
    int _idxCabinLight      = -2;

    
    if (_idxBlinkLeft < 0) {
        _idxBlinkLeft = findFirstMapIndex(FunctionList::handleBlinkLeft);
        if (_idxBlinkLeft < 0) return;
    }
    if (_idxBlinkRight < 0) {
        _idxBlinkRight = findFirstMapIndex(FunctionList::handleBlinkRight);
        if (_idxBlinkRight < 0) return;
    }
    if (_idxHeadLight < 0) {
        _idxHeadLight = findFirstMapIndex(FunctionList::handleLightHead);
        if (_idxHeadLight < 0) return;
    }
    if (_idxBeamLight < 0) {
        _idxBeamLight = findFirstMapIndex(FunctionList::handleLightBeam);
        if (_idxBeamLight < 0) return;
    }
    if (_idxRearLight < 0) {
        _idxRearLight = findFirstMapIndex(FunctionList::handleLightRear);
        if (_idxRearLight < 0) return;
    }
    if (_idxBrakeLight < 0) {
        _idxBrakeLight = findFirstMapIndex(FunctionList::handleLightBrake);
        if (_idxBrakeLight < 0) return;
    }
    if (_idxReverseLight < 0) {
        _idxReverseLight = findFirstMapIndex(FunctionList::handleLightReverse);
        if (_idxReverseLight < 0) return;
    }
    if (_idxPosLight < 0) {
        _idxPosLight = findFirstMapIndex(FunctionList::handleLightPos);
        if (_idxPosLight < 0) return;
    }
    if (_idxWorkLight < 0) {
        _idxWorkLight = findFirstMapIndex(FunctionList::handleLightWork);
        if (_idxWorkLight < 0) return;
    }
    if (_idxCabinLight < 0) {
        _idxCabinLight = findFirstMapIndex(FunctionList::handleLightCabin);
        if (_idxCabinLight < 0) return;
    }

    // -------- Config Mapper --------
    _mapBlinkLeft       = &cfg_rcGpioMap[static_cast<size_t>(_idxBlinkLeft)];
    _mapBlinkRight      = &cfg_rcGpioMap[static_cast<size_t>(_idxBlinkRight)];
    _mapHeadLight       = &cfg_rcGpioMap[static_cast<size_t>(_idxHeadLight)];
    _mapBeamLight       = &cfg_rcGpioMap[static_cast<size_t>(_idxBeamLight)];
    _mapRearLight       = &cfg_rcGpioMap[static_cast<size_t>(_idxRearLight)];
    _mapBrakeLight      = &cfg_rcGpioMap[static_cast<size_t>(_idxBrakeLight)];
    _mapReverseLight    = &cfg_rcGpioMap[static_cast<size_t>(_idxReverseLight)];
    _mapPosLight        = &cfg_rcGpioMap[static_cast<size_t>(_idxPosLight)];
    _mapWorkLight       = &cfg_rcGpioMap[static_cast<size_t>(_idxWorkLight)];
    _mapCabinLight      = &cfg_rcGpioMap[static_cast<size_t>(_idxCabinLight)];
}

// ---------------------------------------------------------
//  Aktuelle RC Inputs aus Queue qRCCom holen
// ---------------------------------------------------------
void CtrlSim::handleRcQueue()
{
    if (!q_CRSF) return;

    uint32_t missed = 0;    
    uint32_t _lastReceive = _rc.timestampMs;

    // -------- Nur die neueste Nachricht wird behalten --------
    while (xQueueReceive(q_CRSF, &_rc, 0) == pdTRUE) {
        missed++;
        _newRcData = true;
    }

    #if defined(DEBUG_CTRLSIM_RC_MISSED)
        if (missed > 1) {
            uint32_t delta = _rc.timestampMs - _lastReceive;
            Serial.print("CTRLSIM: RC Frames verpasst. Anzahl in Buffer: ");
            Serial.print(missed);
            Serial.print(" - Delta zum letzten Frame: ");
            Serial.println(delta);
        }
    #endif
    
    return;
}

// ---------------------------------------------------------
//  Failsafe check und PWF Status setzen
// ---------------------------------------------------------
void CtrlSim::failSafeCheck()
{
    // -------- Warten auf neue Nachricht --------
    if (!_newRcData) return;

    // -------- Warnblinken wenn RC UpLink Fehler --------
    if (_rc.state != RcLinkState::UpLink){
        _failsafeActive = true; 
        setPWF(PWFStatus::Parken);
        Serial.println("CTRLSIM: RC Downlink - Failsafe aktiv - PWF Status Parken");
    }
    else{
        _failsafeActive = false; 
        if (_statePWF == PWFStatus::Parken){
            setPWF(PWFStatus::Wohnen);
        }
    }
  
    _gpioFrame.failsafe = _failsafeActive;

    return;
}

// ---------------------------------------------------------
//  Failsafe abhandeln: Warnblink und Ausgänge auf Failsafe
// ---------------------------------------------------------
void CtrlSim::failSafeHandle()
{
    
    // -------- Warnblinken --------
    handleBlinkLights();

    // -------- Light Output auf Default Werte --------
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapHeadLight->gpioConfig),     CTRL_FAILSAFE_VALUE });
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapBeamLight->gpioConfig),     CTRL_FAILSAFE_VALUE });
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapRearLight->gpioConfig),     CTRL_FAILSAFE_VALUE });  
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapBrakeLight->gpioConfig),    CTRL_FAILSAFE_VALUE });
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapReverseLight->gpioConfig),  CTRL_FAILSAFE_VALUE });  
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapPosLight->gpioConfig),      CTRL_FAILSAFE_VALUE });   
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapWorkLight->gpioConfig),     CTRL_FAILSAFE_VALUE });  
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapCabinLight->gpioConfig),    CTRL_FAILSAFE_VALUE });  

    // for (int i = 0; i < CRSF_NUM_CHANNELS; i++) {
    //         _rc.channel[i] = 1500;
    //     }

}

// ---------------------------------------------------------
//  Methode für die Lichtsteuerung
// ---------------------------------------------------------
void CtrlSim::handleLights()
{
    // -------- Inputs --------
    const bool rc_lightOn  = (_rc.channel[int(_mapHeadLight->channelIndex)] > CTRL_POS_2P_ON);
    const bool rc_beamPressed = (_rc.channel[int(_mapBeamLight->channelIndex)] > CTRL_POS_2P_ON);
    const bool rc_workLightOn  = (_rc.channel[int(_mapWorkLight->channelIndex)] > CTRL_POS_2P_ON);
    

    // ##### Ersetzen ####
    const bool rc_brakePressed = ((900 < _rc.channel[2]) && (_rc.channel[2] < 1100));
    const bool rc_ReversePressed = ((900 < _rc.channel[3]) && (_rc.channel[3] < 1100));
    const bool rc_startEngine  = (_rc.channel[2] > 1800);

    // -------- Beamtaster für Langdruck --------
    const bool beamPressPos   =  rc_beamPressed && !_beamPressedPrev;

    if (beamPressPos) {
        _beamPressStartMs = millis();
        _beamLongHandled = false;
    }
    const bool beamLongPressed =
        (rc_beamPressed && !_beamLongHandled && (millis() - _beamPressStartMs >= BEAM_LONG_MS));

    if (beamLongPressed) _beamLongHandled = true;


    // -------- LightState change --------
    switch (_base)
    {
        case LightState::Off:
            if (rc_lightOn) _base = LightState::LightOn;
            else if (rc_beamPressed) _base = LightState::Flash;
            break;

        case LightState::Flash:
            if (!rc_beamPressed) _base = LightState::Off;
            break;

        case LightState::LightOn:
            if (!rc_lightOn) _base = LightState::Off;
            else if (beamPressPos) _base = LightState::BeamFlash;
            break;

        case LightState::BeamFlash:
            if (!rc_lightOn) _base = LightState::Off;
            else if (!rc_beamPressed) _base = LightState::LightOn;
            else if (beamLongPressed) _base = LightState::BeamOn;
            break;

        case LightState::BeamOn:
            if (!rc_lightOn) _base = LightState::Off;
            else if (beamLongPressed) _base = LightState::LightOn; // <-- raus per Longpress
            // kurzer Druck: absichtlich keine Aktion
            break;
    }
    _beamPressedPrev = rc_beamPressed;


    // -------- Output change --------
    uint16_t valueHead      = LIGHT_OUT;
    uint16_t valueRear      = LIGHT_OUT;
    uint16_t valueReverse   = LIGHT_OUT;
    uint16_t valuePos       = LIGHT_OUT;
    uint16_t valueWork      = LIGHT_OUT;

    switch (_base)
    {
        case LightState::Off:
            valueHead = LIGHT_OUT;
            valueRear = LIGHT_OUT;
            valuePos  = LIGHT_OUT;
            // Serial.println("off");
            break;

        case LightState::Flash:
            valueHead = BEAM_BRIGHTNESS;
            valueRear = LIGHT_OUT;
            valuePos  = LIGHT_OUT;
            //Serial.println("Flash");
            break;

        case LightState::LightOn:
            valueHead = HEAD_BRIGHTNESS;
            valueRear = REAR_BRIGHTNESS;  
            valuePos  = POS_BRIGHTNESS;  
            //Serial.println("Lighton");        
            break;

        case LightState::BeamFlash:
            valueHead = BEAM_BRIGHTNESS;
            valueRear = REAR_BRIGHTNESS;
            valuePos  = POS_BRIGHTNESS; 
            //Serial.println("BEamFlash");
            break;

        case LightState::BeamOn:
            valueHead = BEAM_BRIGHTNESS;
            valueRear = REAR_BRIGHTNESS; 
            valuePos  = POS_BRIGHTNESS;  
            //Serial.println("BEamOn");        
            break;
    }


    // -------- Overlays --------
    if (rc_brakePressed)    valueRear    = BRAKE_BRIGHTNESS;   
    if (rc_ReversePressed)  valueReverse = REVERSE_BRIGHTNESS;
    if (rc_workLightOn)     valueWork    = WORK_BRIGHTNESS;    


    // -------- Overlay Spannungseinbruch beim Starten--------
    if (rc_startEngine) {
        const uint32_t now = millis();
        tickFlicker(_startFlicker, now);    // Wichtig: tickFlicker nur 1x pro Loop, dann update für jedes Licht

        // intensityFactor: 1.0 = normal, z.B. 0.6 fürs Rücklicht
        updateSineFlickerMul(_startFlicker, valueHead, 1.0f, 1050, 1600);
        updateSineFlickerMul(_startFlicker, valueRear, 0.7f, 1050, 1600);
        updateSineFlickerMul(_startFlicker, valuePos, 1.0f, 1050, 1600);
        updateSineFlickerMul(_startFlicker, valueReverse, 0.7f, 1050, 1600);
        updateSineFlickerMul(_startFlicker, valueWork, 1.0f, 1050, 1600);
    } else {
        // Wenn Start-Effekt vorbei: Zustand zurücksetzen, damit nächstes Mal wieder "von vorne" startet
        flickerReset(_startFlicker);
    }


    // -------- Light Request an Queue --------
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapHeadLight->gpioConfig),valueHead });
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapRearLight->gpioConfig),valueRear });  
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapPosLight->gpioConfig), valuePos });  
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapReverseLight->gpioConfig), valueReverse });  
    addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapWorkLight->gpioConfig),valueWork });  

    //Serial.println(valueHead);

}

// ---------------------------------------------------------
//  Methode für Blinken
// ---------------------------------------------------------
void CtrlSim::handleBlinkLights() {

    // -------- Warnblinken bei PWF Parken --------
    const bool hazardReq = (_statePWF == PWFStatus::Parken);

    // ### Achtung fix 3Pos ### 
    const int  leftChannel  = _rc.channel[int(_mapBlinkLeft->channelIndex)];
    const bool leftReq = (CTRL_3POS_2P_MIN < leftChannel && leftChannel < CTRL_3POS_2P_MAX);
    const bool rightReq  = (_rc.channel[int(_mapBlinkRight->channelIndex)] > CTRL_3POS_1P);

    // -------- Modus (Hazard hat Priorität) --------
    BlinkState blinkState = checkBlinkRequest(hazardReq, leftReq, rightReq); // Off/Left/Right/Hazard

    // -------- Mode-Queue Logik --------
    if (blinkState == BlinkState::Hazard) {
        _activeBlink = BlinkState::Hazard;
        _pendingBlink = BlinkState::Hazard;
    } else {
        if (_activeBlink == BlinkState::Off) {
            _activeBlink = blinkState;
            _pendingBlink = blinkState;
        } else {
            // -------- Aktuell läuft Blinkvorgang, umschalten erst nach Zyklus --------
            _pendingBlink = blinkState;
        }
    }
    if (_activeBlink == BlinkState::Off) {
        _blinkOn = false;
        return;
    }

    // -------- Blinktakt --------
    if (millis() - _lastBlinkMs > BLINK_TIME) {
        _lastBlinkMs = millis();
        _blinkOn = !_blinkOn;

        const uint16_t value = _blinkOn ? BLINK_BRIGHTNESS : 1000;

        // -------- Switch für Blinkstate --------
        switch (_activeBlink) {
            case BlinkState::Left:
                addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapBlinkLeft->gpioConfig), value });
                break;

            case BlinkState::Right:
                addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapBlinkRight->gpioConfig), value });
                break;

            case BlinkState::Hazard:
                addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapBlinkLeft->gpioConfig), value });
                addToGpioQueue(_gpioFrame, PinControl{ static_cast<uint8_t>(_mapBlinkRight->gpioConfig), value });
                //Serial.println(value);
                break;

            case BlinkState::Off:
            default:
                break;
        }

        // -------- Kompletter Blinkzyklus muss eingehalten werden --------
        if (!_blinkOn) { 
            if (_pendingBlink != _activeBlink) {
                _activeBlink = _pendingBlink;
            }
        }
    }
}

// ---------------------------------------------------------
//  Debug Schnittstelle. Gibt alle RC Sender Kanäle als Rohwerte aus
// ---------------------------------------------------------
void CtrlSim::debug()
{
    if (!_newControlData)        return;          // keine neuen Daten
    // if (_failsafeActive)  return;          // im Failsafe → nicht spammen
    if (_failsafeActive && (millis() - _lastDebugMs < 500)) return;  // nur alle 100ms

    // _lastDebugMs = _now;

    if (_newControlData && (millis() - _lastDebugMs > 500)){
        _lastDebugMs = millis();
        Serial.print("CTRLSIM: CRSF Channels: ");
        //Serial.print(_rc.timestampMs);
        for (int i = 0; i < CRSF_NUM_CHANNELS; i++) {
            Serial.print(_rc.channel[i]);
            Serial.print(i < (CRSF_NUM_CHANNELS - 1) ? ", " : "\n");
        }
        Serial.print("CTRLSIM: RCCom Failsafe   - ");
        //Serial.println(_rc.FailSafeRC);
        Serial.print("CTRLSIM: CtrlSim Failsafe   - "); 
        Serial.println(_failsafeActive);
    }
}

// ---------------------------------------------------------
//  GPIO Data an Queue q_Gpio übergeben
// ---------------------------------------------------------
void CtrlSim::publishGPIOQueue()
{
    if (q_Gpio != NULL) {
        // Neu geparste Channel Daten werden an die Queue übergeben
        //     
           //Serial.println("New GPIO Data");
            int dataCount = _gpioFrame.gpioCount; 
            if (dataCount > 0){
                xQueueOverwrite(q_Gpio, &_gpioFrame);
                // Serial.print("New GPIO Data:   ");
                // Serial.println(dataCount);

            }
            xQueueOverwrite(q_Gpio, &_gpioFrame);


            
            // frame.timestampMs = _lastPacketMs;
            // for (int i = 0; i < CRSF_CHANNEL; ++i) {
            //     frame.channel[i] = _rcChannel[i];
            // }
            // frame.FailSafeRC = _failsafeActive;
            // xQueueOverwrite(qRCCom, &frame);

            // _lastFailsafeSend = 0;
            //Serial.println("RCCOM: CRSF Data - New Data in Queue");               
        //}

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
    }
}

// ---------------------------------------------------------
//  PWF Status setzen
// ---------------------------------------------------------
void CtrlSim::setPWF(PWFStatus targetPWF)
{

    switch (targetPWF) {
        case PWFStatus::Parken:
            // Motor/ESC sperren

            _statePWF = PWFStatus::Parken;
            Serial.println("CTRLSIM: PWF Status zu Parken");  

            // Control Datenstruktur mit Failsafe vorbelegen
            // _cc.blinkenRightAct = true;
            break;

        case PWFStatus::Wohnen:
            // Kein Fahren, aber „Zündung an“
            

            _statePWF = PWFStatus::Wohnen;
            Serial.println("CTRLSIM: PWF Status zu Wohnen"); 
            break;

        case PWFStatus::Fahren:

            _statePWF = PWFStatus::Fahren;
            Serial.println("CTRLSIM: PWF Status zu Fahren"); 
            break;
    }
}

// ---------------------------------------------------------
//  Sucht im Config Array nach dem Index der Funktion
// ---------------------------------------------------------
int CtrlSim::findFirstMapIndex(FunctionList f) {
  for (size_t i = 0; i < MAP_COUNT; ++i) {
    if (cfg_rcGpioMap[i].functionList == f) return (int)i;
  }
  return -1;
}

// ---------------------------------------------------------
//  Priorisiert den Blink Request
// ---------------------------------------------------------
BlinkState CtrlSim::checkBlinkRequest(bool hazard, bool left, bool right)
{
    // Hazard hat Priorität
    if (hazard) return BlinkState::Hazard;

    if (left && !right) {
        _lastDir = BlinkDir::Left;
        return BlinkState::Left;
    }

    if (right && !left) {
        _lastDir = BlinkDir::Right;
        return BlinkState::Right;
    }

    // Beide gleichzeitig -> last wins
    if (left && right) {
        if (_lastDir == BlinkDir::Left)  return BlinkState::Left;
        if (_lastDir == BlinkDir::Right) return BlinkState::Right;
        return BlinkState::Off; // Fallback
    }

    return BlinkState::Off;
}



/*
 * File: config.h
 *
 * Description:
 *      Allgemeine Konfigurationsdatei
 *
 * Usage:
 *      Einbinden mit Header
 * 
 * Notes:
 *      
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/*
 *  Debug-Level (global) Define wenn serielle Debug Ausgaben aktiv sein soll 
 *  #ifdef TRACKED_DEBUG
 *      Serial.println("Debug aktiv");
 *  #endif
 */ 
#define DEBUG_Controller
#define DEBUG_LIGHT
#define DEBUG_SOUND
#define DEBUG_DRIVE
#define DEBUG_SOUND    

#define PWF_STATUS 

enum class PWFState : uint8_t {
    Parken  = 0,
    Wohnen  = 1,
    Fahren  = 2
};




#define LIGHT_HEADLIGHT
#define LIGHT_HEADLIGHT_BEAM
#define LIGHT_BLINK_RIGHT
#define LIGHT_BLINK_LEFT
#define LIGHT_POS_FRONT
#define LIGHT_REARLIGHT
#define LIGHT_BRAKE
#define LIGHT_REAR_WORK
#define LIGHT_FOG
#define LIGHT_CABINE
#define LIGHT_TACHO


#ifdef LIGHT_BRAKE
    u_int8_t lightBrakeBrightness = 200;
#endif

// Hardware-Pins
// #define LED_PIN   5
// #define BUTTON_PIN 13

// // Projekt-Konstanten
// #define BAUDRATE 115200
// #define USE_FEATURE_X 1

#endif
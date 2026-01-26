#pragma once
#include "vehicle/VehicleTypes.h"
#include "rc/RCModelTypes.h"
#include "board/BoardTypes.h"

// Fahrzeug auswählen (Physik des realen Fahrzeugs)
constexpr VehicleType SELECTED_VEHICLE = VehicleType::LR_SERIES3_109_225D;

// RC-Modell auswählen (dein konkreter Aufbau: Reifen, Masse, ESC Verhalten)
//constexpr RCModelType SELECTED_RC_MODEL = RCModelType::LR_SERIES3_RC_2300KV;

// Board auswählen (Pinmap/UART/Timer)
//constexpr BoardType SELECTED_BOARD = BoardType::ESP32DEV;

// globale Sim-Toggles
constexpr bool ENABLE_LOW_RANGE = false;     // später z.B. per Schalter
constexpr bool ENABLE_LOGGING   = true;
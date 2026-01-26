#pragma once
#include "vehicle/VehicleTypes.h"

/** ---------------------------------------------------------
 *      Daten von einem LandRover Series3 109" mit 2.25 Diesel (10J) 
 * 
 *      VehicleModel         -   Physikalische Fahrzeugdaten
 * 
 *      Bestehend aus:
 *      - EngineSpec        -   Motorcharakteristik
 *      - GearboxSpec       -   Schaltgetriebe
 *      - TransferCaseSpec  -   Verteilergetriebe (High / Low)
 *      - DiffSpec          -   Achsübersetzung
 *      - VehicleSpec       -   Fahrzeugmasse, Räder, Widerstände
 */

inline constexpr VehicleModel LR_SERIES3_109_225D = {         // Land Rover Series III 109" 2.25 Diesel – komplettes Fahrzeugmodell

  .engine = {                                                 // Motorparameter
    .idle_rpm = 850.0f,                                       // Leerlaufdrehzahl des Dieselmotors [U/min]
    .redline_rpm = 4000.0f,                                   // Drehzahlbegrenzer / maximale Nenndrehzahl [U/min]

    .rpm =       {  850,  1200,  1750,  2500,  3200,  4000 }, // Stützstellen der Drehmomentkurve [U/min]
    .torque_nm = {   60,   110,   140,   125,    95,    70 }, // Motordrehmoment an diesen Punkten [Nm] (Peak bei ~1750 rpm)

    .drag_k1_nm = 8.0f,                                       // Grund-Schleppmoment des Motors (innere Reibung, Kompression) [Nm]
    .drag_k2_nm = 35.0f,                                      // Drehzahlabhängiger Anteil der Motorbremse (zunehmend mit rpm)

    .limiter_soft_range = 400.0f                              // Bereich oberhalb der Redline, in dem der Soft-Limiter das Drehmoment abregelt [U/min]
  },

  .gearbox = {                                                // Schaltgetriebe (LT76 Series III)
  .num_gears = 4,                                             // Anzahl der Vorwärtsgänge
    .ratio = { 3.68f, 2.22f, 1.50f, 1.00f, 0, 0 },            // Übersetzungen von 1. bis 4. Gang (Untersetzung → Drehmomentverstärkung)
    .reverse_ratio = 4.01f,                                   // Übersetzung des Rückwärtsgangs
    .shift_cut_ms = 180.0f                                    // Dauer der Drehmomentunterbrechung beim Gangwechsel (Kupplung / Synchronisierung) [ms]
  },

  .transfer = {                                               // Verteilergetriebe (High / Low Range)
    .high_ratio = 1.148f,                                     // Straßenuntersetzung (High Range)
    .low_ratio  = 2.350f                                      // Geländegang-Untersetzung (Low Range)
  },
  
  .diff = { 
    .ratio = 4.7f                                             // Achsübersetzung (Differential) – verstärkt Drehmoment an den Rädern
  },

  .chassis = {                                                // Fahrzeug- und Fahrwerksdaten
    .mass_kg = 1451.0f,                                       // Fahrzeugmasse (Series III 109" Leergewicht) [kg]
    .wheel_radius_m = 0.4015f,                                // Effektiver Radradius (7.50-16 Reifen, Ø ~803 mm) [m]
    .Crr = 0.020f,                                            // Rollwiderstandskoeffizient (Reifen + Lager)
    .CdA = 1.50f                                              // Luftwiderstandsfläche (sehr kantige Karosserie)
  }
};

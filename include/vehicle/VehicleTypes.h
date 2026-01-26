#pragma once
#include <stdint.h>

/** ---------------------------------------------------------
 *      Daten der realen Fahrzeugfahrzeugmodelle
 * 
 *      VehicleType         -   Enum für die Auswahl des Fahrzeuges
 * 
 *      VehicleModel        -   Physikalische Fahrzeugdaten
 * 
 *      Bestehend aus:
 *      - EngineSpec        -   Motorcharakteristik
 *      - GearboxSpec       -   Schaltgetriebe
 *      - TransferCaseSpec  -   Verteilergetriebe (High / Low)
 *      - DiffSpec          -   Achsübersetzung
 *      - VehicleSpec       -   Fahrzeugmasse, Räder, Widerstände
 */



// -------- Auswahl der verfügbaren realen Fahrzeugmodelle --------
enum class VehicleType : uint8_t {
  LR_SERIES3_109_225D,   // Land Rover Series III, 109", 2.25L Diesel
  // später hinzufügen: DEFENDER_200TDI, UNIMOG_406, ...
};

// -------------------- Motor --------------------
// Beschreibt die dynamischen Eigenschaften des Verbrennungsmotors
// für die Längsdynamik-Simulation (Drehmoment, Drehzahl, Motorbremse, Begrenzer).
struct EngineSpec {

  // Leerlaufdrehzahl [U/min]
  // Unterhalb dieser Drehzahl wird der Motor in der Simulation nicht weiter abgebremst
  // (Kupplung getrennt / Leerlaufbetrieb).
  float idle_rpm;

  // Maximale zulässige Drehzahl [U/min]
  // Ab dieser Drehzahl greift der Drehzahlbegrenzer (Soft-Limiter).
  float redline_rpm;

  // Anzahl der Stützstellen der Drehmomentkennlinie
  static const int TORQUE_POINTS = 6;

  // Stützstellen der Drehzahlachse [U/min]
  // Aufsteigend sortiert, dienen zur Interpolation der Drehmomentkurve.
  float rpm[TORQUE_POINTS];

  // Motordrehmoment an den jeweiligen Stützstellen [Nm]
  // Beschreibt die Volllastkennlinie des Motors.
  float torque_nm[TORQUE_POINTS];

  // Grund-Schleppmoment des Motors [Nm]
  // Modelliert innere Reibung, Pumpverluste und Kompression bei Gas = 0.
  float drag_k1_nm;

  // Drehzahlabhängiger Anteil des Schleppmoments [Nm]
  // Bestimmt, wie stark die Motorbremse mit steigender Drehzahl zunimmt
  // (z. B. quadratischer Verlauf im Modell).
  float drag_k2_nm;

  // Bereich oberhalb der Redline, in dem der Soft-Limiter wirkt [U/min]
  // Innerhalb dieses Fensters wird das Drehmoment kontinuierlich auf Null reduziert
  // (Diesel-Governor-Verhalten statt harter Zündaussetzer).
  float limiter_soft_range;
};

// -------------------- Getriebe --------------------
// Beschreibt das Schaltgetriebe inkl. Rückwärtsgang
// und den zeitlichen Ablauf eines Schaltvorgangs.
struct GearboxSpec {

  // Anzahl der Vorwärtsgänge (z. B. 4 beim Series III)
  int num_gears;

  // Übersetzungsverhältnisse der Vorwärtsgänge
  // ratio[0] = 1. Gang, ratio[1] = 2. Gang, ...
  // Werte > 1 bedeuten Untersetzung (Drehmomentverstärkung).
  float ratio[6];

  // Übersetzungsverhältnis des Rückwärtsgangs
  float reverse_ratio;

  // Dauer der Drehmomentunterbrechung beim Gangwechsel [ms]
  // Simuliert Kupplung treten, Synchronisation und Einrücken des Gangs.
  float shift_cut_ms;
};

// -------------------- Verteilergetriebe --------------------
// High- und Low-Range-Untersetzung (z. B. Straße / Gelände).
struct TransferCaseSpec {

  // Übersetzungsverhältnis im High-Range (Straßenbetrieb)
  float high_ratio;

  // Übersetzungsverhältnis im Low-Range (Geländeuntersetzung)
  float low_ratio;
};

// -------------------- Differential --------------------
// Achsübersetzung zwischen Kardanwelle und Rädern.
struct DiffSpec {

  // Übersetzungsverhältnis des Differentials (z. B. 4.7:1)
  float ratio;
};

// -------------------- Fahrzeug / Fahrwerk --------------------
// Physikalische Eigenschaften des Gesamtfahrzeugs,
// die Trägheit und Widerstände bestimmen.
struct VehicleSpec {

  // Fahrzeugmasse [kg]
  // Bestimmt die Trägheit und damit Beschleunigung und Verzögerung.
  float mass_kg;

  // Effektiver Radradius [m]
  // Für Umrechnung zwischen Fahrzeuggeschwindigkeit und Raddrehzahl.
  float wheel_radius_m;

  // Rollwiderstandskoeffizient [-]
  // Modelliert Reifen- und Lagerverluste (typisch 0.01 ... 0.03).
  float Crr;

  // Luftwiderstandsfläche Cd * A [m²]
  // Fasst Luftwiderstandsbeiwert und Stirnfläche zusammen.
  float CdA;
};

// -------------------- Komplettes Fahrzeugmodell --------------------
// Bündelt alle physikalischen Daten eines realen Fahrzeugs
// von Motor über Antriebsstrang bis zu Masse und Rädern.
struct VehicleModel {

  // Motorcharakteristik
  EngineSpec engine;

  // Schaltgetriebe
  GearboxSpec gearbox;

  // Verteilergetriebe (High / Low)
  TransferCaseSpec transfer;

  // Achsübersetzung
  DiffSpec diff;

  // Fahrzeugmasse, Räder, Widerstände
  VehicleSpec chassis;
};

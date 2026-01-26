#include "vehicle/VehicleRegistry.h"

/** ---------------------------------------------------------
 *      Getter für das ausgewählte Fahrzeugmodell vom realen Fahrzeug 
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

// -------- *** Weitere Modelle einbinden *** --------
#include "vehicle/models/LR_SERIES3_109_225D.h"

const VehicleModel& getVehicleModel(VehicleType type)
{
  switch (type) {
    case VehicleType::LR_SERIES3_109_225D:
      return LR_SERIES3_109_225D;

    default:
      return LR_SERIES3_109_225D; // Abändern sobald weitere Modelle
  }
}
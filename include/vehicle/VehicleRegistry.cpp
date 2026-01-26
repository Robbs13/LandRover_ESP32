#include "VehicleRegistry.h"

// Modelle einbinden
#include "models/LR_SERIES3_109_225D.h"

const VehicleModel& getVehicleModel(VehicleType type)
{
  switch (type) {
    case VehicleType::LR_SERIES3_109_225D:
    default:
      return LR_SERIES3_109_225D;
  }
}
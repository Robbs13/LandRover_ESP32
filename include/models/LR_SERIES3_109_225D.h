#pragma once
#include "vehicle/VehicleTypes.h"

// Land Rover Series III 109" – 2.25 Diesel (10J) – Startwerte
inline constexpr VehicleModel LR_SERIES3_109_225D = {
  .engine = {
    .idle_rpm = 850.0f,
    .redline_rpm = 4000.0f,
    .rpm =       {  850,  1200,  1750,  2500,  3200,  4000 },
    .torque_nm = {   60,   110,   140,   125,    95,    70 },
    .drag_k1_nm = 8.0f,
    .drag_k2_nm = 35.0f,
    .limiter_soft_range = 400.0f
  },

  .gearbox = {
    .num_gears = 4,
    .ratio = { 3.68f, 2.22f, 1.50f, 1.00f, 0, 0 },
    .reverse_ratio = 4.01f,
    .shift_cut_ms = 180.0f
  },

  .transfer = { .high_ratio = 1.148f, .low_ratio = 2.350f },
  .diff = { .ratio = 4.7f },

  .chassis = {
    .mass_kg = 1451.0f,
    .wheel_radius_m = 0.4015f,
    .Crr = 0.020f,
    .CdA = 1.50f
  }
};
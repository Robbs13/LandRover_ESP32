#pragma once
#include <stdint.h>

// Fahrzeug-Auswahl
enum class VehicleType : uint8_t {
  LR_SERIES3_109_225D,
  // später: DEFENDER_200TDI, ...
};

// -------- Engine --------
struct EngineSpec {
  float idle_rpm;                 // [rpm]
  float redline_rpm;              // [rpm]
  static const int TORQUE_POINTS = 6;
  float rpm[TORQUE_POINTS];       // [rpm] ascending
  float torque_nm[TORQUE_POINTS]; // [Nm]
  float drag_k1_nm;               // [Nm] base drag
  float drag_k2_nm;               // [Nm] rpm-dependent (your model)
  float limiter_soft_range;       // [rpm]
};

// -------- Drivetrain --------
struct GearboxSpec {
  int   num_gears;
  float ratio[6];                 // ratio[0]=1st...
  float reverse_ratio;
  float shift_cut_ms;             // [ms]
};

struct TransferCaseSpec {
  float high_ratio;
  float low_ratio;
};

struct DiffSpec {
  float ratio;
};

// -------- Vehicle / chassis --------
struct VehicleSpec {
  float mass_kg;
  float wheel_radius_m;
  float Crr;
  float CdA;
};

// Complete bundle
struct VehicleModel {
  EngineSpec engine;
  GearboxSpec gearbox;
  TransferCaseSpec transfer;
  DiffSpec diff;
  VehicleSpec chassis;
};
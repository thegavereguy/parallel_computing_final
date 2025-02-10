struct Conditions {
  const double L;        // Length of the rod
  const double alpha;    // Thermal diffusivity
  const double t_final;  // Final time
  const int n_x;         // Number of spatial points
  const int n_t;         // Number of time steps
};
void initialize_array(float* array, int size);

const float expected[16]{100,       83.93145,  68.75215,  55.306667,
                         44.362194, 36.59398,  32.58662,  32.84042,
                         37.768017, 47.669025, 62.678932, 82.70068,
                         107.33814, 135.85602, 167.18753, 200};

// long version
//  const Conditions test_cases[16] = {
//      {1.0, 0.01, 1.0, 256, 1000},         // Smallest grid, lowest time steps
//      {1.0, 0.02, 1.5, 1024, 5000},        // 4x increase in spatial, 5x in
//      time {2.0, 0.03, 2.0, 4096, 10000},       // 4x spatial increase {1.5,
//      0.04, 2.5, 16384, 20000},      // 4x spatial increase {1.0, 0.01, 3.0,
//      65536, 40000},      // 4x spatial increase {1.5, 0.02, 3.5, 131072,
//      60000},     // 2x spatial increase {2.0, 0.03, 4.0, 262144, 80000}, //
//      2x spatial increase {2.0, 0.04, 4.5, 524288, 100000},    // 2x spatial
//      increase {1.0, 0.01, 5.0, 1048576, 120000},   // 2x spatial increase
//      {1.5, 0.02, 5.5, 2097152, 140000},   // 2x spatial increase
//      {2.0, 0.03, 6.0, 4194304, 160000},   // 2x spatial increase
//      {2.0, 0.04, 6.5, 8388608, 180000},   // 2x spatial increase
//      {1.0, 0.01, 7.0, 12582912, 200000},  // 1.5x spatial increase
//      {1.5, 0.02, 7.5, 16777216, 220000},  // 1.33x spatial increase
//      {2.0, 0.03, 8.0, 16777216, 250000},  // Keeping max spatial, longer time
//      {2.0, 0.04, 8.5, 16777216, 300000}   // Ultimate test case
//  };

// short version
const Conditions test_cases[4] = {
    {1.0, 0.01, 1.0, 256, 1000},     // Smallest grid, lowest time steps
    {1.0, 0.02, 1.5, 1024, 5000},    // 4x increase in spatial, 5x in time
    {2.0, 0.03, 2.0, 4096, 10000},   // 4x spatial increase
    {1.5, 0.04, 2.5, 16384, 20000},  // 4x spatial increase
};

struct Conditions {
  const double L;        // Length of the rod
  const double alpha;    // Thermal diffusivity
  const double t_final;  // Final time
  const int n_x;         // Number of spatial points
  const int n_t;         // Number of time steps
};
void initialize_array(float* array, int size);

const Conditions test_cases[8] = {
    {1.0, 0.01, 1.0, 256, 1000},      // Smallest grid, lowest time steps
    {1.0, 0.02, 2.0, 1024, 10000},    // Moderate resolution
    {2.0, 0.03, 2.5, 8192, 25000},    // Increasing spatial/temporal complexity
    {1.5, 0.04, 3.0, 32768, 35000},   // Larger domain, high diffusivity
    {1.0, 0.01, 1.0, 65536, 40000},   // Large spatial resolution, moderate time
    {1.5, 0.03, 2.5, 262144, 60000},  // Large problem size, high stability
    {2.0, 0.04, 3.0, 524288, 80000},  // High-end test case
    {2.0, 0.04, 3.0, 524288, 100000}  // Maximum complexity
};

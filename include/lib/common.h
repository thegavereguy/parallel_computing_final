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
// const Conditions test_cases[16] = {
//     {1.0, 0.01, 1.0, 256, 1000},        {1.0, 0.02, 1.5, 1024, 5000},
//     {2.0, 0.03, 2.0, 4096, 10000},      {1.5, 0.04, 2.5, 16384, 20000},
//     {1.0, 0.01, 3.0, 65536, 40000},     {1.5, 0.02, 3.5, 131072, 60000},
//     {2.0, 0.03, 4.0, 262144, 80000},    {2.0, 0.04, 4.5, 524288, 100000},
//     {1.0, 0.01, 5.0, 1048576, 120000},  {1.5, 0.02, 5.5, 2097152, 140000},
//     {2.0, 0.03, 6.0, 4194304, 160000},  {2.0, 0.04, 6.5, 8388608, 180000},
//     {1.0, 0.01, 7.0, 12582912, 200000}, {1.5, 0.02, 7.5, 16777216, 220000},
//     {2.0, 0.03, 8.0, 16777216, 250000}, {2.0, 0.04, 8.5, 16777216, 300000}};

// hard version
// const Conditions test_cases[12] = {
//     {1.0, 0.001, 1.0, 256, 2000},        {1.0, 0.0015, 1.25, 1024, 8000},
//     {1.0, 0.002, 1.5, 4096, 32000},      {1.5, 0.0025, 1.75, 16384, 100000},
//     {2.0, 0.003, 2.0, 65536, 250000},    {1.5, 0.0035, 2.25, 262144, 500000},
//     {1.5, 0.004, 2.5, 524288, 750000},   {1.25, 0.0045, 2.75, 1048576,
//     1000000}, {1.0, 0.001, 3.0, 2097152, 1250000}, {1.25, 0.0015, 3.25,
//     4194304, 1500000}, {1.5, 0.002, 3.5, 8388608, 1750000}, {2.0,
//     0.0025, 3.75, 16777216, 2000000},
// };

// easy version
// const Conditions test_cases[12] = {
//     {1.0, 0.001, 1.0, 256, 2000},       {1.0, 0.0015, 1.25, 512, 65000},
//     {1.0, 0.002, 1.5, 1024, 130000},    {1.0, 0.0025, 1.75, 2048, 195000},
//     {1.25, 0.003, 2.0, 4096, 260000},   {1.25, 0.0035, 2.25, 8192, 325000},
//     {1.25, 0.004, 2.25, 16384, 390000}, {1.5, 0.0045, 2.5, 32768, 455000},
//     {1.5, 0.004, 2.5, 65536, 520000},   {1.5, 0.0035, 2.5, 131072, 585000},
//     {1.5, 0.003, 2.5, 262144, 650000},  {1.5, 0.004, 2.5, 524288, 750000},
// };
//
// easier version
const Conditions test_cases[8] = {
    {1.0, 0.001, 1.0, 256, 2000},       {1.0, 0.0015, 1.25, 512, 65000},
    {1.0, 0.002, 1.5, 1024, 130000},    {1.0, 0.0025, 1.75, 2048, 195000},
    {1.25, 0.003, 2.0, 4096, 260000},   {1.25, 0.0035, 2.25, 8192, 325000},
    {1.25, 0.004, 2.25, 16384, 390000}, {1.5, 0.0045, 2.5, 32768, 455000},
};

const Conditions space_focused_cases[12] = {
    {1.0, 0.001, 1.0, 1024, 100000},     {1.0, 0.001, 1.0, 4096, 100000},
    {1.0, 0.001, 1.5, 16384, 150000},    {1.0, 0.001, 1.5, 65536, 150000},
    {1.25, 0.001, 2.0, 262144, 200000},  {1.25, 0.001, 2.0, 524288, 200000},
    {1.25, 0.001, 2.5, 1048576, 250000}, {1.25, 0.001, 2.5, 2097152, 250000},
    // {1.5, 0.001, 3.0, 4194304, 300000},  {1.5, 0.001, 3.0, 8388608, 300000},
    // {1.5, 0.001, 3.5, 16777216, 350000}, {1.5, 0.001, 3.5, 33554432, 350000},
};
const Conditions time_focused_cases[12] = {
    {1.0, 0.001, 1.0, 1024, 2000},     {1.0, 0.001, 1.0, 1024, 50000},
    {1.0, 0.001, 1.5, 2048, 100000},   {1.0, 0.001, 1.5, 2048, 250000},
    {1.25, 0.001, 2.0, 4096, 500000},  {1.25, 0.001, 2.0, 4096, 750000},
    {1.25, 0.001, 2.5, 8192, 1000000}, {1.25, 0.001, 2.5, 8192, 1250000},
    // {1.5, 0.001, 3.0, 16384, 1500000}, {1.5, 0.001, 3.0, 16384, 1750000},
    // {1.5, 0.001, 3.5, 32768, 1850000}, {1.5, 0.001, 3.5, 32768, 2000000},
};

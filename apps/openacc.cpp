#include <openacc.h>  // OpenACC header for GPU acceleration

#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

#define NX 16        // Number of spatial grid points
#define NT 30        // Number of time steps
#define ALPHA 0.5    // Diffusion coefficient
#define DX 1.0 / NX  // Grid spacing
#define DT 0.1 / NT  // Time step size

void heat_equation(double* u, double* u_new, int nx, int nt, double alpha,
                   double dx, double dt) {
  double lambda = alpha * dt / (dx * dx);

// Transfer data to GPU
#pragma acc data copy(u[0 : nx], u_new[0 : nx])
  {
    for (int t = 0; t < nt; t++) {
// Parallel computation using OpenACC
#pragma acc parallel loop
      for (int i = 1; i < nx - 1; i++) {
        u_new[i] = u[i] + lambda * (u[i - 1] - 2.0 * u[i] + u[i + 1]);
      }

      // Swap pointers (efficient memory management)
      std::swap(u, u_new);
    }
  }
}

int main() {
  double* u_new = new double[NX];
  double* u     = new double[NX];

  // Initial condition: Step function
  for (int i = 0; i < NX; i++) {
    u[i] = 0.0;
  }
  u[0]      = 100;
  u[NX - 1] = 200;

  // double start_time = acc_get_wtime();
  heat_equation(u, u_new, NX, NT, ALPHA, DX, DT);
  // double end_time = acc_get_wtime();

  // Print execution time
  // std::cout << "Execution Time: " << (end_time - start_time) << " seconds"
  // << std::endl;

  // Print final values for validation
  for (int i = 0; i < NX; i += NX / 10) {
    std::cout << u[i] << " ";
  }
  std::cout << std::endl;

  return 0;
}

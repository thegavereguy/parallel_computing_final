#include <fmt/base.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include <utility>

const double L       = 1.0;  // Length of the rod
const double alpha   = 0.5;  // Thermal diffusivity
const double t_final = 0.1;  // Final time
const int n_x        = 16;   // Number of spatial points
const int n_t        = 30;   // Number of time steps

void heat_equation(double *u, double *u_new, int local_nx, double alpha,
                   int rank, int size) {
  float dt = t_final / (n_t - 1);
  float dx = L / (n_x - 1);

  MPI_Request request[4];  // Non-blocking send/receive requests

  for (int t = 0; t < n_t; t++) {
    // Non-blocking communication: exchange ghost cells
    if (rank > 0) {
      MPI_Isend(&u[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD,
                &request[0]);  // Send left boundary
      MPI_Irecv(&u[0], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD,
                &request[1]);  // Receive left neighbor
    }
    if (rank < size - 1) {
      MPI_Isend(&u[local_nx], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD,
                &request[2]);  // Send right boundary
      MPI_Irecv(&u[local_nx + 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD,
                &request[3]);  // Receive right neighbor
    }

    fmt::print("rank = {}\n", rank);
    // Compute internal grid points (excluding ghost cells)
    for (int i = 2; i < local_nx; i++) {
      u_new[i] =
          u[i] + alpha * (u[i - 1] - 2.0 * u[i] + u[i + 1]) * (dt / (dx * dx));
      fmt::print("{} ", u_new[i]);
    }
    fmt::print("\n");

    // Wait for communication to complete
    if (rank > 0) MPI_Waitall(2, request, MPI_STATUSES_IGNORE);
    if (rank < size - 1) MPI_Waitall(2, &request[2], MPI_STATUSES_IGNORE);

    // Compute boundary points after receiving ghost cells
    u_new[1] = u[1] + alpha * (u[0] - 2.0 * u[1] + u[2]) * (dt / (dx * dx));
    u_new[local_nx] =
        u[local_nx] +
        alpha * (u[local_nx - 1] - 2.0 * u[local_nx] + u[local_nx + 1]) *
            (dt / (dx * dx));

    if (rank == 0) {
      u_new[1] = u[1];
    } else if (rank == size - 1) {
      u_new[local_nx] = u[local_nx];
    }
    // Update u
    for (int i = 1; i <= local_nx; i++) {
      u[i] = u_new[i];
    }

    // Swap pointers
    std::swap(u, u_new);
  }
}

int main(int argc, char **argv) {
  int rank, size;
  float dt = t_final / (n_t - 1);
  float dx = L / (n_x - 1);

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Local domain size per process (including ghost cells)
  int local_nx = n_x / size;
  if (rank == size - 1)
    local_nx +=
        n_x % size;  // Last process gets extra points if NX is not divisible

  // Allocate arrays (with extra ghost cells)
  double *u     = (double *)malloc((local_nx + 2) * sizeof(double));
  double *u_new = (double *)malloc((local_nx + 2) * sizeof(double));

  // Initialize local grid
  for (int i = 1; i <= local_nx; i++) {
    int global_i =
        rank * (n_x / size) + i;  // Convert local index to global index
    if (global_i == 1) {
      u[i] = 100.0;
    } else if (global_i == n_x) {
      u[i] = 200.0;
    } else {
      u[i] = 0.0;
    }
  }

  double start_time = MPI_Wtime();

  heat_equation(u, u_new, local_nx, alpha, rank, size);

  double end_time = MPI_Wtime();

  // Gather results to rank 0
  if (rank == 0) {
    double *global_u = (double *)malloc(n_x * sizeof(double));
    MPI_Gather(&u[1], local_nx, MPI_DOUBLE, global_u, local_nx, MPI_DOUBLE, 0,
               MPI_COMM_WORLD);
    for (int i = 0; i < n_x; i++) {
      printf("%f ", global_u[i]);
    }
    printf("Execution Time: %f seconds\n", end_time - start_time);
    free(global_u);
  } else {
    MPI_Gather(&u[1], local_nx, MPI_DOUBLE, NULL, local_nx, MPI_DOUBLE, 0,
               MPI_COMM_WORLD);
  }

  free(u);
  free(u_new);
  MPI_Finalize();
  return 0;
}

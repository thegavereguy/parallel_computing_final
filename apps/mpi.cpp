#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define NX 16        // Total number of grid points
#define NT 30        // Number of time steps
#define ALPHA 0.5    // Diffusion coefficient
#define DX 1.0 / NX  // Grid spacing
#define DT 0.1 / NT  // Time step size

void heat_equation(double *u, double *u_new, int local_nx, double lambda,
                   int rank, int size) {
  MPI_Request request[4];  // Non-blocking send/receive requests

  for (int t = 0; t < NT; t++) {
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

    // Compute internal grid points (excluding ghost cells)
    for (int i = 2; i < local_nx; i++) {
      u_new[i] = u[i] + lambda * (u[i - 1] - 2.0 * u[i] + u[i + 1]);
    }

    if (rank > 0) MPI_Waitall(2, request, MPI_STATUSES_IGNORE);
    if (rank < size - 1) MPI_Waitall(2, &request[2], MPI_STATUSES_IGNORE);

    // Compute boundary points after receiving ghost cells
    u_new[1] = u[1] + lambda * (u[0] - 2.0 * u[1] + u[2]);
    u_new[local_nx] =
        u[local_nx] +
        lambda * (u[local_nx - 1] - 2.0 * u[local_nx] + u[local_nx + 1]);

    // Swap pointers
    double *temp = u;
    u            = u_new;
    u_new        = temp;
  }
}

int main(int argc, char **argv) {
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Local domain size per process (including ghost cells)
  int local_nx = NX / size;
  if (rank == size - 1)
    local_nx +=
        NX % size;  // Last process gets extra points if NX is not divisible

  // Allocate arrays (with extra ghost cells)
  double *u     = (double *)malloc((local_nx + 2) * sizeof(double));
  double *u_new = (double *)malloc((local_nx + 2) * sizeof(double));

  // Initialize local grid
  for (int i = 1; i <= local_nx; i++) {
    int global_i =
        rank * (NX / size) + i;  // Convert local index to global index
    u[i] = 0;
  }
  u[0]            = 100;
  u[local_nx - 1] = 200;

  double lambda     = ALPHA * DT / (DX * DX);
  double start_time = MPI_Wtime();

  heat_equation(u, u_new, local_nx, lambda, rank, size);

  double end_time = MPI_Wtime();

  // Gather results to rank 0
  if (rank == 0) {
    double *global_u = (double *)malloc(NX * sizeof(double));
    MPI_Gather(&u[1], local_nx, MPI_DOUBLE, global_u, local_nx, MPI_DOUBLE, 0,
               MPI_COMM_WORLD);
    for (int i = 0; i < NX; i++) {
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

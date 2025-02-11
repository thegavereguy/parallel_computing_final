#include <fmt/base.h>
#include <lib/common.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <utility>

void heat_equation(float *input, float *output, int local_nx, int rank,
                   int size, float L, float alpha, float t_final, float n_x,
                   float n_t) {
  float dt = t_final / (n_t - 1);
  float dx = L / (n_x - 1);

  MPI_Request request[4];  // Non-blocking send/receive requests

  for (int t = 0; t < n_t; t++) {
    // Non-blocking communication: exchange ghost cells
    if (rank > 0) {
      MPI_Isend(&input[1], 1, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD,
                &request[0]);  // Send left boundary
      MPI_Irecv(&input[0], 1, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD,
                &request[1]);  // Receive left neighbor
    }
    if (rank < size - 1) {
      MPI_Isend(&input[local_nx], 1, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD,
                &request[2]);  // Send right boundary
      MPI_Irecv(&input[local_nx + 1], 1, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD,
                &request[3]);  // Receive right neighbor
    }

    // Compute internal grid points (excluding ghost cells)
    for (int i = 2; i < local_nx; i++) {
      output[i] =
          input[i] + alpha * (input[i - 1] - 2.0 * input[i] + input[i + 1]) *
                         (dt / (dx * dx));
    }

    // Wait for communication to complete
    if (rank > 0) MPI_Waitall(2, request, MPI_STATUSES_IGNORE);
    if (rank < size - 1) MPI_Waitall(2, &request[2], MPI_STATUSES_IGNORE);

    // Compute boundary points after receiving ghost cells
    output[1] = input[1] + alpha * (input[0] - 2.0 * input[1] + input[2]) *
                               (dt / (dx * dx));
    output[local_nx] =
        input[local_nx] + alpha *
                              (input[local_nx - 1] - 2.0 * input[local_nx] +
                               input[local_nx + 1]) *
                              (dt / (dx * dx));

    if (rank == 0) {
      output[1] = input[1];
    } else if (rank == size - 1) {
      output[local_nx] = input[local_nx];
    }
    // Update u
    for (int i = 1; i <= local_nx; i++) {
      input[i] = output[i];
    }
    // Swap pointers
    std::swap(input, output);
  }
}
void run(int argc, char **argv, float L, float alpha, float t_final, int n_x,
         int n_t) {
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
  float *input  = new float[local_nx + 2];
  float *output = new float[local_nx + 2];

  // Initialize local grid
  for (int i = 1; i <= local_nx; i++) {
    int global_i =
        rank * (n_x / size) + i;  // Convert local index to global index
    if (global_i == 1) {
      input[i] = 100.0;
    } else if (global_i == n_x) {
      input[i] = 200.0;
    } else {
      input[i] = 0.0;
    }
  }

  double start_time = MPI_Wtime();

  heat_equation(input, output, local_nx, rank, size, L, alpha, t_final, n_x,
                n_t);

  double end_time = MPI_Wtime();

  // Gather results to rank 0
  if (rank == 0) {
    float *res = new float[local_nx + 2];
    MPI_Gather(&input[1], local_nx, MPI_FLOAT, res, local_nx, MPI_FLOAT, 0,
               MPI_COMM_WORLD);
    // printf("Time: %f seconds\n", (end_time - start_time) * 1000);
    float millis = (end_time - start_time) * 1000;
    fmt::print("{},{},{},{},{},{}\n", n_x, n_t, millis, millis, millis, 1);
    delete[] res;
  } else {
    MPI_Gather(&input[1], local_nx, MPI_FLOAT, NULL, local_nx, MPI_FLOAT, 0,
               MPI_COMM_WORLD);
  }

  delete[] input;
  delete[] output;
  MPI_Finalize();
}

int main(int argc, char **argv) {
  const float L       = std::stof(argv[1]);  // Length of the rod
  const float alpha   = std::stof(argv[2]);  // Thermal diffusivity
  const float t_final = std::stof(argv[3]);  // Final time
  const int n_x       = std::stoi(argv[4]);  // Number of spatial points
  const int n_t       = std::stoi(argv[5]);  // Number of time steps

  // for (Conditions condition : test_cases) {
  //   fmt::print("L: {}, alpha: {}, t_final: {}, n_x: {}, n_t: {}\n",
  //   condition.L,
  //              condition.alpha, condition.t_final, condition.n_x,
  //              condition.n_t);
  //   run(argc, argv, condition.L, condition.alpha, condition.t_final,
  //       condition.n_x, condition.n_t);
  // }
  run(argc, argv, L, alpha, t_final, n_x, n_t);

  return 0;
}

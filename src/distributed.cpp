#include <fmt/base.h>
#include <lib/distributed.h>
#include <mpi.h>

#include <utility>

void heat_equation(float *input, float *output, int local_nx, int rank,
                   int size, Conditions conditions) {
  float dt = conditions.t_final / (conditions.n_t - 1);
  float dx = conditions.L / (conditions.n_x - 1);

  MPI_Request request[4];  // Non-blocking send/receive requests

  for (int t = 0; t < conditions.n_t; t++) {
    // Non-blocking communication: exchange ghost cells
    if (rank > 0) {
      MPI_Isend(&input[1], 1, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD,
                &request[0]);  // Send left boundary
      MPI_Irecv(&input[0], 1, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD,
                &request[1]);  // Receive left neighbor
    }
    if (rank < conditions.n_x - 1) {
      MPI_Isend(&input[local_nx], 1, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD,
                &request[2]);  // Send right boundary
      MPI_Irecv(&input[local_nx + 1], 1, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD,
                &request[3]);  // Receive right neighbor
    }

    fmt::print("rank = {}\n", rank);
    // Compute internal grid points (excluding ghost cells)
    for (int i = 2; i < local_nx; i++) {
      output[i] =
          input[i] + conditions.alpha *
                         (input[i - 1] - 2.0 * input[i] + input[i + 1]) *
                         (dt / (dx * dx));
      fmt::print("{} ", output[i]);
    }
    fmt::print("\n");

    // Wait for communication to complete
    if (rank > 0) MPI_Waitall(2, request, MPI_STATUSES_IGNORE);
    if (rank < size - 1) MPI_Waitall(2, &request[2], MPI_STATUSES_IGNORE);

    // Compute boundary points after receiving ghost cells
    output[1] = input[1] + conditions.alpha *
                               (input[0] - 2.0 * input[1] + input[2]) *
                               (dt / (dx * dx));
    output[local_nx] =
        input[local_nx] + conditions.alpha *
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

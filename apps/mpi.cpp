#include <mpi.h>

#include <iostream>

int main(int argc, char** argv) {
  // Initialize MPI environment
  MPI_Init(&argc, &argv);

  // Get the number of processes and rank of the current process
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Get the processor name
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  // Simple point-to-point communication example
  if (world_size < 2) {
    std::cerr << "This program requires at least 2 processes\n";
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  const int MAX_NUMBERS = 100;
  int numbers[MAX_NUMBERS];

  if (world_rank == 0) {
    // Sender process
    for (int i = 0; i < MAX_NUMBERS; i++) {
      numbers[i] = i;
    }
    std::cout << "Process 0 is sending data to Process 1\n";
    MPI_Send(numbers, MAX_NUMBERS, MPI_INT, 1, 0, MPI_COMM_WORLD);
  } else if (world_rank == 1) {
    // Receiver process
    MPI_Recv(numbers, MAX_NUMBERS, MPI_INT, 0, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    std::cout << "Process 1 received data. First number: " << numbers[0]
              << ", Last number: " << numbers[MAX_NUMBERS - 1] << std::endl;
  }

  // Print basic information from all processes
  std::cout << "Process " << world_rank << " out of " << world_size
            << " on processor " << processor_name << std::endl;

  // Finalize MPI environment
  MPI_Finalize();
  return 0;
}

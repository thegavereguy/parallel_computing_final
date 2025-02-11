#include <lib/common.h>
#include <mpi.h>

void heat_equation(float *input, float *output, int local_nx, int rank,
                   int size, Conditions);
int run(int, char **);

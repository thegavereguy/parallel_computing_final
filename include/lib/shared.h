#include <lib/common.h>
void sequential(Conditions, float*, float*);
void parallel2_inner(Conditions, float*, float*);
void parallel4_inner(Conditions, float*, float*);
void parallel8_inner(Conditions, float*, float*);

void parallel2_outer(Conditions, float*, float*);

void sequential_unroll(Conditions, float*, float*);

void parallel2_collapse(Conditions, float*, float*);

void prototype(Conditions, float*, float*);

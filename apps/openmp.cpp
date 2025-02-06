#include <lib/test.h>
#include <omp.h>

#include <iostream>

int main() {
#pragma omp parallel num_threads(4)
  {
    std::cout << "Hello, world!" << std::endl;
  }

  test();
  return 0;
}

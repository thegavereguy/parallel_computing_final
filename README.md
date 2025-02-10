# Fundamentals of Parallel Programming - Final Project

## Parallelized Solution of 1D Heat Equation


## Index

## Getting started

### Dependencies

- CMake >= 3.20
- Vulkan API >= 1.3

### Cloning repository
```sh
git clone --recursive https://github.com/thgavereguy/parallel_computing_final
```

### Building 

The project has 4 main parts, 3 of which can be skipped during compilation.

#### Parts
- OpenMP implementation '(required)'.
- Vulkan implementation (can be included by setting the ENV variable )
    - 'export BUILD_VULKAN=true'
- MPI implementation (can be included by setting the ENV variable )
    - 'export BUILD_MPI=true'
- OpenAcc implementation (can be included by setting the ENV variable )
    - 'export BUILD_ACC=true'

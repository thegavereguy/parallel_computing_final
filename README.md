# Fundamentals of Parallel Programming - Final Project

## Parallelized Solution of 1D Heat Equation
## Index

<!--toc:start-->
- [Fundamentals of Parallel Programming - Final Project](#fundamentals-of-parallel-programming-final-project)
  - [Parallelized Solution of 1D Heat Equation](#parallelized-solution-of-1d-heat-equation)
  - [Index](#index)
  - [Project structure](#project-structure)
  - [Important notes](#important-notes)
  - [Getting started](#getting-started)
    - [Dependencies](#dependencies)
    - [Cloning repository](#cloning-repository)
    - [Building](#building)
      - [Parts](#parts)
      - [Optimization](#optimization)
      - [Manual building](#manual-building)
      - [Automatic building and test running](#automatic-building-and-test-running)
<!--toc:end-->

## Project structure

- `apps`
    - Source code for test executables and the MPI solution
- `framework`
    - Source code for the Vulkan framework
- `include/lib`
    - Header files of the libraries
- `scripts`
    - Bash and python scripts to build, run benchmarks and visualize results
- `shaders`
    - Source code for GLSL shaders
- `src`
    - Source code of the libraries
- `tests`
    - Source code for tests and benchmarks
- `third-party`
    - Git submodules of dependencies

## Important notes
Due to an old GLIBC version, the Vulkan implementation cannot be compiled on the `cluster`, as the compilation of one of the dependencies fails.
Please make sure to export `BUILD_VULKAN=false`.
A test implementation using OpenACC was added but it's not working correctly. 
If compiling manually on the `cluster`, make sure to use the absolute path for the most recent version of CMake (`/apps/cmake-3.20.3/bin/cmake`), as the on provided by the module 'cmake-3.15.2' causes build problems.

## Getting started
### Dependencies

- CMake >= 3.20
- Vulkan API >= 1.3 (if enabled)

### Cloning repository
```sh
git clone --recursive https://github.com/thgavereguy/parallel_computing_final
cd parallel_computing_final
```

### Building 

The project has 4 main parts, 3 of which can be skipped during compilation. Enabling the building of each part also toggles the running of the benchmarks and relative data collection when running the 'assignment.sh' script.

#### Parts
- OpenMP implementation '(required)'.
- Vulkan implementation (can be included by setting the ENV variable )
    - `export BUILD_VULKAN=true`
- MPI implementation (can be included by setting the ENV variable )
    - `export BUILD_MPI=true`
- OpenAcc implementation (can be included by setting the ENV variable )
    - `export BUILD_ACC=true`
#### Optimization

Compiler optimization can be toggled using:
```
export BUILD_OPTMIZED=true|false
```

#### Manual building

After selecting the required parts we can manually compile the project:


```sh
mkdir build
cd build
cmake .. # of /apps/cmake-3.20.3/bin/cmake on the cluster
make 
```

#### Automatic building and test running

The build process, as well as the data collection process, can be executed by running `./scripts/assingment.sh`.
The various build settings can be changes modifying the exported variable at the top of the file. Previously exported variables are overwritten.

The results are saved into a `results` folder.


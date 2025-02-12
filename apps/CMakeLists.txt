
if(DEFINED ENV{BUILD_VULKAN} AND "$ENV{BUILD_VULKAN}" STREQUAL "true")
	add_executable(vk vk.cpp)
	target_link_libraries(vk PUBLIC framework)
	target_link_libraries(vk PUBLIC fmt)
endif()

# custom target for OpenMP
add_executable(openmp openmp.cpp)
target_link_libraries(openmp PUBLIC  fmt shared)
#target_compile_options(openmp PUBLIC -fopenmp)


if(DEFINED ENV{BUILD_MPI} AND "$ENV{BUILD_MPI}" STREQUAL "true")
	message("building MPI solution")

	find_package(MPI REQUIRED)
	add_executable(mpi mpi.cpp)
	target_link_libraries(mpi PUBLIC  fmt MPI::MPI_CXX common)
	target_include_directories(mpi PUBLIC ${MPI_CXX_INCLUDE_PATH})

if(DEFINED ENV{BUILD_OPTIMIZED} AND "$ENV{BUILD_OPTIMIZED}" STREQUAL "true")
		message("Building optimized version of MPI")
	target_compile_options(mpi PUBLIC -O3)
else()
	target_compile_options(mpi PUBLIC -O0)
		message("Building non-optimized version of MPI")
endif()

# custom target to run mpi target with mpirun
	add_custom_target(run_mpi
		COMMAND mpirun -np 4 $<TARGET_FILE:mpi>
		DEPENDS mpi
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Running mpi target with mpirun"
	)
else()
	message("Skipping MPI solution")
endif()

if(DEFINED ENV{BUILD_ACC} AND "$ENV{BUILD_ACC}" STREQUAL "true")
	message("building OpenACC solution")

	find_package(OpenACC REQUIRED)

	add_executable(openacc openacc.cpp)
	target_link_libraries(openacc PUBLIC fmt)
	target_link_libraries(openacc PUBLIC OpenACC::OpenACC_CXX)
	target_compile_options(openacc PUBLIC -fopenacc )
endif()


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

	add_executable(mpi mpi.cpp)
	find_package(MPI REQUIRED)
	target_include_directories(mpi SYSTEM PRIVATE ${MPI_INCLUDE_PATH})
	# target_link_libraries(mpi ${MPI_C_LIBRARIES})
	target_link_libraries(mpi PUBLIC MPI::MPI_CXX)

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

add_executable(vk vk.cpp)
target_link_libraries(vk PUBLIC framework)
target_link_libraries(vk PUBLIC fmt)

# custom target for OpenMP
add_executable(openmp openmp.cpp)
find_package(OpenMP REQUIRED)
target_link_libraries(openmp PUBLIC OpenMP::OpenMP_CXX)

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

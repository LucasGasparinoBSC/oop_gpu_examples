message("-- Configuring MPI, if appropriate...")
if (USE_MPI)
    find_package(MPI REQUIRED)
    if (NOT MPI_FOUND)
        #do nothing
        message(FATAL_ERROR "MPI not found!")
    endif()
endif()

function(set_mpi)
    target_link_libraries(${PROJECT_NAME} MPI::MPI_CXX MPI::MPI_C MPI::MPI_Fortran)
endfunction()
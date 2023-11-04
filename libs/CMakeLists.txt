add_subdirectory(image_processing)
add_subdirectory(neural_network)
add_subdirectory(sudoku_solver)

option(ENABLE_UNUSED_IP "Enable unused image processing library.")

if (ENABLE_UNUSED_IP)
    add_subdirectory(image_processing_unused)
endif ()

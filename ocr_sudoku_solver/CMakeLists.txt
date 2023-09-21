add_executable(ocr_sudoku_solver src/main.c)

target_link_libraries(
    ocr_sudoku_solver PRIVATE image_processing neural_network sudoku_solver
)

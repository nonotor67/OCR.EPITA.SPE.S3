add_executable(ocr_sudoku_solver src/main.c)

target_link_libraries(
    ocr_sudoku_solver PRIVATE image_processing neural_network sudoku_solver
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
target_link_libraries(ocr_sudoku_solver PRIVATE PkgConfig::GTK3)

add_executable(ocr_sudoku_solver src/main.c)

target_link_libraries(
    ocr_sudoku_solver PRIVATE image_processing neural_network sudoku_solver
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
target_include_directories(ocr_sudoku_solver PRIVATE ${GTK3_INCLUDE_DIRS})
target_compile_options(ocr_sudoku_solver PRIVATE ${GTK3_CFLAGS_OTHER})
target_link_directories(ocr_sudoku_solver PRIVATE ${GTK3_LIBRARY_DIRS})
target_link_libraries(ocr_sudoku_solver PRIVATE ${GTK3_LIBRARIES})
target_link_options(ocr_sudoku_solver PRIVATE ${GTK3_LDFLAGS_OTHER})

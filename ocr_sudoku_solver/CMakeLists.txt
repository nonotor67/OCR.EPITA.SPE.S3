add_executable(ocr_sudoku_solver src/main.c)

target_link_libraries(
    ocr_sudoku_solver PRIVATE image_processing neural_network sudoku_solver
)

find_package(PkgConfig REQUIRED)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
target_link_libraries(ocr_sudoku_solver PRIVATE SDL2::SDL2)

find_package(SDL2_image REQUIRED)
target_link_libraries(ocr_sudoku_solver PRIVATE SDL2_image::SDL2_image)

find_package(SDL2_ttf REQUIRED)
target_link_libraries(ocr_sudoku_solver PRIVATE SDL2_ttf::SDL2_ttf)

pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
target_link_libraries(ocr_sudoku_solver PRIVATE PkgConfig::GTK3)

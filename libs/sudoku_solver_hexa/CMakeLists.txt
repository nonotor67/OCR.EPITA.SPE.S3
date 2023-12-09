add_library(sudoku_solver_hexa src/sudoku_solver_hexa.c)

target_sources(
    sudoku_solver_hexa PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
    include/sudoku_solver_hexa.h
)

add_subdirectory(tools)

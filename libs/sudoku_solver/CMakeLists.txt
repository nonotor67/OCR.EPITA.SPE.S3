add_library(sudoku_solver src/sudoku_solver.c)

target_sources(
    sudoku_solver PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
    include/sudoku_solver.h
)

add_subdirectory(tools)

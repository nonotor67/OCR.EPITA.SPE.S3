add_executable(sudoku_solver_test sudoku_solver_test.c)
target_link_libraries(
    sudoku_solver_test PRIVATE sudoku_solver PkgConfig::CRITERION
)
add_test(NAME sudoku_solver_test COMMAND sudoku_solver_test)

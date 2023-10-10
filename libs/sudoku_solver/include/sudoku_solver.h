#ifndef OCR_SUDOKU_SOLVER_SUDOKU_SOLVER_H
#define OCR_SUDOKU_SOLVER_SUDOKU_SOLVER_H

#include <stdbool.h>

#define SS_WIDTH 9
#define SS_GRID_SIZE (SS_WIDTH * SS_WIDTH)

typedef int ss_grid[SS_GRID_SIZE];

// TODO: Remove hello.
void ss_say_hello(void);
bool ss_is_valid(const ss_grid grid);
bool ss_solve_sudoku(ss_grid grid);

#endif // OCR_SUDOKU_SOLVER_SUDOKU_SOLVER_H

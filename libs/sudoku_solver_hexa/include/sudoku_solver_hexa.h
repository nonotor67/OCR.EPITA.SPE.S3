#ifndef OCR_SUDOKU_SOLVER_SUDOKU_SOLVER_HEXA_H
#define OCR_SUDOKU_SOLVER_SUDOKU_SOLVER_HEXA_H

#include <stdbool.h>

#define SS_WIDTH 16
#define SS_GRID_SIZE (SS_WIDTH * SS_WIDTH)

typedef int ss_grid[SS_GRID_SIZE];

// TODO: Remove hello.
void ss_say_hello(void);
void ss_print_board(ss_grid *grid);
void init_possible_values(ss_grid *grid);
bool ss_is_valid(const ss_grid *grid);
bool ss_solve_sudoku(ss_grid *grid);

#endif // OCR_SUDOKU_SOLVER_SUDOKU_SOLVER_HEXA_H

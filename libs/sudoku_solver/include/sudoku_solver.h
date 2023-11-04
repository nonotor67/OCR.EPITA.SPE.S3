#ifndef OCR_SUDOKU_SOLVER_SUDOKU_SOLVER_H
#define OCR_SUDOKU_SOLVER_SUDOKU_SOLVER_H

#include <stdbool.h>

/// The width of a sudoku grid in number of cells.
#define SS_WIDTH 9
// The number of cells in a sudoku grid.
#define SS_GRID_SIZE (SS_WIDTH * SS_WIDTH)

/// A sudoku grid.
typedef int ss_grid[SS_GRID_SIZE];

/// Checks if a sudoku is valid.
/// \param grid The sudoku to check.
/// \return True if the grid is valid, otherwise false.
bool ss_is_valid(const ss_grid grid);

/// Solves a sudoku.
/// \param grid The sudoku to solve.
/// \return True if the sudoku could be solved, otherwise false.
bool ss_solve_sudoku(ss_grid grid);

#endif // OCR_SUDOKU_SOLVER_SUDOKU_SOLVER_H

#include "sudoku_solver.h"

#include <stdbool.h>
#include <stdio.h>

// TODO: Remove hello.
void ss_say_hello(void) {
    puts("Hello from sudoku_solver");
}

bool ss_is_valid(const ss_grid grid) {
    // check if there is no duplicate in the row
    for (int line = 0; line < SS_WIDTH; line++) {
        bool seen[SS_WIDTH] = { false };
        for (int col = 0; col < SS_WIDTH; col++) {
            int value = grid[line * SS_WIDTH + col];
            if (value != 0) {
                if (seen[value - 1]) {
                    return false;
                }
                seen[value - 1] = true;
            }
        }
    }

    // check if there is no duplicate in the column
    for (int col = 0; col < SS_WIDTH; col++) {
        bool seen[SS_WIDTH] = { false };
        for (int line = 0; line < SS_WIDTH; line++) {
            int value = grid[line * SS_WIDTH + col];
            if (value != 0) {
                if (seen[value - 1]) {
                    return false;
                }
                seen[value - 1] = true;
            }
        }
    }

    // check if there is no duplicate in the box
    for (int box = 0; box < SS_WIDTH; box++) {
        bool seen[SS_WIDTH] = { false };
        for (int box_cell = 0; box_cell < SS_WIDTH; box_cell++) {
            int line = box / 3 * 3 + box_cell / 3;
            int col = box % 3 * 3 + box_cell % 3;

            int value = grid[col * SS_WIDTH + line];
            if (value != 0) {
                if (seen[value - 1]) {
                    return false;
                }
                seen[value - 1] = true;
            }
        }
    }

    return true;
}

bool ss_solve_sudoku(ss_grid grid) {
    if (!ss_is_valid(grid)) {
        return false;
    }

    // find the first empty cell
    int i = 0;
    while (i < SS_GRID_SIZE && grid[i] != 0) {
        i++;
    }

    // if there is no empty cell, the board is solved
    if (i == SS_GRID_SIZE) {
        return true;
    }

    // try to fill the empty cell
    for (int value = 1; value <= SS_WIDTH; value++) {
        grid[i] = value;
        if (ss_solve_sudoku(grid)) {
            return true;
        }
    }

    // if no value works, the board is unsolvable
    grid[i] = 0;
    return false;
}

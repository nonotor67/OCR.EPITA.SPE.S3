#include "../include/sudoku_solver_hexa.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


bool possible_values[SS_GRID_SIZE][SS_WIDTH] = {{false}};

// TODO: Remove hello.
void ss_say_hello(void) {
    puts("Hello from sudoku_solver");
}

void init_possible_values(ss_grid *grid)
{
    for (int count = 0; count < SS_GRID_SIZE; count++){
        if ((*grid)[count] == -1)
        {
            for (int a = 0; a < SS_WIDTH; a++){
                possible_values[count][a] = true;
            }
        }
        //printf("%d\n", (*grid)[count]);
    }
    int value = 0;
    for (int line = 0; line < SS_WIDTH; line++) 
    {
        for (int col = 0; col < SS_WIDTH; col++) 
        {
            if ((*grid)[line * SS_WIDTH + col] == -1)
            {
                for (int inside = 0; inside < SS_WIDTH; inside++) {
                    //printf("inside: %d, line: %d, col: %d\n", inside, line, col);
                    if ((value = (*grid)[line * SS_WIDTH + inside]) != -1) {
                        possible_values[line * SS_WIDTH + col][value] = false;
                    }
                    if ((value = (*grid)[inside * SS_WIDTH + col]) != -1) {
                        possible_values[line * SS_WIDTH + col][value] = false;
                    }
                    if ((value = (*grid)[((line / 4) * 4 + (inside / 4)) * SS_WIDTH + (col / 4) * 4 + (inside % 4)]) != -1) {
                        //printf("first index: %d, inside index :%d, value: %d\n---\n", line * SS_WIDTH + col, inside_line * SS_WIDTH + inside_col, value);
                        possible_values[line * SS_WIDTH + col][value] = false;
                    }
                }
            }
        }
    }
}

/*void set_tf(ss_grid *grid, int line, int col, bool flag)
{
    int value = 0;
    for (int inside = 0; inside < SS_WIDTH; inside++) {
        //printf("inside: %d, line: %d, col: %d\n", inside, line, col);
        if ((value = (*grid)[line * SS_WIDTH + inside]) != -1) {
            possible_values[line * SS_WIDTH + col][value] = false;
        }
        if ((value = (*grid)[inside * SS_WIDTH + col]) != -1) {
            possible_values[line * SS_WIDTH + col][value] = false;
        }
        if ((value = (*grid)[((line / 4) * 4 + (inside / 4)) * SS_WIDTH + (col / 4) * 4 + (inside % 4)]) != -1) {
            //printf("first index: %d, inside index :%d, value: %d\n---\n", line * SS_WIDTH + col, inside_line * SS_WIDTH + inside_col, value);
            possible_values[line * SS_WIDTH + col][value] = false;
        }
    }
}*/

void ss_print_board(ss_grid *grid){
    if (*grid[0] == -1)
        printf(".");
    if (*grid[0] > 9)
            printf("%c", ((*grid)[0] + 'A' - 10));
    else
        printf("%d", (*grid)[0]);

    for (int i = 1; i < SS_GRID_SIZE; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }
        if (i % 64 == 0) {
            printf("\n");
        }
        if (i % 4 == 0 && i % 16 != 0) {
            printf(" ");
        }
        if ((*grid)[i] == -1)
            printf(".");
        else if ((*grid)[i] > 9)
            printf("%c", (*grid)[i] + 'A' - 10);
        else
            printf("%d", (*grid)[i]);
    } printf("\n");
    printf("\n------------------------\n");
}

bool ss_is_valid(const ss_grid *grid) {
    // check if there is no duplicate in the row
    for (int line = 0; line < SS_WIDTH; line++) {
        bool seen_line[SS_WIDTH] = { false };
        bool seen_col[SS_WIDTH] = { false };
        for (int col = 0; col < SS_WIDTH; col++) {
            int value_line = (*grid)[line * SS_WIDTH + col];
            int value_col = (*grid)[col * SS_WIDTH + line];
            if (value_line != -1) {
                if (seen_line[value_line]) {
                    //printf("error in line: col %d line %d value %d\n", col, line, value_line);
                    return false;
                }
                seen_line[value_line] = true;
            }
            if (value_col != -1) {
                if (seen_col[value_col]) {
                    //printf("error in col: col %d line %d value %d\n", line, col, value_col);
                    return false;
                }
                seen_col[value_col] = true;
            }
        }
    }

    // check if there is no duplicate in the box
    for (int line = 0; line < SS_WIDTH; line++) {
        bool seen[SS_WIDTH] = { false };
        for (int col = 0; col < SS_WIDTH; col++) {

            int value = (*grid)[(line % 4 * 4 + col % 4) * SS_WIDTH + line / 4 * 4 + col / 4];
            if (value != -1) {
                if (seen[value]) {
                    //printf("error in grid\n");
                    return false;
                }
                seen[value] = true;
            }
        }
    }

    return true;
}

int ss_is_solved(ss_grid *grid)
{
	for (size_t i = 0; i < SS_WIDTH; i++)
	{
		for (size_t j = 0; j < SS_WIDTH; j++)
		{
			if ((*grid)[i * SS_WIDTH + j] == -1)
				return 0;
		}
	}
	return ss_is_valid(grid);
}

// list for each cell of possible values
// if value inserted within cell/column/line remove possibility
// if value incorrect reinsert value within list

bool ss_solve_sudoku(ss_grid *grid) {
    int x = 0;
	int y = 0;
	int find = 0;
	while (x < SS_WIDTH && !find)
	{
		while(y < SS_WIDTH && !find)
		{
			if ((*grid)[x * SS_WIDTH + y] == -1)
				find = 1;
			else
				y++;
		}
		if (!find)
		{
			x++;
			y = 0;
		}
	}

	if (!find)
		return true;

	for (int val = 0; val < SS_WIDTH; val++)
	{
        if (possible_values[x * SS_WIDTH + y][val])
        {
            (*grid)[x * SS_WIDTH + y] = val;
            //possible_values[x * SS_WIDTH + y][val] = false;
            if (ss_is_valid(grid))
            {
                //ss_print_board(board->grid);
                if (ss_solve_sudoku(grid))
                    return true;
            }
            (*grid)[x * SS_WIDTH + y] = -1;
        }
	}
    return false;
}

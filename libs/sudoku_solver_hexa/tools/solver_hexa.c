#include "../include/sudoku_solver_hexa.h"

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *grid_file_path = argv[1];

    FILE *grid_file = fopen(grid_file_path, "r");
    if (grid_file == NULL) {
        fprintf(stderr, "error: cannot open file %s\n", grid_file_path);
        return EXIT_FAILURE;
    }

    ss_grid grid;
    //ss_grid *grid = malloc(sizeof(ss_grid) * SS_GRID_SIZE);

    for (int i = 0; i < SS_GRID_SIZE; i++) {
        int value = fgetc(grid_file);

        if (value == EOF) {
            if (ferror(grid_file)) {
                perror("error: cannot read from file");
            } else {
                fputs("error: file is too short\n", stderr);
            }

            fclose(grid_file);
            return EXIT_FAILURE;
        }

        if (value == ' ' || value == '\n') {
            i--;
            continue;
        }

        if (value == '.') {
            grid[i] = -1;
            continue;
        }

        if (value >= 'A' && value <= 'F'){
            grid[i] = value - 'A' + 10;
            continue;
        }

        if (value < '0' || value > '9') {
            fprintf(
                stderr,
                "error: file '%s' contains invalid character '%c'\n",
                grid_file_path,
                value
            );
            fclose(grid_file);
            return EXIT_FAILURE;
        }

        grid[i] = value - '0';
    }

    fclose(grid_file);

    init_possible_values(&grid);

    ss_print_board(&grid);

    if (ss_is_valid(&grid))
        printf ("board is valid\n");
    else
        errx(EXIT_FAILURE, "board INVALID\n");

    if (!ss_solve_sudoku(&grid)) {
        fprintf(stderr, "error: cannot solve sudoku\n");
        return EXIT_FAILURE;
    }
    ss_print_board(&grid);

    const char *result_file_ext = ".result";
    size_t result_file_path_len =
        strlen(grid_file_path) + strlen(result_file_ext) + 1;

    char *result_file_path = malloc(result_file_path_len);

    if (!result_file_path) {
        perror("error: failed to allocate result file path");
        return EXIT_FAILURE;
    }

    strcpy(result_file_path, grid_file_path);
    strcat(result_file_path, result_file_ext);

    FILE *result_file = fopen(result_file_path, "w");

    free(result_file_path);

    if (result_file == NULL) {
        perror("error: cannot create file");
        return EXIT_FAILURE;
    }

    if (grid[0] > 9)
        fprintf(result_file, "%c", grid[0] + 'A' - 10);
    else
        fprintf(result_file, "%d", grid[0]);

    for (int i = 1; i < SS_GRID_SIZE; i++) {
        if (i % 16 == 0 && i != -1) {
            fprintf(result_file, "\n");
        }
        if (i % 64 == 0 && i != -1) {
            fprintf(result_file, "\n");
        }
        if (i % 4 == 0 && i % 16 != 0 && i != -1) {
            fprintf(result_file, " ");
        }
        if (grid[i] > 9)
            fprintf(result_file, "%c", grid[i] + 'A' - 10);
        else
            fprintf(result_file, "%d", grid[i]);
    }
    fprintf(result_file, "\n");
    fclose(result_file);
    //free(grid);

    return EXIT_SUCCESS;
}

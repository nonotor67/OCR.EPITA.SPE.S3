#include "sudoku_solver.h"

#include <stdio.h>
#include <stdlib.h>
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

    ss_grid board;
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
            board[i] = 0;
            continue;
        }

        if (value < '1' || value > '9') {
            fprintf(
                stderr,
                "error: file '%s' contains invalid character '%c'\n",
                grid_file_path,
                value
            );
            fclose(grid_file);
            return EXIT_FAILURE;
        }

        board[i] = value - '0';
    }

    fclose(grid_file);

    if (!ss_solve_sudoku(board)) {
        fprintf(stderr, "error: cannot solve sudoku\n");
        return EXIT_FAILURE;
    }

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

    for (int i = 0; i < SS_GRID_SIZE; i++) {
        if (i % 9 == 0 && i != 0) {
            fprintf(result_file, "\n");
        }
        if (i % 27 == 0 && i != 0) {
            fprintf(result_file, "\n");
        }
        if (i % 3 == 0 && i != 0 && i % 9 != 0) {
            fprintf(result_file, " ");
        }
        fprintf(result_file, "%d", board[i]);
    }
    fprintf(result_file, "\n");
    fclose(result_file);

    return EXIT_SUCCESS;
}

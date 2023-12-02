#ifndef OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H
#define OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H

#include <stdbool.h>
#include <stddef.h>

struct nn_array {
    size_t size;
    float *data;
};

struct nn_matrix {
    size_t rows;
    size_t cols;
    float *data;
};

struct nn_dataset {
    struct nn_array labels;
    struct nn_matrix pixels;
};

bool nn_dataset_load(struct nn_dataset *dataset, const char *filepath);

void nn_dataset_fini(struct nn_dataset *dataset);

struct nn_model {
    struct nn_matrix w1;
    struct nn_array b1;
    struct nn_matrix w2;
    struct nn_array b2;
};

#endif // OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H

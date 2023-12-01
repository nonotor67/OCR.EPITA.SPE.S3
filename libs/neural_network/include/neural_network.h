#ifndef OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H
#define OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H

#include <stdbool.h>
#include <stddef.h>

struct nn_dataset {
    size_t size;
    float *labels;
    float *pixels;
};

bool nn_dataset_load(struct nn_dataset *dataset, const char *filepath);

void nn_dataset_fini(struct nn_dataset *dataset);

struct nn_model {
    float *w1;
    float *b1;
    float *w2;
    float *b2;
};

#endif // OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H

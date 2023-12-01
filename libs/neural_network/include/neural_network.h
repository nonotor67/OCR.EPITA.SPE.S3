#ifndef OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H
#define OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H

#include <stdbool.h>
#include <stddef.h>

#define NN_IMAGE_WIDTH 28
#define NN_IMAGE_SIZE (NN_IMAGE_WIDTH * NN_IMAGE_WIDTH)

struct nn_dataset {
    size_t size;
    float *labels;
    float *pixels;
};

bool nn_dataset_load(struct nn_dataset *dataset, const char *filepath);

void nn_dataset_fini(struct nn_dataset *dataset);

#endif // OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H

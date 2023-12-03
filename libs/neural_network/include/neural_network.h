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

bool nn_model_init(struct nn_model *model);

void nn_model_fini(struct nn_model *model);

void nn_model_rand(struct nn_model *model);

bool nn_model_read(struct nn_model *model, const char *filepath);

struct nn_context {
    struct nn_matrix z1;
    struct nn_matrix a1;
    struct nn_matrix a2;
    struct nn_array a2_sum;
};

bool nn_context_init(
    struct nn_context *ctx,
    const struct nn_model *model,
    const struct nn_dataset *dataset
);

void nn_context_fini(struct nn_context *ctx);

struct nn_matrix nn_infer(
    struct nn_context *ctx,
    const struct nn_model *model,
    struct nn_matrix input
);

#endif // OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H

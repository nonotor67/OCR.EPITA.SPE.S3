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

void nn_transpose(struct nn_matrix dst, struct nn_matrix src);

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

bool nn_model_write(const struct nn_model *model, const char *filepath);

struct nn_forward_prop_context {
    struct nn_matrix z1;
    struct nn_matrix a1;
    struct nn_matrix a2;
    struct nn_array a2_sum;
};

struct nn_backward_prop_context {
    struct nn_matrix dz2;
    struct nn_matrix a1_t;
    struct nn_matrix dw2;
    struct nn_array db2;
    struct nn_matrix w2_t;
    struct nn_matrix dz1;
    struct nn_matrix x_t;
    struct nn_matrix dw1;
    struct nn_array db1;
};

struct nn_train_context {
    struct nn_forward_prop_context fwd;
    struct nn_backward_prop_context bwd;
};

bool nn_train_context_init(
    struct nn_train_context *ctx,
    const struct nn_model *model,
    const struct nn_dataset *dataset
);

void nn_train_context_fini(struct nn_train_context *ctx);

void nn_train(
    struct nn_train_context *ctx,
    struct nn_model *model,
    const struct nn_dataset *dataset,
    float learning_rate,
    size_t iterations
);

struct nn_infer_context {
    struct nn_forward_prop_context fwd;
};

bool nn_infer_context_init(
    struct nn_infer_context *ctx,
    const struct nn_model *model
);

void nn_infer_context_fini(struct nn_infer_context *ctx);

size_t nn_infer(
    struct nn_infer_context *ctx,
    const struct nn_model *model,
    struct nn_array input
);

#endif // OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H

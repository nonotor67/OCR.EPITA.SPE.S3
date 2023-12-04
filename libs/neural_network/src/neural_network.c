#include "neural_network.h"

#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Layer sizes in number of neurons.
#define NN_INPUT_SIZE (28 * 28)
#define NN_HIDDEN_SIZE 10
#define NN_OUTPUT_SIZE 10

#define NN_LOG(fmt, ...)        \
    fprintf(                    \
        stderr,                 \
        "%s.%d: %s: " fmt "\n", \
        __FILE_NAME__,          \
        __LINE__,               \
        __func__,               \
        ##__VA_ARGS__           \
    )
#define NN_LOGE(fmt, ...) NN_LOG(fmt ": %s", ##__VA_ARGS__, strerror(errno))

static bool nn_array_init(struct nn_array *array, size_t size) {
    array->size = size;
    return (array->data = malloc(size * sizeof(float)));
}

static void nn_array_fini(struct nn_array *array) {
    free(array->data);
}

static bool nn_matrix_init(struct nn_matrix *matrix, size_t rows, size_t cols) {
    matrix->rows = rows;
    matrix->cols = cols;
    return (matrix->data = malloc(rows * cols * sizeof(float)));
}

static void nn_matrix_fini(struct nn_matrix *matrix) {
    free(matrix->data);
}

#define NN_AT(matrix, x, y) ((matrix).data[(y) * (matrix).cols + (x)])

static struct nn_array nn_as_array(struct nn_matrix matrix) {
    return (struct nn_array){
        .size = matrix.rows * matrix.cols,
        .data = matrix.data,
    };
}

static bool nn_read(struct nn_array dst, FILE *file) {
    size_t pos = 0;

    while (pos < dst.size) {
        if (feof(file) || ferror(file)) {
            NN_LOGE("unexpected EOF or read error");
            return false;
        }

        pos += fread(&dst.data[pos], sizeof(float), dst.size - pos, file);
    }

    return true;
}

static bool nn_write(struct nn_array src, FILE *file) {
    size_t pos = 0;

    while (pos < src.size) {
        if (feof(file)) {
            NN_LOGE("write error");
            return false;
        }

        pos += fwrite(&src.data[pos], sizeof(float), src.size - pos, file);
    }

    return true;
}

bool nn_dataset_load(struct nn_dataset *dataset, const char *filepath) {
    FILE *file = fopen(filepath, "rb");

    if (!file) {
        NN_LOGE("failed to open file");
        return false;
    }

    size_t dataset_size;

    if (fread(&dataset_size, sizeof(dataset_size), 1, file) < 1) {
        NN_LOGE("failed to read size");
        fclose(file);
        return false;
    }

    if (!nn_array_init(&dataset->labels, dataset_size)) {
        NN_LOGE("failed to allocate labels");
        fclose(file);
        return false;
    }

    if (!nn_read(dataset->labels, file)) {
        NN_LOGE("failed to read labels");
        nn_array_fini(&dataset->labels);
        fclose(file);
        return false;
    }

    if (!nn_matrix_init(&dataset->pixels, NN_INPUT_SIZE, dataset_size)) {
        NN_LOGE("failed to allocate pixels");
        nn_array_fini(&dataset->labels);
        fclose(file);
        return false;
    }

    if (!nn_read(nn_as_array(dataset->pixels), file)) {
        NN_LOGE("failed to read pixels");
        nn_matrix_fini(&dataset->pixels);
        nn_array_fini(&dataset->labels);
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

void nn_dataset_fini(struct nn_dataset *dataset) {
    nn_array_fini(&dataset->labels);
    nn_matrix_fini(&dataset->pixels);
}

bool nn_model_init(struct nn_model *model) {
    if (!nn_matrix_init(&model->w1, NN_HIDDEN_SIZE, NN_INPUT_SIZE)) {
        return false;
    }

    if (!nn_array_init(&model->b1, NN_HIDDEN_SIZE)) {
        nn_matrix_fini(&model->w1);
        return false;
    }

    if (!nn_matrix_init(&model->w2, NN_OUTPUT_SIZE, NN_HIDDEN_SIZE)) {
        nn_array_fini(&model->b1);
        nn_matrix_fini(&model->w1);
        return false;
    }

    if (!nn_array_init(&model->b2, NN_OUTPUT_SIZE)) {
        nn_matrix_fini(&model->w2);
        nn_array_fini(&model->b1);
        nn_matrix_fini(&model->w1);
        return false;
    }

    return true;
}

void nn_model_fini(struct nn_model *model) {
    nn_matrix_fini(&model->w1);
    nn_array_fini(&model->b1);
    nn_matrix_fini(&model->w2);
    nn_array_fini(&model->b2);
}

static void nn_rand(struct nn_array dst, float displacement) {
    for (size_t i = 0; i < dst.size; i++) {
        dst.data[i] = (float) rand() / (float) RAND_MAX + displacement;
    }
}

void nn_model_rand(struct nn_model *model) {
    nn_rand(nn_as_array(model->w1), -0.5f);
    nn_rand(model->b1, -0.5f);
    nn_rand(nn_as_array(model->w2), -0.5f);
    nn_rand(model->b2, -0.5f);
}

bool nn_model_read(struct nn_model *model, const char *filepath) {
    FILE *file = fopen(filepath, "rb");

    if (!file) {
        NN_LOGE("failed to open file");
        return false;
    }

    return nn_read(nn_as_array(model->w1), file) && nn_read(model->b1, file) &&
        nn_read(nn_as_array(model->w2), file) && nn_read(model->b2, file);
}

bool nn_model_write(const struct nn_model *model, const char *filepath) {
    FILE *file = fopen(filepath, "wb");

    if (!file) {
        NN_LOGE("failed to open file");
        return false;
    }

    return nn_write(nn_as_array(model->w1), file) &&
        nn_write(model->b1, file) && nn_write(nn_as_array(model->w2), file) &&
        nn_write(model->b2, file);
}

static bool nn_forward_prop_context_init(
    struct nn_forward_prop_context *ctx,
    const struct nn_model *model,
    const struct nn_dataset *dataset
) {
    if (!nn_matrix_init(&ctx->z1, model->w1.rows, dataset->pixels.cols)) {
        return false;
    }

    if (!nn_matrix_init(&ctx->a1, model->w1.rows, dataset->pixels.cols)) {
        nn_matrix_fini(&ctx->z1);
        return false;
    }

    if (!nn_matrix_init(&ctx->a2, model->w2.rows, ctx->a1.cols)) {
        nn_matrix_fini(&ctx->a1);
        nn_matrix_fini(&ctx->z1);
        return false;
    }

    if (!nn_array_init(&ctx->a2_sum, ctx->a2.cols)) {
        nn_matrix_fini(&ctx->a2);
        nn_matrix_fini(&ctx->a1);
        nn_matrix_fini(&ctx->z1);
        return false;
    }

    return true;
}

static void nn_forward_prop_context_fini(struct nn_forward_prop_context *ctx) {
    nn_matrix_fini(&ctx->z1);
    nn_matrix_fini(&ctx->a1);
    nn_matrix_fini(&ctx->a2);
    nn_array_fini(&ctx->a2_sum);
}

static bool nn_backward_prop_context_init(
    struct nn_backward_prop_context *ctx,
    struct nn_forward_prop_context *fwd_ctx,
    const struct nn_model *model,
    const struct nn_dataset *dataset
) {
    if (!nn_matrix_init(&ctx->dz2, fwd_ctx->a2.rows, fwd_ctx->a2.cols)) {
        return false;
    }

    if (!nn_matrix_init(&ctx->a1_t, fwd_ctx->a1.cols, fwd_ctx->a1.rows)) {
        nn_matrix_fini(&ctx->dz2);
    }

    if (!nn_matrix_init(&ctx->dw2, ctx->dz2.rows, ctx->a1_t.cols)) {
        nn_matrix_fini(&ctx->a1_t);
        nn_matrix_fini(&ctx->dz2);
        return false;
    }

    if (!nn_array_init(&ctx->db2, model->b2.size)) {
        nn_matrix_fini(&ctx->dw2);
        nn_matrix_fini(&ctx->a1_t);
        nn_matrix_fini(&ctx->dz2);
    }

    if (!nn_matrix_init(&ctx->w2_t, model->w2.cols, model->w2.rows)) {
        nn_array_fini(&ctx->db2);
        nn_matrix_fini(&ctx->dw2);
        nn_matrix_fini(&ctx->a1_t);
        nn_matrix_fini(&ctx->dz2);
    }

    if (!nn_matrix_init(&ctx->dz1, ctx->w2_t.rows, ctx->dz2.cols)) {
        nn_matrix_fini(&ctx->w2_t);
        nn_array_fini(&ctx->db2);
        nn_matrix_fini(&ctx->dw2);
        nn_matrix_fini(&ctx->a1_t);
        nn_matrix_fini(&ctx->dz2);
        return false;
    }

    struct nn_matrix pixels = dataset->pixels;

    if (!nn_matrix_init(&ctx->x_t, pixels.cols, pixels.rows)) {
        nn_matrix_fini(&ctx->dz1);
        nn_array_fini(&ctx->db2);
        nn_matrix_fini(&ctx->dw2);
        nn_matrix_fini(&ctx->a1_t);
        nn_matrix_fini(&ctx->dz2);
    }

    if (!nn_matrix_init(&ctx->dw1, ctx->dz1.rows, pixels.rows)) {
        nn_matrix_fini(&ctx->x_t);
        nn_matrix_fini(&ctx->dz1);
        nn_array_fini(&ctx->db2);
        nn_matrix_fini(&ctx->dw2);
        nn_matrix_fini(&ctx->a1_t);
        nn_matrix_fini(&ctx->dz2);
        return false;
    }

    if (!nn_array_init(&ctx->db1, model->b1.size)) {
        nn_matrix_fini(&ctx->dw1);
        nn_matrix_fini(&ctx->x_t);
        nn_matrix_fini(&ctx->dz1);
        nn_array_fini(&ctx->db2);
        nn_matrix_fini(&ctx->dw2);
        nn_matrix_fini(&ctx->a1_t);
        nn_matrix_fini(&ctx->dz2);
    }

    return true;
}

static void nn_backward_prop_context_fini(struct nn_backward_prop_context *ctx
) {
    nn_matrix_fini(&ctx->dz2);
    nn_matrix_fini(&ctx->dw2);
    nn_matrix_fini(&ctx->dz1);
    nn_matrix_fini(&ctx->dw1);
}

bool nn_train_context_init(
    struct nn_train_context *ctx,
    const struct nn_model *model,
    const struct nn_dataset *dataset
) {
    if (!nn_forward_prop_context_init(&ctx->fwd, model, dataset)) {
        return false;
    }

    if (!nn_backward_prop_context_init(&ctx->bwd, &ctx->fwd, model, dataset)) {
        nn_forward_prop_context_fini(&ctx->fwd);
        return false;
    }

    return true;
}

void nn_train_context_fini(struct nn_train_context *ctx) {
    nn_forward_prop_context_fini(&ctx->fwd);
    nn_backward_prop_context_fini(&ctx->bwd);
}

static void
nn_dot(struct nn_matrix dst, struct nn_matrix a, struct nn_matrix b) {
    for (size_t y = 0; y < dst.rows; y++) {
        for (size_t x = 0; x < dst.cols; x++) {
            float sum = 0.0f;

            for (size_t z = 0; z < a.cols; z++) {
                sum += NN_AT(a, z, y) * NN_AT(b, x, z);
            }

            NN_AT(dst, x, y) = sum;
        }
    }
}

static void nn_add(struct nn_matrix a, struct nn_array b) {
    for (size_t y = 0; y < a.rows; y++) {
        float value = b.data[y];

        for (size_t x = 0; x < a.cols; x++) {
            NN_AT(a, x, y) += value;
        }
    }
}

static void nn_relu(struct nn_array dst, struct nn_array src) {
    for (size_t i = 0; i < dst.size; i++) {
        float value = src.data[i];
        dst.data[i] = value >= 0.0f ? value : 0.0f;
    }
}

static void nn_exp(struct nn_array array) {
    for (size_t i = 0; i < array.size; i++) {
        array.data[i] = expf(array.data[i]);
    }
}

static void nn_sum_cols(struct nn_array dst, struct nn_matrix src) {
    memset(dst.data, 0, dst.size * sizeof(float));

    for (size_t y = 0; y < src.rows; y++) {
        for (size_t x = 0; x < src.cols; x++) {
            dst.data[x] += NN_AT(src, x, y);
        }
    }
}

static void nn_div(struct nn_matrix a, struct nn_array b) {
    for (size_t y = 0; y < a.rows; y++) {
        for (size_t x = 0; x < a.cols; x++) {
            NN_AT(a, x, y) /= b.data[x];
        }
    }
}

static void nn_softmax(struct nn_matrix matrix, struct nn_array sum_dst) {
    nn_exp(nn_as_array(matrix));
    nn_sum_cols(sum_dst, matrix);
    nn_div(matrix, sum_dst);
}

static void nn_forward_prop(
    struct nn_forward_prop_context *ctx,
    const struct nn_model *model,
    struct nn_matrix input
) {
    // Z1 = W1.dot(X) + b1
    nn_dot(ctx->z1, model->w1, input);
    nn_add(ctx->z1, model->b1);
    // A1 = ReLU(Z1)
    nn_relu(nn_as_array(ctx->a1), nn_as_array(ctx->z1));
    // Z2 = W2.dot(A1) + b2
    /* NOTE: We only ever use Z2 to calculate A2, so instead of storing Z2
     * separately we write into A2 from the start. */
    nn_dot(ctx->a2, model->w2, ctx->a1);
    nn_add(ctx->a2, model->b2);
    // A2 = softmax(Z2)
    nn_softmax(ctx->a2, ctx->a2_sum);
}

static void nn_one_hot_transposed(struct nn_matrix dst, struct nn_array src) {
    for (size_t y = 0; y < dst.rows; y++) {
        for (size_t x = 0; x < dst.cols; x++) {
            NN_AT(dst, x, y) = (float) ((size_t) src.data[x] == y);
        }
    }
}

static void nn_sub(struct nn_array dst, struct nn_array a, struct nn_array b) {
    for (size_t i = 0; i < a.size; i++) {
        dst.data[i] = a.data[i] - b.data[i];
    }
}

static void nn_transpose(struct nn_matrix dst, struct nn_matrix src) {
    for (size_t y = 0; y < dst.rows; y++) {
        for (size_t x = 0; x < dst.cols; x++) {
            NN_AT(dst, x, y) = NN_AT(src, y, x);
        }
    }
}

static void nn_mul_scalar(struct nn_array a, float b) {
    for (size_t i = 0; i < a.size; i++) {
        a.data[i] *= b;
    }
}

static void nn_sum_rows(struct nn_array dst, struct nn_matrix src) {
    memset(dst.data, 0, dst.size * sizeof(float));

    for (size_t y = 0; y < src.rows; y++) {
        for (size_t x = 0; x < src.cols; x++) {
            dst.data[y] += NN_AT(src, x, y);
        }
    }
}

static void nn_relu_deriv(struct nn_array dst, struct nn_array src) {
    for (size_t i = 0; i < dst.size; i++) {
        float value = src.data[i];
        dst.data[i] = value >= 0.0f ? 1.0f : 0.0f;
    }
}

static void nn_mul(struct nn_array a, struct nn_array b) {
    for (size_t i = 0; i < a.size; i++) {
        a.data[i] *= b.data[i];
    }
}

static void nn_backward_prop(
    struct nn_train_context *ctx,
    const struct nn_model *model,
    const struct nn_dataset *dataset
) {
    // 1 / m
    float rcp_m = 1.0f / (float) dataset->pixels.cols;

    // one_hot_Y = one_hot(Y)
    nn_one_hot_transposed(ctx->bwd.dz2, dataset->labels);
    // dZ2 = A2 - one_hot_Y
    nn_sub(
        nn_as_array(ctx->bwd.dz2),
        nn_as_array(ctx->fwd.a2),
        nn_as_array(ctx->bwd.dz2)
    );
    // dW2 = 1 / m * dZ2.dot(A1.T)
    nn_transpose(ctx->bwd.a1_t, ctx->fwd.a1);
    nn_dot(ctx->bwd.dw2, ctx->bwd.dz2, ctx->bwd.a1_t);
    nn_mul_scalar(nn_as_array(ctx->bwd.dw2), rcp_m);
    // db2 = 1 / m * np.sum(dZ2)
    nn_sum_rows(ctx->bwd.db2, ctx->bwd.dz2);
    nn_mul_scalar(ctx->bwd.db2, rcp_m);
    // dZ1 = W2.T.dot(dZ2) * ReLU_deriv(Z1)
    nn_transpose(ctx->bwd.w2_t, model->w2);
    nn_dot(ctx->bwd.dz1, ctx->bwd.w2_t, ctx->bwd.dz2);
    // NOTE: Mutating Z1.
    nn_relu_deriv(nn_as_array(ctx->fwd.z1), nn_as_array(ctx->fwd.z1));
    nn_mul(nn_as_array(ctx->bwd.dz1), nn_as_array(ctx->fwd.z1));
    // dW1 = 1 / m * dZ1.dot(X.T)
    nn_transpose(ctx->bwd.x_t, dataset->pixels);
    nn_dot(ctx->bwd.dw1, ctx->bwd.dz1, ctx->bwd.x_t);
    nn_mul_scalar(nn_as_array(ctx->bwd.dw1), rcp_m);
    // db1 = 1 / m * np.sum(dZ1)
    nn_sum_rows(ctx->bwd.db1, ctx->bwd.dz1);
    nn_mul_scalar(ctx->bwd.db1, rcp_m);
}

void nn_update_params(
    struct nn_model *model,
    struct nn_backward_prop_context *ctx,
    float learning_rate
) {
    // W1 = W1 - alpha * dW1
    nn_mul_scalar(nn_as_array(ctx->dw1), learning_rate);
    nn_sub(
        nn_as_array(model->w1),
        nn_as_array(model->w1),
        nn_as_array(ctx->dw1)
    );
    // b1 = b1 - alpha * db1
    nn_mul_scalar(ctx->db1, learning_rate);
    nn_sub(model->b1, model->b1, ctx->db1);
    // W2 = W2 - alpha * dW2
    nn_mul_scalar(nn_as_array(ctx->dw2), learning_rate);
    nn_sub(
        nn_as_array(model->w2),
        nn_as_array(model->w2),
        nn_as_array(ctx->dw2)
    );
    // b2 = b2 - alpha * db2
    nn_mul_scalar(ctx->db2, learning_rate);
    nn_sub(model->b2, model->b2, ctx->db2);
}

static void
nn_log_accuracy(struct nn_matrix output, struct nn_array expected_labels) {
    size_t num_accurate_labels = 0;

    for (size_t x = 0; x < output.cols; x++) {
        size_t argmax = 0;

        for (size_t y = 1; y < output.rows; y++) {
            if (NN_AT(output, x, y) > NN_AT(output, x, argmax)) {
                argmax = y;
            }
        }

        if (argmax == (size_t) expected_labels.data[x]) {
            num_accurate_labels++;
        }
    }

    NN_LOG(
        "accuracy: %.9f (%zu/%zu)",
        (float) num_accurate_labels / (float) expected_labels.size,
        num_accurate_labels,
        expected_labels.size
    );
}

void nn_train(
    struct nn_train_context *ctx,
    struct nn_model *model,
    const struct nn_dataset *dataset,
    float learning_rate,
    size_t iterations
) {
    for (size_t i = 0; i < iterations; i++) {
        nn_forward_prop(&ctx->fwd, model, dataset->pixels);
        nn_backward_prop(ctx, model, dataset);
        nn_update_params(model, &ctx->bwd, learning_rate);

        if (i % 10 == 0) {
            NN_LOG("training iteration #%zu", i);
            nn_log_accuracy(ctx->fwd.a2, dataset->labels);
        }
    }
}

// TODO: Needs `struct nn_infer_context`.
#if 0
    struct nn_matrix nn_infer(
        struct nn_context *ctx,
        const struct nn_model *model,
        struct nn_matrix input
    ) {
    nn_forward_prop(ctx, model, input);
    return ctx->a2;
}
#endif

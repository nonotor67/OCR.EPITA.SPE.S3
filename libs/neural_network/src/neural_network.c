#include "neural_network.h"

#include <errno.h>
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

static struct nn_array nn_as_array(struct nn_matrix matrix) {
    return (struct nn_array){
        .size = matrix.rows * matrix.cols,
        .data = matrix.data,
    };
}

static bool nn_read_file(struct nn_array dst, FILE *file) {
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

    if (!nn_read_file(dataset->labels, file)) {
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

    if (!nn_read_file(nn_as_array(dataset->pixels), file)) {
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

static void nn_rand(struct nn_array dst, float displacement) {
    for (size_t i = 0; i < dst.size; i++) {
        dst.data[i] = (float) rand() / (float) RAND_MAX + displacement;
    }
}

static bool nn_model_init_rand(struct nn_model *model) {
    if (!nn_matrix_init(&model->w1, NN_HIDDEN_SIZE, NN_INPUT_SIZE)) {
        return false;
    }

    nn_rand(nn_as_array(model->w1), -0.5f);

    if (!nn_array_init(&model->b1, NN_HIDDEN_SIZE)) {
        nn_matrix_fini(&model->w1);
        return false;
    }

    nn_rand(model->b1, -0.5f);

    if (!nn_matrix_init(&model->w2, NN_OUTPUT_SIZE, NN_HIDDEN_SIZE)) {
        nn_array_fini(&model->b1);
        nn_matrix_fini(&model->w1);
        return false;
    }

    nn_rand(nn_as_array(model->w2), -0.5f);

    if (!nn_array_init(&model->b2, NN_OUTPUT_SIZE)) {
        nn_matrix_fini(&model->w2);
        nn_array_fini(&model->b1);
        nn_matrix_fini(&model->w1);
        return false;
    }

    nn_rand(model->b2, -0.5f);
    return true;
}

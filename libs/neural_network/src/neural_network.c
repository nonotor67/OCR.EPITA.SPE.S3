#include "neural_network.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static bool nn_read_floats(float *dst, size_t count, FILE *file) {
    size_t pos = 0;

    while (pos < count) {
        if (feof(file) || ferror(file)) {
            NN_LOGE("unexpected EOF or read error");
            return false;
        }

        pos += fread(&dst[pos], sizeof(float), count - pos, file);
    }

    return true;
}

bool nn_dataset_load(struct nn_dataset *dataset, const char *filepath) {
    FILE *file = fopen(filepath, "rb");

    if (!file) {
        NN_LOGE("failed to open file");
        return false;
    }

    if (fread(&dataset->size, sizeof(size_t), 1, file) < 1) {
        NN_LOGE("failed to read size");
        fclose(file);
        return false;
    }

    dataset->labels = malloc(dataset->size * sizeof(float));

    if (!dataset->labels) {
        NN_LOGE("failed to allocate labels");
        fclose(file);
        return false;
    }

    if (!nn_read_floats(dataset->labels, dataset->size, file)) {
        NN_LOGE("failed to read labels");
        free(dataset->labels);
        fclose(file);
        return false;
    }

    size_t num_pixels = NN_IMAGE_SIZE * dataset->size;
    dataset->pixels = malloc(num_pixels * sizeof(float));

    if (!dataset->pixels) {
        NN_LOGE("failed to allocate pixels");
        free(dataset->labels);
        fclose(file);
        return false;
    }

    if (!nn_read_floats(dataset->pixels, num_pixels, file)) {
        NN_LOGE("failed to read pixels");
        free(dataset->pixels);
        free(dataset->labels);
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

void nn_dataset_fini(struct nn_dataset *dataset) {
    free(dataset->labels);
    free(dataset->pixels);
}

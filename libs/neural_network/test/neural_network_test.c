#include "neural_network.h"

#include <criterion/criterion.h>

#include <stddef.h>
#include <stdio.h>

Test(neural_network, example) {
    struct nn_dataset dataset;
    cr_assert(nn_dataset_load(&dataset, "train.bin"));

    struct nn_model model;
    cr_assert(nn_model_init(&model));
    cr_assert(nn_model_read(&model, "model_py.bin"));

    struct nn_context ctx;
    cr_assert(nn_context_init(&ctx, &model, &dataset));

    struct nn_matrix output = nn_infer(&ctx, &model, dataset.pixels);

    for (size_t y = 0; y < output.rows; y++) {
        for (size_t x = 0; x < output.cols; x++) {
            printf("%.6f\n", output.data[y * output.cols + x]);
        }

        puts("---");
    }

    nn_context_fini(&ctx);
    nn_model_fini(&model);
    nn_dataset_fini(&dataset);
}

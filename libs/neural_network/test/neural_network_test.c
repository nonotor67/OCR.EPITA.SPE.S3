#include "neural_network.h"

#include <criterion/criterion.h>

#include <stddef.h>
#include <stdio.h>

Test(neural_network, example) {
    struct nn_dataset dataset;
    cr_assert(nn_dataset_load(&dataset, "/tmp/train.bin"));

    struct nn_model model;
    cr_assert(nn_model_init(&model));
    nn_model_rand(&model);

    struct nn_train_context ctx;
    cr_assert(nn_train_context_init(&ctx, &model, &dataset));

    nn_train(&ctx, &model, &dataset, 0.1f, 11);

    nn_model_write(&model, "/tmp/model_c.bin");

    struct nn_infer_context infer_ctx;
    cr_assert(nn_infer_context_init(&infer_ctx, &model));

    float input_data[28 * 28];
    struct nn_array input = { .size = 28 * 28, .data = input_data };

    for (size_t x = 0; x < 4; x++) {
        for (size_t y = 0; y < 28 * 28; y++) {
            input_data[y] = dataset.pixels.data[y * dataset.pixels.cols + x];
        }

        size_t label = nn_infer(&infer_ctx, &model, input);
        printf(
            "got label %zu, expected %zu\n",
            label,
            (size_t) dataset.labels.data[x]
        );
    }

    nn_infer_context_fini(&infer_ctx);
    nn_train_context_fini(&ctx);
    nn_model_fini(&model);
    nn_dataset_fini(&dataset);
}

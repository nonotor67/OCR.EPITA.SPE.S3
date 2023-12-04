#include "neural_network.h"

#include <criterion/criterion.h>

#include <stddef.h>

Test(neural_network, example) {
    struct nn_dataset dataset;
    cr_assert(nn_dataset_load(&dataset, "/tmp/train.bin"));

    struct nn_model model;
    cr_assert(nn_model_init(&model));
    nn_model_rand(&model);

    struct nn_train_context ctx;
    cr_assert(nn_train_context_init(&ctx, &model, &dataset));

    // struct nn_matrix output = nn_infer(&ctx, &model, dataset.pixels);
    nn_train(&ctx, &model, &dataset, 0.1f, 11);

    nn_model_write(&model, "/tmp/model_c.bin");

    nn_train_context_fini(&ctx);
    nn_model_fini(&model);
    nn_dataset_fini(&dataset);
}

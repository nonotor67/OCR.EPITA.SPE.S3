#include "neural_network.h"

#include <criterion/criterion.h>

Test(neural_network, example) {
    struct nn_dataset dataset;
    cr_assert(nn_dataset_load(&dataset, "train.bin"));
    nn_dataset_fini(&dataset);
}

#include "neural_network.h"

#include <criterion/criterion.h>

#include <stdio.h>

Test(neural_network, example) {
    nn_say_hello();

    printf("########## Simple Gates ##########\n");

    float w1 = nn_rand_float();
    float w2 = nn_rand_float();
    float b = nn_rand_float();

    nn_learn_sg(&w1, &w2, &b, 0.1f);

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            printf(
                "%d %d -> %f\n",
                i,
                j,
                nn_sigmoid_f(w1 * (float) i + w2 * (float) j + b)
            );
        }
    }

    printf("########## XOR ##########\n");

    struct nn_model_xor m = nn_rand_xor();
    nn_learn_xor(&m, 0.1f);

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            printf(
                "%d %d -> %f\n",
                i,
                j,
                nn_forward_xor(&m, (float) i, (float) j)
            );
        }
    }

    cr_assert(true);
}

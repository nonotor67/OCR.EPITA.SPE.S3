#include "neural_network.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float nn_sigmoid_f(float x) {
    return 1.0f / (1.0f + expf(-x));
}

float nn_rand_float(void) {
    return (float) rand() / (float) RAND_MAX;
}

struct nn_model_xor nn_rand_xor(void) {
    return (struct nn_model_xor){
        .or_w1 = nn_rand_float(),
        .or_w2 = nn_rand_float(),
        .or_b = nn_rand_float(),
        .nand_w1 = nn_rand_float(),
        .nand_w2 = nn_rand_float(),
        .nand_b = nn_rand_float(),
        .and_w1 = nn_rand_float(),
        .and_w2 = nn_rand_float(),
        .and_b = nn_rand_float(),
    };
}

void nn_print_xor(const struct nn_model_xor *m) {
    printf("or_w1: %f\n", m->or_w1);
    printf("or_w2: %f\n", m->or_w2);
    printf("or_b: %f\n", m->or_b);
    printf("nand_w1: %f\n", m->nand_w1);
    printf("nand_w2: %f\n", m->nand_w2);
    printf("nand_b: %f\n", m->nand_b);
    printf("and_w1: %f\n", m->and_w1);
    printf("and_w2: %f\n", m->and_w2);
    printf("and_b: %f\n", m->and_b);
}

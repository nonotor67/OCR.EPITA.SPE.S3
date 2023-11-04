#include "neural_network.h"

#include <stdio.h>

#define NN_TRAIN_SIZE 4
#define NN_EPOCHS (1000 * 1000)

typedef float nn_sample[3];

const nn_sample nn_xor_samples[] = {
    { 0, 0, 0 },
    { 0, 1, 1 },
    { 1, 0, 1 },
    { 1, 1, 0 },
};
const nn_sample nn_nxor_samples[] = {
    { 0, 0, 1 },
    { 0, 1, 0 },
    { 1, 0, 0 },
    { 1, 1, 1 },
};

const nn_sample *train_xor = nn_xor_samples;

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

float nn_forward_xor(const struct nn_model_xor *m, float x1, float x2) {
    float or = nn_sigmoid_f(m->or_w1 * x1 + m->or_w2 * x2 + m->or_b);
    float nand = nn_sigmoid_f(m->nand_w1 * x1 + m->nand_w2 * x2 + m->nand_b);
    float and = nn_sigmoid_f(m->and_w1 * or +m->and_w2 * nand + m->and_b);
    return and;
}

float nn_cost_xor(const struct nn_model_xor *m) {
    float result = 0.0f;

    for (size_t i = 0; i < 4; i++) {
        float x1 = train_xor[i][0];
        float x2 = train_xor[i][1];
        float y = nn_forward_xor(m, x1, x2);
        float d = y - train_xor[i][2];
        result += d * d;
    }

    result /= 4;
    return result;
}

void nn_learn_xor(struct nn_model_xor *m, float learning_rate) {
    struct nn_model_xor n;

    for (size_t i = 0; i < NN_EPOCHS; ++i) {
        float c = nn_cost_xor(m);

        n = *m;
        n.or_w1 += NN_EPS;
        float d_or_w1 = (nn_cost_xor(&n) - c) / NN_EPS;
        n = *m;
        n.or_w2 += NN_EPS;
        float d_or_w2 = (nn_cost_xor(&n) - c) / NN_EPS;
        n = *m;
        n.or_b += NN_EPS;
        float d_or_b = (nn_cost_xor(&n) - c) / NN_EPS;
        n = *m;
        n.nand_w1 += NN_EPS;

        float d_nand_w1 = (nn_cost_xor(&n) - c) / NN_EPS;
        n = *m;
        n.nand_w2 += NN_EPS;
        float d_nand_w2 = (nn_cost_xor(&n) - c) / NN_EPS;
        n = *m;
        n.nand_b += NN_EPS;
        float d_nand_b = (nn_cost_xor(&n) - c) / NN_EPS;
        n = *m;
        n.and_w1 += NN_EPS;

        float d_and_w1 = (nn_cost_xor(&n) - c) / NN_EPS;
        n = *m;
        n.and_w2 += NN_EPS;
        float d_and_w2 = (nn_cost_xor(&n) - c) / NN_EPS;
        n = *m;
        n.and_b += NN_EPS;
        float d_and_b = (nn_cost_xor(&n) - c) / NN_EPS;

        m->or_w1 -= learning_rate * d_or_w1;
        m->or_w2 -= learning_rate * d_or_w2;
        m->or_b -= learning_rate * d_or_b;
        m->nand_w1 -= learning_rate * d_nand_w1;
        m->nand_w2 -= learning_rate * d_nand_w2;
        m->nand_b -= learning_rate * d_nand_b;
        m->and_w1 -= learning_rate * d_and_w1;
        m->and_w2 -= learning_rate * d_and_w2;
        m->and_b -= learning_rate * d_and_b;
    }
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

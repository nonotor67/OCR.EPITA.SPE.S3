#include "neural_network.h"

#include <stddef.h>

#define NN_TRAIN_SIZE 4
#define NN_EPOCHS (1000 * 1000)

typedef float nn_sample[3];

const nn_sample nn_or_samples[] = {
    { 0, 0, 0 },
    { 0, 1, 1 },
    { 1, 0, 1 },
    { 1, 1, 1 },
};
const nn_sample nn_and_samples[] = {
    { 0, 0, 0 },
    { 0, 1, 0 },
    { 1, 0, 0 },
    { 1, 1, 1 },
};
const nn_sample nn_nand_samples[] = {
    { 0, 0, 1 },
    { 0, 1, 1 },
    { 1, 0, 1 },
    { 1, 1, 0 },
};

const nn_sample *train = nn_or_samples;

float nn_cost_sg(float w1, float w2, float b) {
    float result = 0.0f;

    for (size_t i = 0; i < NN_TRAIN_SIZE; ++i) {
        float x1 = train[i][0];
        float x2 = train[i][1];
        float y = nn_sigmoid_f(x1 * w1 + x2 * w2 + b);
        float d = y - train[i][2];
        result += d * d;
    }

    result /= NN_TRAIN_SIZE;
    return result;
}

float nn_forward_sg(float w1, float w2, float b, float x1, float x2) {
    return nn_sigmoid_f(x1 * w1 + x2 * w2 + b);
}

void nn_learn_sg(float *w1, float *w2, float *b, float learning_rate) {
    for (size_t i = 0; i < NN_EPOCHS; ++i) {
        float c = nn_cost_sg(*w1, *w2, *b);
        float dw1 = (nn_cost_sg(*w1 + NN_EPS, *w2, *b) - c) / NN_EPS;
        float dw2 = (nn_cost_sg(*w1, *w2 + NN_EPS, *b) - c) / NN_EPS;
        float db = (nn_cost_sg(*w1, *w2, *b + NN_EPS) - c) / NN_EPS;
        *w1 -= learning_rate * dw1;
        *w2 -= learning_rate * dw2;
        *b -= learning_rate * db;
    }
}

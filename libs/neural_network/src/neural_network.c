#include "neural_network.h"

#include <math.h>
#include <stdlib.h>

float nn_sigmoid_f(float x) {
    return 1.0f / (1.0f + expf(-x));
}

float nn_rand_float(void) {
    return (float) rand() / (float) RAND_MAX;
}

#include "image_processing/image_processing.h"
#include "neural_network/neural_network.h"
#include "sudoku_solver/sudoku_solver.h"

#include <stdlib.h>

int main(void) {
    ip_say_hello();
    nn_say_hello();
    ss_say_hello();
    return EXIT_SUCCESS;
}

#ifndef OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H
#define OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H

#define NN_EPS 1e-3f

struct nn_model_xor {
    float or_w1;
    float or_w2;
    float or_b;

    float nand_w1;
    float nand_w2;
    float nand_b;

    float and_w1;
    float and_w2;
    float and_b;
};

float nn_sigmoid_f(float x);
float nn_rand_float(void);
struct nn_model_xor nn_rand_xor(void);
void nn_print_xor(const struct nn_model_xor *m);

float nn_cost_sg(float w1, float w2, float b);
float nn_forward_sg(float w1, float w2, float b, float x1, float x2);
void nn_learn_sg(float *w1, float *w2, float *b, float learning_rate);

float nn_cost_xor(const struct nn_model_xor *m);
float nn_forward_xor(const struct nn_model_xor *m, float x1, float x2);
void nn_learn_xor(struct nn_model_xor *m, float learning_rate);

#endif // OCR_SUDOKU_SOLVER_NEURAL_NETWORK_H

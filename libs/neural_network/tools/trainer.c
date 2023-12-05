#include <neural_network.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFAULT_LEARNING_RATE 0.1f
#define DEFAULT_ITERATIONS 11

static void print_usage(const char *program_name) {
    fprintf(
        stderr,
        "usage: %s dataset_path out_model_path [options]\n"
        "options:\n"
        "\t-i arg\tnumber of iterations to perform\n"
        "\t-f arg\tpath to an initial model to continue training\n"
        "\t-l arg\tlearning rate to use\n",
        program_name
    );
}

int main(int argc, char *argv[]) {
    const char *program_name = argv[0];

    const char *dataset_path = argv[1];
    const char *out_model_path = argv[2];

    const char *init_model_path = NULL;
    float learning_rate = DEFAULT_LEARNING_RATE;
    size_t iterations = DEFAULT_ITERATIONS;

    int opt;

    while ((opt = getopt(argc, argv, "i:f:l:")) != -1) {
        switch (opt) {
        case 'i': {
            unsigned long long value = strtoull(optarg, NULL, 10);

            if (value == 0 || errno) {
                fputs("error: invalid argument for -i\n", stderr);
                return EXIT_FAILURE;
            }

            iterations = value;
            break;
        }
        case 'f': init_model_path = optarg; break;
        case 'l': {
            float value = strtof(optarg, NULL);

            if (value == 0.0f || errno) {
                fputs("error: invalid argument for -l\n", stderr);
                return EXIT_FAILURE;
            }

            learning_rate = value;
            break;
        }
        default:
            fprintf(stderr, "error: unrecognized option '-%c'\n", opt);
            return EXIT_FAILURE;
        }
    }

    if (argc < 3) {
        print_usage(program_name);
        return EXIT_FAILURE;
    }

    struct nn_dataset dataset;

    if (!nn_dataset_load(&dataset, dataset_path)) {
        fputs("error: failed to load dataset\n", stderr);
        return EXIT_FAILURE;
    }

    struct nn_model model;

    if (!nn_model_init(&model)) {
        fputs("error: failed to init model\n", stderr);
        return EXIT_FAILURE;
    }

    if (init_model_path) {
        if (!nn_model_read(&model, init_model_path)) {
            fputs("error: failed to read previous model\n", stderr);
            return EXIT_FAILURE;
        }
    } else {
        nn_model_rand(&model);
    }

    struct nn_train_context ctx;

    if (!nn_train_context_init(&ctx, &model, &dataset)) {
        fputs("error: failed to init context\n", stderr);
        return EXIT_FAILURE;
    }

    nn_train(&ctx, &model, &dataset, learning_rate, iterations);

    if (!nn_model_write(&model, out_model_path)) {
        fputs("error: failed to write model\n", stderr);
        return EXIT_FAILURE;
    }

    nn_train_context_fini(&ctx);
    nn_dataset_fini(&dataset);
    nn_model_fini(&model);
    return EXIT_SUCCESS;
}

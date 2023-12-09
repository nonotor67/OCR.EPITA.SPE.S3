#include <image_processing.h>

#include <MagickWand/MagickWand.h>

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define LOG(fmt, ...)           \
    fprintf(                    \
        stderr,                 \
        "%s.%d: %s: " fmt "\n", \
        __FILE_NAME__,          \
        __LINE__,               \
        __func__,               \
        ##__VA_ARGS__           \
    )
#define LOGE(fmt, ...) LOG(fmt ": %s", ##__VA_ARGS__, strerror(errno))

static void print_usage(const char *program_name) {
    fprintf(stderr, "usage: %s input_dir_path output_path\n", program_name);
}

size_t written = 0;

static void make_dataset_sudoku(const char *image_path, FILE *output) {
    const char *image1 = {
        "53..7...."
        "6..195..."
        ".98....6."
        "8...6...3"
        "4..8.3..1"
        "7...2...6"
        ".6....28."
        "...419..5"
        "....8..79", //
    };
    const char *image2 = {
        ".2....6.9"
        "857.642.."
        ".9...1..."
        ".1.65.3.."
        "..81.35.."
        "..3.29.8."
        "...4...6."
        "..287.135"
        "1.6....2.", //
    };
    const char *image3 = {
        ".....458."
        "...721..3"
        "4.3......"
        "21..67..4"
        ".7....2.."
        "63..49..1"
        "3.6......"
        "...158..6"
        ".....695.", //
    };
    const char *image4 = {
        "7.89....2"
        "513..2..8"
        ".9231...7"
        ".5..3.9.."
        "16..2..75"
        "..9.4..6."
        "9...8421."
        "2..6..749"
        "4....15.3", //
    };
    const char *image5 = {
        "........."
        "6.......5"
        "9.53.84.6"
        ".1..4..7."
        "...1.3..."
        "2.......4"
        "4..529..1"
        "..6...5.."
        ".2.4.6.3.", //
    };
    const char *image6 = {
        ".961.854."
        "5.4.62387"
        "23.74.9.1"
        "643.7981."
        ".8.3.4679"
        "9.581.423"
        ".29.81..6"
        "8.75.3.94"
        "45.69723.", //
    };
    puts(image_path);

    float *normalized_pixels[9 * 9];

    if (!ip_process_image(
            normalized_pixels,
            image_path,
            "/tmp/zest.png.%zu%zu.png",
            0.0,
            NULL
        )) {
        LOGE("error: failed to process image");
        exit(EXIT_FAILURE);
    }

    float label = 0.0f;

    for (size_t i = 0; i < 9 * 9; i++) {
        char letter;
        if (strcmp(image_path, "image_01.jpeg") == 0) {
            letter = image1[i];
        } else if (strcmp(image_path, "image_02.jpeg") == 0) {
            letter = image2[i];
        } else if (strcmp(image_path, "image_03.jpeg") == 0) {
            letter = image3[i];
        } else if (strcmp(image_path, "image_04.jpeg") == 0) {
            letter = image4[i];
        } else if (strcmp(image_path, "image_05.jpeg") == 0) {
            letter = image5[i];
        } else if (strcmp(image_path, "image_06.jpeg") == 0) {
            letter = image6[i];
        } else {
            //            puts("skipping unknown image");
            continue;
        }
        if (letter == '.') {
            //            puts("skipping blank cell");
            continue;
        }
        label = (float) (letter - '1');
        if (fwrite(&label, sizeof(label), 1, output) < 1) {
            LOGE("error: failed to write label to file");
            exit(EXIT_FAILURE);
        }

        if (fwrite(normalized_pixels[i], sizeof(float), 28 * 28, output) <
            28 * 28) {
            LOGE("error: failed to write normalized pixels");
            exit(EXIT_FAILURE);
        }
        written++;
    }
}

static void make_dataset(const char *input_dir_path, const char *output_path) {
    FILE *output = fopen(output_path, "w+b");

    if (!output) {
        LOGE("error: failed to open output file");
        exit(EXIT_FAILURE);
    }

    DIR *input_dir = opendir(input_dir_path);

    if (!input_dir) {
        LOGE("error: failed to open input directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;

    while ((entry = readdir(input_dir))) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        make_dataset_sudoku(entry->d_name, output);
    }
    printf("written: %zu\n", written);

    closedir(input_dir);
    fclose(output);
}

int main(int argc, const char *argv[]) {
    const char *program_name = argv[0];

    if (argc < 3) {
        print_usage(program_name);
        return EXIT_FAILURE;
    }

    const char *input_dir_path = argv[1];
    const char *output_path = argv[2];

    MagickWandGenesis();

    make_dataset(input_dir_path, output_path);

    MagickWandTerminus();

    return EXIT_SUCCESS;
}

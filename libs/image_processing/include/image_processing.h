#ifndef OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H
#define OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

#include <stdbool.h>

bool ip_process_image(
    const char *image_path,
    const char *cell_path_fmt,
    double rotate_degrees
);

#endif // OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

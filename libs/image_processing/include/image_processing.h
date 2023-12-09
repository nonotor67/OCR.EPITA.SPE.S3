#ifndef OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H
#define OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

#include <stdbool.h>

#define IP_STEP_1_PATH "/tmp/ocr-sudoku-solver.ip.step1.png"
#define IP_STEP_2_PATH "/tmp/ocr-sudoku-solver.ip.step2.png"
#define IP_STEP_3_PATH "/tmp/ocr-sudoku-solver.ip.step3.png"

bool ip_rotate_image(
    const char *src_path,
    double degrees,
    const char *dst_path
);

/// Reads an image and extracts sudoku cells from it.
/// \param src_path Path to source image file.
/// \return True on success, or false on failure.
bool ip_process_image(const char *src_path, float *dst_pixels[9 * 9]);

#endif // OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

#ifndef OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H
#define OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

#include <stdbool.h>

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

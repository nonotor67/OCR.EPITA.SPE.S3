#ifndef OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H
#define OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

#include <stdbool.h>

bool ip_rotate_image(
    const char *src_path,
    double degrees,
    const char *dst_path
);

/// Reads an image and extracts sudoku cells from it.
/// \param image_path Path to source image file.
/// \param cell_path_fmt Format of paths to sudoku cell images to write.
/// \return True on success, or false on failure.
bool ip_process_image(const char *image_path, const char *cell_path_fmt);

#endif // OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

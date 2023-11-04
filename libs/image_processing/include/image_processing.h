#ifndef OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H
#define OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

#include <stdbool.h>

/// Reads an image and extracts sudoku cells from it.
/// \param image_path Path to source image file.
/// \param cell_path_fmt Format of paths to sudoku cell images to write.
/// \param rotate_degrees Number of degrees to rotate the source image by before
/// actual processing.
/// \return True on success, or false on failure.
bool ip_process_image(
    const char *image_path,
    const char *cell_path_fmt,
    double rotate_degrees
);

#endif // OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

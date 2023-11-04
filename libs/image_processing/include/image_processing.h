#ifndef OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H
#define OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

#include <stdbool.h>

void ip_say_hello(void);

bool ip_process_image(const char *image_path, const char *cell_path_fmt);

#endif // OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

#ifndef OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H
#define OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Surface *load_image(const char *path);

Uint32 getpixel(SDL_Surface *surface, int x, int y);
unsigned long int GetColorPixel(Uint32 pixel, SDL_PixelFormat *format);
void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

void surface_to_grayscale(SDL_Surface *surface);

void Sauvola_Thresholding(SDL_Surface *image);

void wipe_dots(SDL_Surface *surface);

#endif // OCR_SUDOKU_SOLVER_IMAGE_PROCESSING_H

#include "image_processing.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
// path: Path of the image.
SDL_Surface *load_image(const char *path) {
    SDL_Surface *temp = IMG_Load(path);
    if (!temp)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface *copycat =
        SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGB888, 0);
    if (!copycat)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_FreeSurface(temp);
    return copycat;
}

// return the pixel in position x,y in a surface
//
// x:the x position of the pixel
// y:the y position of the pixel
// surface:surface:the surface where to get the pixel
Uint32 getpixel(SDL_Surface *surface, int x, int y) {
    int bytes_pixel = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bytes_pixel;

    switch (bytes_pixel) {
    case 1: return *p; break;

    case 2: return *(Uint16 *) p; break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4: return *(Uint32 *) p; break;

    default: return 0; /* shouldn't happen, but avoids warnings */
    }
}

// return the red color of a pixel
// format:the format of pixel
// pixel:the pixel whose color we want
unsigned long int GetColorPixel(Uint32 pixel, SDL_PixelFormat *format) {
    Uint8 r, g, b;
    SDL_GetRGB(pixel, format, &r, &g, &b);
    return (unsigned long int) r;
}

// Put a pixel in a surface
//
// x:the x position of the pixel
// y:the y position of the pixel
// surface:the surface where to put the pixel
// pixel:the pixel we want to put on the surface
void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel) {
    int bytes_pixel = surface->format->BytesPerPixel;
    // Here p is the address to the pixel we want to retrieve
    Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bytes_pixel;
    switch (bytes_pixel) {
    case 1: *p = pixel; break;

    case 2: *(Uint16 *) p = pixel; break;

    case 3:
        if (SDL_BYTEORDER == SDL_LIL_ENDIAN) {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        } else {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        break;

    case 4: *(Uint32 *) p = pixel; break;
    }
}

Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat *format) {
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    Uint8 average = 0.3 * r + 0.59 * g + 0.11 * b;
    Uint32 color = SDL_MapRGB(format, average, average, average);
    return color;
}

void surface_to_grayscale(SDL_Surface *surface) {
    Uint32 *pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat *format = surface->format;
    SDL_LockSurface(surface);
    for (int count = 0; count < len; count++)
        pixels[count] = pixel_to_grayscale(pixels[count], format);
    SDL_UnlockSurface(surface);
}

// apply a threshold to a specific pixel of an image
// x:the x position of the pixel
// y:the y position of the pixel
// surface:surface:the surface where to apply the threshold
// format:the format of pixel of the surface image
// threshold:the value of the threshold we want to apply
void apply_threshold(
    SDL_Surface *image,
    int x,
    int y,
    Uint8 threshold,
    SDL_PixelFormat *format
) {
    Uint8 r, g, b;
    SDL_GetRGB(getpixel(image, x, y), format, &r, &g, &b);
    if (r > threshold)
        r = g = b = 255;
    else
        r = g = b = 0;
    Uint32 color = SDL_MapRGB(format, r, g, b);
    put_pixel(image, x, y, color);
}

// calculate the integral image of the surface image
unsigned long int **IntegralImage(SDL_Surface *image) {
    size_t w = image->w;
    size_t h = image->h;
    unsigned long int **result = malloc(w * sizeof(unsigned long int *));
    for (size_t i = 0; i < w; i++)
        result[i] = calloc(h, sizeof(unsigned long int));

    SDL_PixelFormat *format = image->format;
    result[0][0] = GetColorPixel(getpixel(image, 0, 0), format);
    for (size_t y = 1; y < h; y++)
        result[0][y] =
            result[0][y - 1] + GetColorPixel(getpixel(image, 0, y), format);
    for (size_t x = 1; x < w; x++)
        result[x][0] =
            result[x - 1][0] + GetColorPixel(getpixel(image, x, 0), format);

    for (size_t x = 1; x < w; x++)
        for (size_t y = 1; y < h; y++)
            result[x][y] = result[x - 1][y] + result[x][y - 1] -
                result[x - 1][y - 1] +
                GetColorPixel(getpixel(image, x, y), format);
    return result;
}

// calculate the integral image squared of the surface image
unsigned long int **IntegralImageSquared(SDL_Surface *image) {
    size_t w = image->w;
    size_t h = image->h;
    unsigned long int **result = malloc(w * sizeof(unsigned long int *));
    for (size_t i = 0; i < w; i++)
        result[i] = calloc(h, sizeof(unsigned long int));

    SDL_PixelFormat *format = image->format;
    unsigned long r = GetColorPixel(getpixel(image, 0, 0), format);
    result[0][0] = r * r;
    for (size_t y = 1; y < h; y++) {
        r = GetColorPixel(getpixel(image, 0, y), format);
        result[0][y] = result[0][y - 1] + r * r;
    }
    for (size_t x = 1; x < w; x++) {
        r = GetColorPixel(getpixel(image, x, 0), format);
        result[x][0] = result[x - 1][0] + r * r;
    }
    for (size_t x = 1; x < w; x++)
        for (size_t y = 1; y < h; y++) {
            r = GetColorPixel(getpixel(image, x, y), format);
            result[x][y] = result[x - 1][y] + result[x][y - 1] -
                result[x - 1][y - 1] + r * r;
        }
    return result;
}

// calculate the mean and the variance in a certain for every pixel of an image
// mean:an arrays that contains the mean of every pixel of the surface image
// variance:an arrays that contains the variance of every pixel of the surface
// image minv:pointers that stock the minimum variance maxv:pointers that stock
// the maximum variance
float mean_variance(
    float **mean,
    float **variance,
    SDL_Surface *image,
    float *minv,
    float *maxv
) {
    size_t w = image->w - 1;
    size_t h = image->h - 1;
    unsigned long int **integral = IntegralImage(image);
    unsigned long int **integral_squared = IntegralImageSquared(image);

    for (size_t x = 0; x < w + 1; x++) {
        for (size_t y = 0; y < h + 1; y++) {
            float xmin = 0;
            float xmax = w;
            float ymin = 0;
            float ymax = h;
            if ((int) x - 10 >= 0)
                xmin = (float) (x - 10);
            if (x + 10 <= w)
                xmax = (float) (x + 10);
            if ((int) y - 10 >= 0)
                ymin = (float) (y - 10);
            if (y + 10 <= h)
                ymax = (float) (y + 10);

            mean[x][y] = (float) (integral[(int) xmax][(int) ymax] +
                                  integral[(int) xmin][(int) ymin] -
                                  integral[(int) xmin][(int) ymax] -
                                  integral[(int) xmax][(int) ymin]) /
                ((xmax - xmin) * (ymax - ymin));
            variance[x][y] =
                (float) (integral_squared[(int) xmax][(int) ymax] +
                         integral_squared[(int) xmin][(int) ymin] -
                         integral_squared[(int) xmin][(int) ymax] -
                         integral_squared[(int) xmax][(int) ymin]) /
                ((xmax - xmin) * (ymax - ymin));
            variance[x][y] = sqrt((variance[x][y] - mean[x][y] * mean[x][y]));
            if (variance[x][y] < *minv)
                *minv = variance[x][y];
            if (variance[x][y] > *maxv)
                *maxv = variance[x][y];
        }
    }
    float res = (float) (integral[w][h] + integral[0][0] - integral[w][0] -
                         integral[0][h]) /
        (float) (w * h);
    for (size_t i = 0; i < w + 1; i++)
        free(integral[i]);
    free(integral);
    for (size_t i = 0; i < w + 1; i++)
        free(integral_squared[i]);
    free(integral_squared);
    return res;
}

// apply the sauvola thresholding method to the surface image
void Sauvola_Thresholding(SDL_Surface *image) {
    size_t w = image->w - 1;
    size_t h = image->h - 1;
    unsigned long int **integral = IntegralImage(image);
    unsigned long int **integral_squared = IntegralImageSquared(image);
    SDL_PixelFormat *format = image->format;
    SDL_LockSurface(image);
    for (size_t x = 0; x < w; x++) {
        for (size_t y = 0; y < h; y++) {
            float xmin = 0;
            float xmax = w;
            float ymin = 0;
            float ymax = h;
            if ((int) x - 10 >= 0)
                xmin = (float) (x - 10);
            if (x + 10 <= w)
                xmax = (float) (x + 10);
            if ((int) y - 10 >= 0)
                ymin = (float) (y - 10);
            if (y + 10 <= h)
                ymax = (float) (y + 10);

            float mean = (float) (integral[(int) xmax][(int) ymax] +
                                  integral[(int) xmin][(int) ymin] -
                                  integral[(int) xmin][(int) ymax] -
                                  integral[(int) xmax][(int) ymin]) /
                ((xmax - xmin) * (ymax - ymin));
            float variance =
                (float) (integral_squared[(int) xmax][(int) ymax] +
                         integral_squared[(int) xmin][(int) ymin] -
                         integral_squared[(int) xmin][(int) ymax] -
                         integral_squared[(int) xmax][(int) ymin]) /
                ((xmax - xmin) * (ymax - ymin));

            variance = sqrt((variance - mean * mean));

            unsigned long int threshold =
                mean * (1 + 0.2 * (float) (variance / 128 - 1));

            apply_threshold(image, x, y, threshold, format);
        }
    }
    for (size_t i = 0; i < w + 1; i++)
        free(integral[i]);
    free(integral);
    for (size_t i = 0; i < w + 1; i++)
        free(integral_squared[i]);
    free(integral_squared);
    SDL_UnlockSurface(image);
}

// wipes dots
void wipe_dots(SDL_Surface *surface) {
    unsigned int MAX_W = surface->w;
    unsigned int MAX_H = surface->h;
    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;
    Uint8 r1, r1r, r1d, r1l, r1h, g1, b1, g, b;

    SDL_LockSurface(surface);
    for (unsigned int x = 0; x < MAX_H; x++) {
        for (unsigned int y = 0; y < MAX_W; y++) {
            // initialise number of neighbours and fills the reds of neighbours
            // if it is a corner or a border
            int neighbours = 0;
            r1r = 255;
            r1d = 255;
            r1l = 255;
            r1h = 255;

            // gets the red values of the pixels (green and blue do not matter
            // since the image is in black and white)
            SDL_GetRGB(pixels[x * MAX_W + y], format, &r1, &g1, &b1);
            if (x + 1 < MAX_H)
                SDL_GetRGB(pixels[(x + 1) * MAX_W + y], format, &r1r, &g, &b);
            if (y + 1 < MAX_W)
                SDL_GetRGB(pixels[x * MAX_W + y + 1], format, &r1d, &g, &b);
            if (x > 0)
                SDL_GetRGB(pixels[(x - 1) * MAX_W + y], format, &r1l, &g, &b);
            if (y > 0)
                SDL_GetRGB(pixels[x * MAX_W + y - 1], format, &r1h, &g, &b);

            // increments the number of neighbours that are white
            if (r1r == 255)
                neighbours++;
            if (r1l == 255)
                neighbours++;
            if (r1h == 255)
                neighbours++;
            if (r1d == 255)
                neighbours++;

            // changes the pixel to white if it is black and 3 neighbours are
            // white or if it is part of a line
            if (r1 == 0 &&
                ((r1r == r1l && r1d == r1h &&
                  (r1r == 255 || r1d == 255 || (r1r == 255 && r1r == r1d))) ||
                 neighbours > 2))
                pixels[x * 28 + y] = SDL_MapRGB(format, 255, 255, 255);
        }
    }
    SDL_UnlockSurface(surface);
}

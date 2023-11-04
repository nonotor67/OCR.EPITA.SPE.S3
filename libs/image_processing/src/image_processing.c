#include "image_processing.h"

#include <MagickWand/MagickWand.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define IP_MIN(a, b) ((a) < (b) ? (a) : (b))
#define IP_MAX(a, b) ((a) > (b) ? (a) : (b))

struct ip_point {
    ssize_t x;
    ssize_t y;
};

#define IP_NUM_DIRECTIONS 8

const struct ip_point IP_DIRECTIONS[IP_NUM_DIRECTIONS] = {
    { .x = -1, .y = -1 }, // Northwest
    { .x = 0, .y = -1 }, // North
    { .x = 1, .y = -1 }, // Northeast
    { .x = -1, .y = 0 }, // West
    { .x = 1, .y = 0 }, // East
    { .x = -1, .y = 1 }, // Southwest
    { .x = 0, .y = 1 }, // South
    { .x = 1, .y = 1 }, // Southeast
};

struct ip_component {
    struct ip_point top_left;
    struct ip_point top_right;
    struct ip_point bot_left;
    struct ip_point bot_right;
};

struct ip_point_list {
    size_t len;
    size_t cap;
    struct ip_point *data;
};

static bool ip_point_list_init(struct ip_point_list *list) {
    list->len = 0;
    list->cap = 16;
    list->data = reallocarray(NULL, list->cap, sizeof(struct ip_point));
    return list->data != NULL;
}

static void ip_point_list_fini(struct ip_point_list *list) {
    free(list->data);
}

static bool
ip_point_list_push(struct ip_point_list *list, struct ip_point element) {
    if (list->len == list->cap) {
        size_t new_cap = list->cap *= 2;
        void *new_data =
            reallocarray(list->data, list->cap, sizeof(struct ip_point));

        if (!new_data) {
            return false;
        }

        list->cap = new_cap;
        list->data = new_data;
    }

    list->data[list->len++] = element;
    return true;
}

static struct ip_point ip_point_list_pop(struct ip_point_list *list) {
    return list->data[--list->len];
}

static bool ip_find_component(
    struct ip_component *dest,
    const unsigned char *pixels,
    struct ip_point size,
    struct ip_point start,
    bool *visited,
    struct ip_point_list *to_visit
) {
    if (!ip_point_list_push(to_visit, start)) {
        return false;
    }

    dest->top_left = start;
    dest->top_right = start;
    dest->bot_left = start;
    dest->bot_right = start;

    while (to_visit->len > 0) {
        struct ip_point pos = ip_point_list_pop(to_visit);

        if (pos.x < 0 || pos.x >= size.x || pos.y < 0 || pos.y >= size.y) {
            continue;
        }

        size_t pixel_index = pos.y * size.x + pos.x;

        if (visited[pixel_index]) {
            continue;
        }

        visited[pixel_index] = true;

        if (pixels[pixel_index] == 0) {
            continue;
        }

        if (pos.x + pos.y < dest->top_left.x + dest->top_left.y) {
            dest->top_left = pos;
        }

        if (pos.x - pos.y > dest->top_right.x - dest->top_right.y) {
            dest->top_right = pos;
        }

        if (pos.x - pos.y < dest->bot_left.x - dest->bot_left.y) {
            dest->bot_left = pos;
        }

        if (pos.x + pos.y > dest->bot_right.x + dest->bot_right.y) {
            dest->bot_right = pos;
        }

        for (size_t i = 0; i < IP_NUM_DIRECTIONS; i++) {
            struct ip_point next_pos = {
                .x = pos.x + IP_DIRECTIONS[i].x,
                .y = pos.y + IP_DIRECTIONS[i].y,
            };

            if (!ip_point_list_push(to_visit, next_pos)) {
                return false;
            }
        }
    }

    return true;
}

static bool ip_find_grid(
    struct ip_component *dest,
    struct ip_point *min_dest,
    struct ip_point *max_dest,
    const unsigned char *pixels,
    struct ip_point size
) {
    bool *visited = calloc(size.x * size.y, sizeof(bool));

    if (!visited) {
        return false;
    }

    struct ip_point_list to_visit;

    if (!ip_point_list_init(&to_visit)) {
        free(visited);
        return false;
    }

    struct ip_component grid;
    ssize_t grid_size = -1;
    struct ip_point grid_min;
    struct ip_point grid_max;

    for (ssize_t y = 0; y < size.y; y++) {
        for (ssize_t x = 0; x < size.x; x++) {
            if (visited[y * size.x + x] || pixels[y * size.x + x] == 0) {
                continue;
            }

            struct ip_point start = { .x = x, .y = y };
            struct ip_component comp;

            bool ok = ip_find_component(
                &comp,
                pixels,
                size,
                start,
                visited,
                &to_visit
            );

            if (!ok) {
                ip_point_list_fini(&to_visit);
                free(visited);
                return false;
            }

            struct ip_point comp_min = {
                .x = IP_MIN(comp.top_left.x, comp.bot_left.x),
                .y = IP_MIN(comp.top_left.y, comp.top_right.y),
            };
            struct ip_point comp_max = {
                .x = IP_MAX(comp.top_right.x, comp.bot_right.x),
                .y = IP_MAX(comp.bot_left.y, comp.bot_right.y),
            };

            if (comp_min.x == 0 || comp_min.y == 0 ||
                comp_max.x == size.x - 1 || comp_max.y == size.y - 1) {
                continue;
            }

            ssize_t comp_size =
                (comp_max.x - comp_min.x) * (comp_max.y - comp_min.y);

            if (comp_size > grid_size) {
                grid = comp;
                grid_size = comp_size;
                grid_min = comp_min;
                grid_max = comp_max;
            }
        }
    }

    ip_point_list_fini(&to_visit);
    free(visited);

    if (grid_size < 0) {
        return false;
    }

    *dest = grid;
    *min_dest = grid_min;
    *max_dest = grid_max;
    return true;
}

bool ip_process_image(
    const char *image_path,
    const char *cell_path_fmt,
    double rotate_degrees
) {
    MagickWand *wand = NewMagickWand();

    if (MagickReadImage(wand, image_path) == MagickFalse) {
        DestroyMagickWand(wand);
        return false;
    }

    if (fabs(rotate_degrees) >= DBL_EPSILON) {
        PixelWand *pixel_wand = NewPixelWand();
        PixelSetColor(pixel_wand, "white");

        if (MagickRotateImage(wand, pixel_wand, rotate_degrees) ==
            MagickFalse) {
            DestroyPixelWand(pixel_wand);
            DestroyMagickWand(wand);
            return false;
        }

        DestroyPixelWand(pixel_wand);
    }

    if (MagickCannyEdgeImage(wand, 7.0, 3.0, 0.3, 0.4) == MagickFalse) {
        DestroyMagickWand(wand);
        return false;
    }

    ExceptionInfo *exception = AcquireExceptionInfo();

    KernelInfo *kernel = AcquireKernelInfo("Square:5x5", exception);

    if (exception->severity != UndefinedException) {
        CatchException(exception);
        exception = DestroyExceptionInfo(exception);
        DestroyMagickWand(wand);
        return false;
    }

    exception = DestroyExceptionInfo(exception);

    if (MagickMorphologyImage(wand, DilateMorphology, 1, kernel) ==
        MagickFalse) {
        DestroyKernelInfo(kernel);
        DestroyMagickWand(wand);
        return false;
    }

    DestroyKernelInfo(kernel);

    size_t w = MagickGetImageWidth(wand);
    size_t h = MagickGetImageHeight(wand);

    unsigned char *pixels = malloc(w * h);

    if (!pixels) {
        DestroyMagickWand(wand);
        return false;
    }

    if (MagickExportImagePixels(wand, 0, 0, w, h, "I", CharPixel, pixels) ==
        MagickFalse) {
        free(pixels);
        DestroyMagickWand(wand);
        return false;
    }

    struct ip_component grid;
    struct ip_point grid_min;
    struct ip_point grid_max;
    struct ip_point size = { .x = (ssize_t) w, .y = (ssize_t) h };

    if (!ip_find_grid(&grid, &grid_min, &grid_max, pixels, size)) {
        DestroyMagickWand(wand);
        return false;
    }

    free(pixels);

    double distort_args[16] = {
        (double) grid.top_left.x,  (double) grid.top_left.y,
        (double) grid_min.x,       (double) grid_min.y,
        (double) grid.top_right.x, (double) grid.top_right.y,
        (double) grid_max.x,       (double) grid_min.y,
        (double) grid.bot_left.x,  (double) grid.bot_left.y,
        (double) grid_min.x,       (double) grid_max.y,
        (double) grid.bot_right.x, (double) grid.bot_right.y,
        (double) grid_max.x,       (double) grid_max.y,
    };

    wand = DestroyMagickWand(wand);
    wand = NewMagickWand();

    if (MagickReadImage(wand, image_path) == MagickFalse) {
        DestroyMagickWand(wand);
        return false;
    }

    if (fabs(rotate_degrees) >= DBL_EPSILON) {
        PixelWand *pixel_wand = NewPixelWand();
        PixelSetColor(pixel_wand, "white");

        if (MagickRotateImage(wand, pixel_wand, rotate_degrees) ==
            MagickFalse) {
            DestroyPixelWand(pixel_wand);
            DestroyMagickWand(wand);
            return false;
        }

        DestroyPixelWand(pixel_wand);
    }

    bool result = MagickDistortImage(
        wand,
        PerspectiveDistortion,
        16,
        distort_args,
        MagickFalse
    );

    if (!result) {
        DestroyMagickWand(wand);
        return false;
    }

    ssize_t cw = (grid_max.x - grid_min.x) / 9;
    ssize_t ch = (grid_max.y - grid_min.y) / 9;

    for (ssize_t y = 0; y < 9; y++) {
        for (ssize_t x = 0; x < 9; x++) {
            char *cell_path;
            asprintf(&cell_path, cell_path_fmt, x, y);

            MagickWand *cell_wand = NewMagickWand();
            ssize_t cx = x * cw + grid_min.x;
            ssize_t cy = y * ch + grid_min.y;

            if (MagickAddImage(cell_wand, wand) &&
                MagickCropImage(cell_wand, cw, ch, cx, cy) &&
                MagickWriteImage(cell_wand, cell_path) == MagickFalse) {
                free(cell_path);
                DestroyMagickWand(cell_wand);
                DestroyMagickWand(wand);
                return false;
            }

            free(cell_path);
            DestroyMagickWand(cell_wand);
        }
    }

    DestroyMagickWand(wand);
    return true;
}

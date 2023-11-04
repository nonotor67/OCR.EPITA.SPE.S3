#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <gtk/gtk.h>

#include <stdbool.h>

#define GRID_SIZE 9

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 400

GtkWidget *input_image;
GtkWidget *output_image;

bool output_generated = false;

// Example sudoku to test output image generation.
const int sudoku[GRID_SIZE * GRID_SIZE] = {
    5, 3, 2, 0, 7, 0, 0, 0, 0, // Row 0
    6, 0, 0, 1, 9, 5, 0, 0, 0, // Row 1
    0, 9, 8, 6, 6, 6, 0, 6, 0, // Row 2
    8, 0, 0, 0, 6, 0, 0, 0, 3, // Row 3
    4, 0, 0, 8, 0, 3, 0, 0, 1, // Row 4
    7, 0, 0, 0, 2, 0, 0, 0, 6, // Row 5
    0, 6, 0, 0, 0, 0, 2, 8, 0, // Row 6
    0, 0, 0, 4, 1, 9, 0, 0, 5, // Row 7
    0, 0, 0, 0, 8, 0, 0, 7, 9, // Row 8
};

void draw_sudoku(SDL_Renderer *renderer, const int *arr) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int cellWidth = SCREEN_WIDTH / GRID_SIZE;
    int cellHeight = SCREEN_HEIGHT / GRID_SIZE;
    int fontSize = cellHeight / 2;

    TTF_Font *resizedFont = TTF_OpenFont("./Roboto-Regular.ttf", fontSize);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int x = 0; x < SCREEN_WIDTH; x += cellWidth) {
        SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
    }

    for (int y = 0; y < SCREEN_HEIGHT; y += cellHeight) {
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        int row = i / GRID_SIZE;
        int col = i % GRID_SIZE;
        int number = arr[i];

        if (number == 0) {
            continue;
        }

        char text[2];
        snprintf(text, 2, "%d", number);

        SDL_Color text_color = { 255, 255, 255, 255 };
        SDL_Surface *surface =
            TTF_RenderText_Solid(resizedFont, text, text_color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect text_rect = {
            col * cellWidth + cellWidth / 2 - fontSize / 2,
            row * cellHeight + cellHeight / 2 - fontSize / 2,
            fontSize,
            fontSize,
        };

        if (SDL_RenderCopy(renderer, texture, NULL, &text_rect) < 0) {
            printf("SDL_RenderCopy failed: %s\n", SDL_GetError());
        }

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    TTF_CloseFont(resizedFont);
    SDL_RenderPresent(renderer);
}

void create_and_save_output(char *path, const int *arr) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Sudoku Generation",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Init();

    draw_sudoku(renderer, arr);

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_TARGET,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
    SDL_SetRenderTarget(renderer, texture);
    draw_sudoku(renderer, arr);
    SDL_Surface *optimized_surface =
        SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(
        renderer,
        NULL,
        SDL_PIXELFORMAT_ARGB8888,
        optimized_surface->pixels,
        optimized_surface->pitch
    );
    SDL_SetRenderTarget(renderer, NULL);

    char *file_path;
    asprintf(&file_path, "%s%s", path, "ocr-sudoku.png");
    IMG_SavePNG(optimized_surface, file_path);
    free(file_path);

    SDL_FreeSurface(optimized_surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
}

static void choose_image_button_clicked(void) {
    output_generated = false;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Choose Image",
        NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancel",
        GTK_RESPONSE_CANCEL,
        "Open",
        GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), FALSE);
    gtk_file_chooser_set_current_folder(
        GTK_FILE_CHOOSER(dialog),
        g_get_home_dir()
    );

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename =
            gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_image_set_from_file(GTK_IMAGE(input_image), filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void generate_output_button_clicked(void) {
    output_generated = true;

    create_and_save_output("/tmp/", sudoku);
    gtk_image_set_from_file(GTK_IMAGE(output_image), "/tmp/ocr-sudoku.png");
}

static void download_output_button_clicked(void) {
    if (!output_generated) {
        return;
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Choose Image",
        NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancel",
        GTK_RESPONSE_CANCEL,
        "Open",
        GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), FALSE);
    gtk_file_chooser_set_current_folder(
        GTK_FILE_CHOOSER(dialog),
        g_get_home_dir()
    );

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *folder_path =
            gtk_file_chooser_get_current_folder((GtkFileChooser *) dialog);
        char *file_path;
        asprintf(&file_path, "%s%s", folder_path, "/");
        free(folder_path);
        create_and_save_output(file_path, sudoku);
        free(file_path);
        unlink("/tmp/ocr-sudoku.png");
    }

    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "ClosedAI|OCR");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scrolled_window),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_AUTOMATIC
    );
    gtk_container_add(GTK_CONTAINER(scrolled_window), main_box);

    gtk_container_add(GTK_CONTAINER(window), scrolled_window);

    input_image = gtk_image_new();
    output_image = gtk_image_new();
    GtkWidget *choose_image_button = gtk_button_new_with_label("Choose Image");
    g_signal_connect(
        choose_image_button,
        "clicked",
        G_CALLBACK(choose_image_button_clicked),
        NULL
    );

    GtkWidget *generate_output_button =
        gtk_button_new_with_label("Generate Output");
    g_signal_connect(
        generate_output_button,
        "clicked",
        G_CALLBACK(generate_output_button_clicked),
        NULL
    );

    GtkWidget *save_output_button = gtk_button_new_with_label("Save Output");
    g_signal_connect(
        save_output_button,
        "clicked",
        G_CALLBACK(download_output_button_clicked),
        NULL
    );

    gtk_box_pack_start(GTK_BOX(main_box), choose_image_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), input_image, TRUE, TRUE, 0);
    gtk_box_pack_start(
        GTK_BOX(main_box),
        generate_output_button,
        TRUE,
        TRUE,
        0
    );
    gtk_box_pack_start(GTK_BOX(main_box), output_image, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), save_output_button, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    return EXIT_SUCCESS;
}

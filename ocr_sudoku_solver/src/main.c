#include <image_processing.h>
#include <neural_network.h>
#include <sudoku_solver.h>

#include <MagickWand/MagickWand.h>
#include <gtk/gtk.h>

#define ROTATED_IMAGE_PATH "/tmp/ocr-sudoku-solver.rotated_image.png"
#define OUTPUT_IMAGE_PATH "/tmp/ocr-sudoku-solver.output_image.png"

#define OUTPUT_FONT_PATH "Roboto-Regular.ttf"
#define OUTPUT_IMAGE_WIDTH 1000

GtkWindow *window = NULL;

GtkImage *output_image = NULL;

GtkFileChooserButton *input_image = NULL;
GtkSpinButton *input_rotation = NULL;
GtkButton *input_solve = NULL;
GtkButton *input_save = NULL;

struct nn_model model;

static void set_image_from_file(const char *filepath) {
    if (!filepath) {
        return;
    }

    double degrees = gtk_spin_button_get_value(input_rotation);

    if (!ip_rotate_image(filepath, degrees, ROTATED_IMAGE_PATH)) {
        fputs("error: failed to rotate image\n", stderr);
        exit(EXIT_FAILURE);
    }

    GtkWidget *output_box = gtk_widget_get_parent(GTK_WIDGET(output_image));
    int width = gtk_widget_get_allocated_width(output_box);
    int height = gtk_widget_get_allocated_height(output_box);

    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(
        ROTATED_IMAGE_PATH,
        width,
        height,
        TRUE,
        &error
    );

    if (!pixbuf) {
        fputs("error: failed to create pixbuf\n", stderr);
        exit(EXIT_FAILURE);
    }

    gtk_image_set_from_pixbuf(output_image, pixbuf);
}

static void on_input_image_file_set(
    __attribute__((unused)) void *widget,
    __attribute__((unused)) gpointer user_data
) {
    gchar *filename =
        gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(input_image));
    set_image_from_file(filename);
}

static void draw_sudoku(const ss_grid sudoku) {
    MagickWand *wand = NewMagickWand();

    MagickSetSize(wand, OUTPUT_IMAGE_WIDTH, OUTPUT_IMAGE_WIDTH);
    PixelWand *background = NewPixelWand();
    PixelSetColor(background, "white");
    MagickNewImage(wand, OUTPUT_IMAGE_WIDTH, OUTPUT_IMAGE_WIDTH, background);
    DestroyPixelWand(background);

    DrawingWand *drawing_wand = NewDrawingWand();
    PixelWand *stroke_color = NewPixelWand();
    PixelSetColor(stroke_color, "black");
    DrawSetStrokeColor(drawing_wand, stroke_color);
    DrawSetStrokeWidth(drawing_wand, 1);
    DrawSetStrokeAntialias(drawing_wand, MagickTrue);

    if (DrawSetFont(drawing_wand, OUTPUT_FONT_PATH) == MagickFalse) {
        fputs("warning: failed to set font\n", stderr);
    }

    double font_size = 60.0;
    int cell_width = OUTPUT_IMAGE_WIDTH / 9;
    int cell_height = OUTPUT_IMAGE_WIDTH / 9;

    DrawSetFontSize(drawing_wand, font_size);

    for (int x = 0; x < OUTPUT_IMAGE_WIDTH; x += cell_width) {
        DrawLine(drawing_wand, x, 0, x, OUTPUT_IMAGE_WIDTH);
    }

    for (int y = 0; y < OUTPUT_IMAGE_WIDTH; y += cell_height) {
        DrawLine(drawing_wand, 0, y, OUTPUT_IMAGE_WIDTH, y);
    }

    for (int i = 0; i < 9 * 9; i++) {
        int row = i / 9;
        int col = i % 9;
        int number = sudoku[i];

        if (number == 0) {
            fputs("warning: empty cell\n", stderr);
            continue;
        }

        char text[2];
        snprintf(text, 2, "%d", number);

        DrawSetFillColor(drawing_wand, stroke_color);
        DrawAnnotation(
            drawing_wand,
            col * cell_width + cell_width / 2 - font_size / 3,
            row * cell_height + cell_height / 2 + font_size / 3,
            text
        );
    }

    MagickDrawImage(wand, drawing_wand);

    if (MagickWriteImage(wand, OUTPUT_IMAGE_PATH) == MagickFalse) {
        fputs("error: failed to write output image\n", stderr);
    }

    DestroyDrawingWand(drawing_wand);
    DestroyPixelWand(stroke_color);
}

static void on_input_solve_clicked(
    __attribute__((unused)) void *widget,
    __attribute__((unused)) gpointer user_data
) {
    float *all_cells_pixels[9 * 9];

    if (!ip_process_image(ROTATED_IMAGE_PATH, all_cells_pixels)) {
        fputs("error: failed to process image\n", stderr);
        return;
    }

    struct nn_infer_context ctx;

    if (!nn_infer_context_init(&ctx, &model)) {
        puts("error: failed to init ctx");
        nn_model_fini(&model);
        exit(EXIT_FAILURE);
    }

    ss_grid sudoku = { 0 };

    for (size_t cell_index = 0; cell_index < 9 * 9; cell_index++) {
        float *cell_pixels = all_cells_pixels[cell_index];

        if (!cell_pixels) {
            // Empty cell.
            continue;
        }

        struct nn_array input = {
            .size = 28 * 28,
            .data = cell_pixels,
        };
        size_t label = nn_infer(&ctx, &model, input);

        sudoku[cell_index] = (int) (label + 1);
    }

    for (size_t y = 0; y < 9; y++) {
        for (size_t x = 0; x < 9; x++) {
            int value = sudoku[y * 9 + x];
            printf("%c", (char) (value == 0 ? '.' : value + '0'));
        }

        putchar('\n');
    }

    draw_sudoku(sudoku);
    set_image_from_file(OUTPUT_IMAGE_PATH);
}

static void on_input_save_clicked(
    __attribute__((unused)) void *widget,
    __attribute__((unused)) gpointer user_data
) {
    GtkWidget *save_dialog = gtk_file_chooser_dialog_new(
        "Save File",
        window,
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel",
        GTK_RESPONSE_CANCEL,
        "_Save",
        GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_file_chooser_set_do_overwrite_confirmation(
        GTK_FILE_CHOOSER(save_dialog),
        TRUE
    );
    gtk_file_chooser_set_current_name(
        GTK_FILE_CHOOSER(save_dialog),
        "Untitled.png"
    );

    gint response = gtk_dialog_run(GTK_DIALOG(save_dialog));

    if (response == GTK_RESPONSE_ACCEPT) {
        puts("TODO");
    }

    gtk_widget_destroy(GTK_WIDGET(save_dialog));
}

int main(
    __attribute__((unused)) int argc,
    __attribute__((unused)) char *argv[]
) {
    gtk_init(NULL, NULL);

    GtkBuilder *builder = gtk_builder_new();
    GError *error = NULL;

    if (gtk_builder_add_from_file(builder, "app.glade", &error) == 0) {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return EXIT_FAILURE;
    }

    // Gets the widgets.
    window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));

    output_image = GTK_IMAGE(gtk_builder_get_object(builder, "output-image"));

    input_image =
        GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "input-image"));
    input_rotation =
        GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "input-rotation"));
    input_solve = GTK_BUTTON(gtk_builder_get_object(builder, "input-solve"));
    input_save = GTK_BUTTON(gtk_builder_get_object(builder, "input-save"));

    // Connects signal handlers.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(
        input_image,
        "file-set",
        G_CALLBACK(on_input_image_file_set),
        NULL
    );
    g_signal_connect(
        input_rotation,
        "value-changed",
        G_CALLBACK(on_input_image_file_set),
        NULL
    );
    g_signal_connect(
        input_solve,
        "clicked",
        G_CALLBACK(on_input_solve_clicked),
        NULL
    );
    g_signal_connect(
        input_save,
        "clicked",
        G_CALLBACK(on_input_save_clicked),
        NULL
    );

    if (!nn_model_init(&model)) {
        fputs("error: failed to init model\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (!nn_model_read(&model, "model.bin")) {
        fputs("error: failed to read model\n", stderr);
        nn_model_fini(&model);
        exit(EXIT_FAILURE);
    }

    gtk_main();

    return EXIT_SUCCESS;
}

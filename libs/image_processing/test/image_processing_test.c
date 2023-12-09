#include "image_processing.h"

#include <MagickWand/MagickWand.h>
#include <criterion/criterion.h>

#include <stdio.h>

Test(image_processing, example) {
    MagickWandGenesis();

    cr_assert(ip_process_image("image_01.jpeg", "/tmp/dest_01.%zu%zu.png"));
    cr_assert(ip_process_image("image_02.jpeg", "/tmp/dest_02.%zu%zu.png"));
    cr_assert(ip_process_image("image_03.jpeg", "/tmp/dest_03.%zu%zu.png"));
    cr_assert(ip_process_image("image_04.jpeg", "/tmp/dest_04.%zu%zu.png"));
    cr_assert(ip_process_image("image_05.jpeg", "/tmp/dest_05.%zu%zu.png"));
    cr_assert(ip_process_image("image_06.jpeg", "/tmp/dest_06.%zu%zu.png"));

    cr_assert(ip_rotate_image("image_02.jpeg", 15.0, "/tmp/image_07.jpeg"));
    cr_assert(ip_process_image("/tmp/image_07.jpeg", "/tmp/dest_07.%zu%zu.png")
    );

    MagickWandTerminus();
}

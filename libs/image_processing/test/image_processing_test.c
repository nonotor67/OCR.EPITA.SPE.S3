#include "image_processing.h"

#include <MagickWand/MagickWand.h>
#include <criterion/criterion.h>

#include <stdio.h>

Test(image_processing, example) {
    MagickWandGenesis();

    cr_assert(ip_process_image("image_01.jpeg", "dest_01.%zu%zu.png", 0.0));
    cr_assert(ip_process_image("image_02.jpeg", "dest_02.%zu%zu.png", 0.0));
    cr_assert(ip_process_image("image_03.jpeg", "dest_03.%zu%zu.png", 0.0));
    cr_assert(ip_process_image("image_04.jpeg", "dest_04.%zu%zu.png", 0.0));
    cr_assert(ip_process_image("image_05.jpeg", "dest_05.%zu%zu.png", 0.0));
    cr_assert(ip_process_image("image_06.jpeg", "dest_06.%zu%zu.png", 0.0));

    cr_assert(ip_process_image("image_02.jpeg", "dest_07.%zu%zu.png", 15.0));

    MagickWandTerminus();
}

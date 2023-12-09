#include "image_processing.h"

#include <MagickWand/MagickWand.h>
#include <criterion/criterion.h>

Test(image_processing, example) {
    MagickWandGenesis();

    cr_assert(ip_process_image("image_01.jpeg", NULL));
    cr_assert(ip_process_image("image_02.jpeg", NULL));
    cr_assert(ip_process_image("image_03.jpeg", NULL));
    cr_assert(ip_process_image("image_04.jpeg", NULL));
    cr_assert(ip_process_image("image_05.jpeg", NULL));
    cr_assert(ip_process_image("image_06.jpeg", NULL));

    cr_assert(ip_rotate_image("image_02.jpeg", 15.0, "/tmp/image_07.jpeg"));
    cr_assert(ip_process_image("/tmp/image_07.jpeg", NULL));

    MagickWandTerminus();
}

#include "image_processing.h"

#include <MagickWand/MagickWand.h>
#include <criterion/criterion.h>

#include <stdio.h>

Test(image_processing, example) {
    MagickWandGenesis();

    cr_assert(ip_process_image("image_01.jpeg", "dest_01.cell_%zu%zu.png"));
    cr_assert(ip_process_image("image_02.jpeg", "dest_02.cell_%zu%zu.png"));
    cr_assert(ip_process_image("image_03.jpeg", "dest_03.cell_%zu%zu.png"));
    cr_assert(ip_process_image("image_04.jpeg", "dest_04.cell_%zu%zu.png"));
    cr_assert(ip_process_image("image_05.jpeg", "dest_05.cell_%zu%zu.png"));
    cr_assert(ip_process_image("image_06.jpeg", "dest_06.cell_%zu%zu.png"));

    MagickWandTerminus();
}

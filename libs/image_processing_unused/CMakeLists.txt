add_library(image_processing_unused src/image_processing.c)

target_sources(
    image_processing_unused PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
    include/image_processing.h
)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
target_link_libraries(image_processing_unused PRIVATE SDL2::SDL2)

find_package(SDL2_image REQUIRED)
target_link_libraries(image_processing_unused PRIVATE SDL2_image::SDL2_image)

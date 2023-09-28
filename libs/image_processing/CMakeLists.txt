add_library(image_processing src/image_processing.c)

target_sources(
    image_processing PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
    include/image_processing.h
)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
target_link_libraries(image_processing PRIVATE SDL2::SDL2)

find_package(SDL2_image REQUIRED)
target_link_libraries(image_processing PRIVATE SDL2_image::SDL2_image)

add_subdirectory(test)

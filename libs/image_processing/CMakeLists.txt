add_library(image_processing src/image_processing.c)

target_sources(
    image_processing PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
    include/image_processing/image_processing.h
)

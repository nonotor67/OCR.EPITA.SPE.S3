add_library(image_processing src/image_processing.c)

target_sources(
    image_processing PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
    include/image_processing.h
)

pkg_check_modules(MAGICKWAND REQUIRED IMPORTED_TARGET MagickWand)
target_link_libraries(image_processing PUBLIC PkgConfig::MAGICKWAND)

add_subdirectory(test)

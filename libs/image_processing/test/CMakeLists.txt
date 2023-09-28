add_executable(image_processing_test image_processing_test.c)
target_link_libraries(
    image_processing_test PRIVATE image_processing PkgConfig::CRITERION
)
add_test(NAME image_processing_test COMMAND image_processing_test)

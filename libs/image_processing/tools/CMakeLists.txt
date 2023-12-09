add_executable(mkdataset mkdataset.c)
target_link_libraries(mkdataset PRIVATE image_processing neural_network)

pkg_check_modules(MAGICKWAND REQUIRED IMPORTED_TARGET MagickWand)
target_link_libraries(mkdataset PRIVATE PkgConfig::MAGICKWAND)

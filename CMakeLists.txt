cmake_minimum_required(VERSION 3.25)
project(ocr_sudoku_solver C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# C compiler warnings.
add_compile_options("$<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wall;-Wextra;-Wpedantic>")
# Sanitizers.
set(SANITIZER_OPTIONS $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
add_compile_options(${SANITIZER_OPTIONS})
add_link_options(${SANITIZER_OPTIONS})

enable_testing()

find_package(PkgConfig REQUIRED)
pkg_check_modules(CRITERION REQUIRED IMPORTED_TARGET criterion)

add_subdirectory(libs)
add_subdirectory(ocr_sudoku_solver)

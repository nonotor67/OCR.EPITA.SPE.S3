cmake_minimum_required(VERSION 3.25)
project(ocr_sudoku_solver C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

find_package(PkgConfig REQUIRED)
pkg_check_modules(CRITERION REQUIRED IMPORTED_TARGET criterion)

add_subdirectory(libs)
add_subdirectory(ocr_sudoku_solver)

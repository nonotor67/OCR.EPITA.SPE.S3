add_library(neural_network src/neural_network.c)

target_sources(
    neural_network PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
    include/neural_network.h
)

target_link_libraries(neural_network PUBLIC m)

add_subdirectory(test)

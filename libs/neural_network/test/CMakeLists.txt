add_executable(neural_network_test neural_network_test.c)
target_link_libraries(
    neural_network_test PRIVATE neural_network PkgConfig::CRITERION
)
add_test(NAME neural_network_test COMMAND neural_network_test)

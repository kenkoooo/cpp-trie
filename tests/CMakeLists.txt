include_directories(../src/)

add_executable(const_graph_test const_graph_test.cpp)
target_link_libraries(const_graph_test
    gtest
    gtest_main
)
add_test(const_graph_test const_graph_test)

add_executable(converter_test converter_test.cpp)
target_link_libraries(converter_test
    gtest
    gtest_main
)
add_test(converter_test converter_test)

add_executable(search_test search_test.cpp)
target_link_libraries(search_test
    gtest
    gtest_main
)
add_test(search_test search_test)


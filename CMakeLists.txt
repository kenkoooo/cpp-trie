cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project("cpp-trie")
enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS YES)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(tests)

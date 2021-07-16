cmake_minimum_required(VERSION 3.13)
project(gbas CXX)

find_package(Boost COMPONENTS unit_test_framework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

add_subdirectory(lib/expected)

add_subdirectory(src)
add_subdirectory(test)

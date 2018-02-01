cmake_minimum_required (VERSION 2.6)

project(algo)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE sources "src/*.cpp")

add_executable(algo ${sources})
target_include_directories(algo PUBLIC "test" ".")
target_link_libraries(algo PUBLIC gtest gmock pthread)
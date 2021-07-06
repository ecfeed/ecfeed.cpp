cmake_minimum_required(VERSION 3.5)
project(PackageTest CXX)

add_compile_options(-std=c++17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(example example.cpp)
target_link_libraries(example ${CONAN_LIBS})

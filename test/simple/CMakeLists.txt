cmake_minimum_required(VERSION 3.5)
project(ecfeed)

add_compile_options(-std=c++17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(ecfeed main.cpp)

target_link_libraries(ecfeed CONAN_PKG::ecfeed CONAN_PKG::ecfeed)
target_link_libraries(ecfeed CONAN_PKG::libcurl CONAN_PKG::libcurl)
target_link_libraries(ecfeed CONAN_PKG::openssl CONAN_PKG::openssl)
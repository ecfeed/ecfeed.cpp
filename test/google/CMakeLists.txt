cmake_minimum_required(VERSION 3.20)

project(ecfeed)

set(CMAKE_CXX_COMPILER_VERSION 8)

add_compile_options(-std=c++2a)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# add_executable(ecfeed src/export.cpp)
add_executable(ecfeed src/generate.cpp)
# add_executable(ecfeed src/f10x10.cpp)
# add_executable(ecfeed src/f100x2.cpp)
# add_executable(ecfeed src/test.cpp)

target_link_libraries(${PROJECT_NAME} stdc++fs)
target_link_libraries(ecfeed CONAN_PKG::ecfeed CONAN_PKG::ecfeed)
target_link_libraries(ecfeed CONAN_PKG::libcurl CONAN_PKG::libcurl)
target_link_libraries(ecfeed CONAN_PKG::openssl CONAN_PKG::openssl)
target_link_libraries(ecfeed CONAN_PKG::gtest CONAN_PKG::gtest)
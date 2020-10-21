#include "gtest/gtest.h"

// conan install conanfile.txt -g compiler_args
// clear && g++ main.cpp test_generate.cpp test_export.cpp  @conanbuildinfo.args -o output -std=c++17 && ./output

// Main is optional.

// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
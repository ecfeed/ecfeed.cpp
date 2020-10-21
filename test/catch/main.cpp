// #define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

// conan install conanfile.txt -g compiler_args
// clear && g++ main.cpp test_generate.cpp test_export.cpp  @conanbuildinfo.args -o output -std=c++17 && ./output

int main( int argc, char* argv[] ) {
    int result = Catch::Session().run( argc, argv );
    return result;
}
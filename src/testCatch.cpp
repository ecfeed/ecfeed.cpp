#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "ecfeed.hpp"
#include <iostream>

// conan install src/conanfile.txt -g compiler_args
// clear && g++ src/testCatch.cpp @conanbuildinfo.args -o test -std=c++17 && ./test

ecfeed::TestProvider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
auto data = testProvider.generateNwise("General.testMethod")->toList();

TEST_CASE("EcFeed test", "[NWise generate]")
{
    for (int i = 0 ; i < data.size() ; i++) {
        SECTION("test [" + std::to_string(i) + "]") {
            std::cout << data[i] << std::endl;
        }
    }
}
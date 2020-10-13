#define CATCH_CONFIG_MAIN
#include "ext/catch.h"
#include "ecfeed.h"
#include <iostream>

// clear && g++-8 -pthread -std=c++17 -o test src/testCatch.cpp  -lcurl -lcrypto -lstdc++fs  && ./test

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
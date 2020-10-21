#include "catch2/catch.hpp"
#include "ecfeed.hpp"
#include <iostream>

namespace test_generate {
    ecfeed::test_provider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
    auto data = testProvider.generate_nwise("General.testMethod")->to_list();
}

TEST_CASE("EcFeed test generate", "[NWise generate]") {
    for (int i = 0 ; i < test_generate::data.size() ; i++) {
        SECTION("test [" + std::to_string(i) + "]") {
            std::cout << test_generate::data[i] << std::endl;
        }
    }
}
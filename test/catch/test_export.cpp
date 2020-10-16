#include "catch2/catch.hpp"
#include "../../src/ecfeed.hpp"
#include <iostream>

namespace test_export {
    ecfeed::test_provider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
    auto data = testProvider.export_nwise("General.testMethod")->to_list();
}

TEST_CASE("EcFeed test export", "[NWise export]") {
    for (int i = 0 ; i < test_export::data.size() ; i++) {
        SECTION("test [" + std::to_string(i) + "]") {
            std::cout << test_export::data[i] << std::endl;
        }
    }
}
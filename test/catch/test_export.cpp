#include "catch2/catch.hpp"
#include "ecfeed.hpp"
#include <iostream>

namespace test_export {
    ecfeed::test_provider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
}

TEST_CASE("EcFeed test export", "[NWise export]") {
    auto i = GENERATE(from_range(test_export::testProvider.export_nwise("General.testMethod")->to_list()));
    std::cout << i << std::endl;
}
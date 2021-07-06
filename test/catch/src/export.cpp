#include "catch2/catch.hpp"
#include "ecfeed.hpp"
#include <iostream>

namespace test_export {
    ecfeed::test_provider testProvider("IMHL-K0DU-2U0I-J532-25J9");
}

TEST_CASE("EcFeed test export", "[NWise export]") {
    auto i = GENERATE(from_range(test_export::testProvider.export_nwise("com.example.test.Playground.size_10x10")->to_list()));
    std::cout << i << std::endl;
}
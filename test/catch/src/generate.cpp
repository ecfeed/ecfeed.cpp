#include "catch2/catch.hpp"
#include "ecfeed.hpp"
#include <iostream>

namespace test_generate {
    ecfeed::test_provider testProvider("IMHL-K0DU-2U0I-J532-25J9");
}

TEST_CASE("EcFeed test generate", "[NWise generate]") {
    auto i = GENERATE(from_range(test_generate::testProvider.generate_nwise("com.example.test.Playground.size_10x10")->to_list()));
    std::cout << i << std::endl;
}
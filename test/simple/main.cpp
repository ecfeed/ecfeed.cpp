#include "ecfeed.hpp"
#include <iostream>

int main() {
    
    std::string model = "V0G6-MHNQ-PDSR-G2WB-XOKV";
    std::string method = "General.testMethod";

    ecfeed::test_provider testProvider(model);

    for (auto& test : *testProvider.generate_nwise(method)) {
        std::cout << test << std::endl;
    }

}
#include "../../src/ecfeed.hpp"
#include <iostream>

int main() {
    
    std::string model = "V0G6-MHNQ-PDSR-G2WB-XOKV";
    std::string method = "General.testMethod";

    ecfeed::test_provider testProvider(model);

    for (auto& test : *testProvider.export_static(method, ecfeed::params().constraints("NONE").template_type(ecfeed::template_type::xml))) {
        std::cout << test << std::endl;
    }

}
#include "ecfeed.hpp"
#include <iostream>

int main() {
    
    std::string model = "IMHL-K0DU-2U0I-J532-25J9";
    std::string method = "com.example.test.Playground.size_100x2";

    ecfeed::test_provider testProvider(model);

    for (auto& test : *testProvider.export_nwise(method, ecfeed::params_nwise().constraints("NONE").template_type(ecfeed::template_type::xml))) {
        std::cout << test << std::endl;
    }

}
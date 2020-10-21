#include "ecfeed.hpp"
#include "nwise_generate.h"
#include "nwise_export.h"
#include <iostream>

// clear && g++ test/multiple_headers/main.cpp test/multiple_headers/nwise_generate.cpp test/multiple_headers/nwise_export.cpp -o output -std=c++17 -pthread -lcurl -lcrypto -lstdc++fs && ./output

int main() {
    
    std::string model = "V0G6-MHNQ-PDSR-G2WB-XOKV";
    std::string method = "General.testMethod";

    for (ecfeed::test_arguments test : nwise_generate(model, method)) {
        std::cout << test << std::endl;
    }

    for (std::string test : nwise_export(model, method)) {
        std::cout << test << std::endl;
    }

}
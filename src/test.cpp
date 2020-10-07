#include "main.cpp"
#include "gtest/gtest.h"
#include <iostream>
#include<queue>

std::vector<TestArguments> test_parameters() {
    std::string method = "ZCPH-DFYI-R7R7-R6MM-89L8";
    std::string keyStorePath = "/home/krzysztof/Desktop/git/ecfeed.java/com.ecfeed.runner/src/test/resources/security.p12";
    std::string generatorAddress = "develop-gen.ecfeed.com";

    ecfeed::TestProvider testProvider(method, keyStorePath, generatorAddress);

    std::vector<TestArguments> data;
    for (TestArguments element : *testProvider.generateNwise("QuickStart.test")) {
        data.push_back(element);
    }

    return data;
}

std::string method = "ZCPH-DFYI-R7R7-R6MM-89L8";
std::string keyStorePath = "/home/krzysztof/Desktop/git/ecfeed.java/com.ecfeed.runner/src/test/resources/security.p12";
std::string generatorAddress = "develop-gen.ecfeed.com";
ecfeed::TestProvider testProvider(method, keyStorePath, generatorAddress);

class ParameterizedEcFeedProviderFixture : public ::testing::TestWithParam<TestArguments> {};

INSTANTIATE_TEST_CASE_P(testSuite, ParameterizedEcFeedProviderFixture, ::testing::ValuesIn(*testProvider.generateNwise("QuickStart.test")));
TEST_P(ParameterizedEcFeedProviderFixture, testProvider1) {

   std::cout << GetParam() << std::endl;
   auto p = GetParam();
   int arg1 = p.get<int>(1);

}
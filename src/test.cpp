#include "main.cpp"
#include "gtest/gtest.h"
#include <iostream>
#include<queue>

std::vector<TestArguments> test_parameters() {
    ecfeed::TestProvider tp(
        "ZCPH-DFYI-R7R7-R6MM-89L8", 
        "/home/krzysztof/Desktop/git/ecfeed.java/com.ecfeed.runner/src/test/resources/security.p12", 
        "develop-gen.ecfeed.com"
    );

    std::vector<TestArguments> data;
    for (TestArguments element : *tp.generateNwise("QuickStart.test")) {
        data.push_back(element);
    }

    return data;
}

class ParameterizedEcFeedProviderFixture : public ::testing::TestWithParam<TestArguments> {};

INSTANTIATE_TEST_CASE_P(testSuite, ParameterizedEcFeedProviderFixture, ::testing::ValuesIn(test_parameters()));
TEST_P(ParameterizedEcFeedProviderFixture, testProvider1) {

   std::cout << GetParam() << std::endl;

}
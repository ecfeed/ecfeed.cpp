#include "main.cpp"
#include "gtest/gtest.h"
#include <iostream>
#include<queue>

class myTestFixture1: public ::testing::Test { 
public: 
    ecfeed::TestProvider* testProvider;

    void SetUp() override { 
        testProvider = new ecfeed::TestProvider(
            "ZCPH-DFYI-R7R7-R6MM-89L8", 
            "/home/krzysztof/Desktop/git/ecfeed.java/com.ecfeed.runner/src/test/resources/security.p12", 
            "develop-gen.ecfeed.com");
        }

    void TearDown() override {
        delete testProvider;
    }
};

struct TestParameter{
    int a;
    bool b;

    TestParameter(int a_, bool b_) : a(a_), b(b_) { }
};

std::ostream& operator<<(std::ostream& os, const TestParameter& p) {
    return os << p.a << ", " << p.b; 
}

class ParameterizedEcFeedProviderFixture : public ::myTestFixture1, public ::testing::WithParamInterface<std::string> {};
std::vector<TestParameter> test_parameters_1({TestParameter(1, true), TestParameter(2, false)});

std::vector<std::string> test_parameters_2() {
    ecfeed::TestProvider tp(
        "ZCPH-DFYI-R7R7-R6MM-89L8", 
        "/home/krzysztof/Desktop/git/ecfeed.java/com.ecfeed.runner/src/test/resources/security.p12", 
        "develop-gen.ecfeed.com"
    );

    std::vector<std::string> data;
    for (std::string element : *tp.exportNwise("QuickStart.test", ecfeed::TemplateType::CSV)) {
        data.push_back(element);
    }

    return data;
}


INSTANTIATE_TEST_CASE_P(testSuite, ParameterizedEcFeedProviderFixture, ::testing::ValuesIn(test_parameters_2()));
TEST_P(ParameterizedEcFeedProviderFixture, testProvider1) {

   std::cout << GetParam() << std::endl;
}
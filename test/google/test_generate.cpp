#include "gtest/gtest.h"
#include "../../src/ecfeed.hpp"
#include <iostream>

namespace test_generate {
   int n = 2;
   int coverage = 100;
   int length = 50;
   bool duplicates = true;
   bool adaptive = true;
   ecfeed::template_type template_type = ecfeed::template_type::csv;
   std::set<std::string> constraints = {"constraint1"};
   std::set<std::string> test_suites = {"suite1"};
   std::map<std::string, std::set<std::string>> choices = {{"arg0", {"choice1", "choice2"}}, {"arg1", {"choice1", "choice2"}}};

   std::map<std::string, std::any> optionsGenerateNWise = {{"coverage", coverage}, {"n", n}, {"constraints", constraints}, {"choices", choices}};
   std::map<std::string, std::any> optionsGeneratePairwise = {{"coverage", coverage}, {"constraints", constraints}, {"choices", choices}};
   std::map<std::string, std::any> optionsGenerateRandom = {{"length", length}, {"duplicates", duplicates}, {"adaptive", adaptive}, {"constraints", constraints}, {"choices", choices}};
   std::map<std::string, std::any> optionsGenerateCartesian = {{"constraints", constraints}, {"choices", choices}};
   std::map<std::string, std::any> optionsGenerateStatic = {{"constraints", constraints}, {"choices", choices}, {"testSuites", test_suites}};

   ecfeed::test_provider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
   std::string method = "General.testMethod";
}

// -----------------------------------------------------------------------------------------------------------------------------

class FixtureGenerate : public ::testing::TestWithParam<ecfeed::test_arguments> {};

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_nwise(test_generate::method, test_generate::optionsGenerateNWise))->to_list()));
// TEST_P(FixtureGenerate, NWise) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_pairwise(test_generate::method, test_generate::optionsGeneratePairwise))->to_list()));
// TEST_P(FixtureGenerate, Pairwise_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_cartesian(test_generate::method, test_generate::optionsGenerateCartesian))->to_list()));
// TEST_P(FixtureGenerate, Cartesian_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_random(test_generate::method, test_generate::optionsGenerateRandom))->to_list()));
// TEST_P(FixtureGenerate, Random_Export) {
//    std::cout << GetParam() << std::endl;
// }

INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_static(test_generate::method, test_generate::optionsGenerateStatic))->to_list()));
TEST_P(FixtureGenerate, Static_Export) {
   std::cout << GetParam() << std::endl;
}
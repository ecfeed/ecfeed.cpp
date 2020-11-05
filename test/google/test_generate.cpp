#include "gtest/gtest.h"
#include "ecfeed.hpp"
#include <iostream>

namespace test_generate {
  
   std::set<std::string> constraints = {"constraint1"};
   std::set<std::string> test_suites = {"suite1"};
   std::map<std::string, std::set<std::string>> choices = {{"arg0", {"choice1", "choice2"}}, {"arg1", {"choice1", "choice2"}}};

   ecfeed::params_nwise _nwise = ecfeed::params_nwise().n(2).coverage(100).constraints(constraints).choices(choices);
   ecfeed::params_pairwise _pairwise = ecfeed::params_pairwise().coverage(100).constraints(constraints).choices(choices);
   ecfeed::params_random _random = ecfeed::params_random().length(50).duplicates(true).adaptive(true).constraints(constraints).choices(choices);
   ecfeed::params_cartesian _cartesian = ecfeed::params_cartesian().constraints(constraints).choices(choices);
   ecfeed::params_static _static = ecfeed::params_static().test_suites(test_suites);

   ecfeed::test_provider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
   std::string method = "General.testMethod";
}

// -----------------------------------------------------------------------------------------------------------------------------

class FixtureGenerate : public ::testing::TestWithParam<ecfeed::test_arguments> {};

INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_nwise(test_generate::method, test_generate::_nwise))->to_list()));
TEST_P(FixtureGenerate, NWise) {
   std::cout << GetParam() << std::endl;
}

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_pairwise(test_generate::method, test_generate::_pairwise))->to_list()));
// TEST_P(FixtureGenerate, Pairwise_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_cartesian(test_generate::method, test_generate::_cartesian))->to_list()));
// TEST_P(FixtureGenerate, Cartesian_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_random(test_generate::method, test_generate::_random))->to_list()));
// TEST_P(FixtureGenerate, Random_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_static(test_generate::method, test_generate::_static))->to_list()));
// TEST_P(FixtureGenerate, Static_Export) {
//    std::cout << GetParam() << std::endl;
// }
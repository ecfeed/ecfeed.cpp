#include "gtest/gtest.h"
#include "ecfeed.hpp"
#include <iostream>

namespace test_export {

   std::set<std::string> constraints = {"constraint1"};
   std::set<std::string> test_suites = {"suite1"};
   std::map<std::string, std::set<std::string>> choices = {{"arg0", {"choice1", "choice2"}}, {"arg1", {"choice1", "choice2"}}};

   ecfeed::params options = ecfeed::params()
                              .n(2)
                              .coverage(100)
                              .length(50)
                              .duplicates(true)
                              .adaptive(true)
                              .template_type(ecfeed::template_type::csv)
                              .constraints(constraints)
                              .test_suites(test_suites)
                              .choices(choices);

   ecfeed::test_provider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
   std::string method = "General.testMethod";
}

// -----------------------------------------------------------------------------------------------------------------------------

class FixtureExport : public ::testing::TestWithParam<std::string> {};

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider.export_nwise(test_export::method, test_export::options))->to_list()));
// TEST_P(FixtureExport, NWise_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider.export_pairwise(test_export::method, test_export::options))->to_list()));
// TEST_P(FixtureExport, Pairwise_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider.export_cartesian(test_export::method, test_export::options))->to_list()));
// TEST_P(FixtureExport, Cartesian_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider.export_random(test_export::method, test_export::options))->to_list()));
// TEST_P(FixtureExport, Random_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider.export_static(test_export::method, test_export::options))->to_list()));
// TEST_P(FixtureExport, Static_Export) {
//    std::cout << GetParam() << std::endl;
// }
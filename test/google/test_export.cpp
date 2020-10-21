#include "gtest/gtest.h"
#include "ecfeed.hpp"
#include <iostream>

namespace test_export {
   int n = 2;
   int coverage = 100;
   int length = 50;
   bool duplicates = true;
   bool adaptive = true;
   ecfeed::template_type template_type = ecfeed::template_type::csv;
   std::set<std::string> constraints = {"constraint1"};
   std::set<std::string> test_suites = {"suite1"};
   std::map<std::string, std::set<std::string>> choices = {{"arg0", {"choice1", "choice2"}}, {"arg1", {"choice1", "choice2"}}};

   std::map<std::string, std::any> optionsExportNWise = {{"template", template_type}, {"coverage", coverage}, {"n", n}, {"constraints", constraints}, {"choices", choices}};
   std::map<std::string, std::any> optionsExportPairwise = {{"template", template_type}, {"coverage", coverage}, {"constraints", constraints}, {"choices", choices}};
   std::map<std::string, std::any> optionsExportRandom = {{"template", template_type}, {"length", length}, {"duplicates", duplicates}, {"adaptive", adaptive}, {"constraints", constraints}, {"choices", choices}};
   std::map<std::string, std::any> optionsExportCartesian = {{"template", template_type}, {"constraints", constraints}, {"choices", choices}};
   std::map<std::string, std::any> optionsExportStatic = {{"template", template_type}, {"constraints", constraints}, {"choices", choices}, {"testSuites", test_suites}};

   ecfeed::test_provider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
   std::string method = "General.testMethod";
}

// -----------------------------------------------------------------------------------------------------------------------------

class FixtureExport : public ::testing::TestWithParam<std::string> {};

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider.export_nwise(test_export::method, test_export::optionsExportNWise))->to_list()));
// TEST_P(FixtureExport, NWise_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider.export_pairwise(test_export::method, test_export::optionsExportPairwise))->to_list()));
// TEST_P(FixtureExport, Pairwise_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider.export_cartesian(test_export::method, test_export::optionsExportCartesian))->to_list()));
// TEST_P(FixtureExport, Cartesian_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider.export_random(test_export::method, test_export::optionsExportRandom))->to_list()));
// TEST_P(FixtureExport, Random_Export) {
//    std::cout << GetParam() << std::endl;
// }

INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider.export_static(test_export::method, test_export::optionsExportStatic))->to_list()));
TEST_P(FixtureExport, Static_Export) {
   std::cout << GetParam() << std::endl;
}
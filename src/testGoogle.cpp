#include "gtest/gtest.h"
#include "ecfeed.hpp"
#include <iostream>

// conan install src/conanfile.txt -g compiler_args
// clear && g++ src/testGoogle.cpp @conanbuildinfo.args -o output -std=c++17 && ./output

int n = 2;
int coverage = 100;
int length = 50;    // The generation should be stopped before reaching the limit.
bool duplicates = true;
bool adaptive = true;
ecfeed::template_type template_type = ecfeed::template_type::json;
std::set<std::string> constraints = {"constraint1"};
std::set<std::string> test_suites = {"suite1"};
std::map<std::string, std::set<std::string>> choices = {{"arg0", {"choice1", "choice2"}}, {"arg1", {"choice1", "choice2"}}};

std::map<std::string, std::any> optionsGenerateNWise = {{"coverage", coverage}, {"n", n}, {"constraints", constraints}, {"choices", choices}};
std::map<std::string, std::any> optionsGeneratePairwise = {{"coverage", coverage}, {"constraints", constraints}, {"choices", choices}};
std::map<std::string, std::any> optionsGenerateRandom = {{"length", length}, {"duplicates", duplicates}, {"adaptive", adaptive}, {"constraints", constraints}, {"choices", choices}};
std::map<std::string, std::any> optionsGenerateCartesian = {{"constraints", constraints}, {"choices", choices}};
std::map<std::string, std::any> optionsGenerateStatic = {{"constraints", constraints}, {"choices", choices}, {"testSuites", test_suites}};

std::map<std::string, std::any> optionsExportNWise = {{"template", template_type}, {"coverage", coverage}, {"n", n}, {"constraints", constraints}, {"choices", choices}};
std::map<std::string, std::any> optionsExportPairwise = {{"template", template_type}, {"coverage", coverage}, {"constraints", constraints}, {"choices", choices}};
std::map<std::string, std::any> optionsExportRandom = {{"template", template_type}, {"length", length}, {"duplicates", duplicates}, {"adaptive", adaptive}, {"constraints", constraints}, {"choices", choices}};
std::map<std::string, std::any> optionsExportCartesian = {{"template", template_type}, {"constraints", constraints}, {"choices", choices}};
std::map<std::string, std::any> optionsExportStatic = {{"template", template_type}, {"constraints", constraints}, {"choices", choices}, {"testSuites", test_suites}};

// -----------------------------------------------------------------------------------------------------------------------------

ecfeed::test_provider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
std::string method = "General.testMethod";

class FixtureGenerate : public ::testing::TestWithParam<ecfeed::test_arguments> {};

INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((testProvider.generate_nwise(method, optionsGenerateNWise))->to_list()));
TEST_P(FixtureGenerate, NWise) {
   std::cout << GetParam() << std::endl;
}

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((testProvider.generate_pairwise(method, optionsGeneratePairwise))->to_list()));
// TEST_P(FixtureGenerate, Pairwise) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((testProvider.generate_cartesian(method, optionsGenerateCartesian))->to_list()));
// TEST_P(FixtureGenerate, Cartesian) {
//    std::cout << GetParam() << std::endl;
//    std::cout << GetParam().get_string("arg1") << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((testProvider.generate_random(method, optionsGenerateRandom))->to_list()));
// TEST_P(FixtureGenerate, Random) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((testProvider.generate_static(method, optionsGenerateStatic))->to_list()));
// TEST_P(FixtureGenerate, Static) {
//    std::cout << GetParam() << std::endl;
// }

class FixtureExport : public ::testing::TestWithParam<std::string> {};

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((testProvider.export_nwise(method, optionsExportNWise))->to_list()));
// TEST_P(FixtureExport, NWise) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((testProvider.export_pairwise(method, optionsExportPairwise))->to_list()));
// TEST_P(FixtureExport, Pairwise) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((testProvider.export_cartesian(method, optionsExportCartesian))->to_list()));
// TEST_P(FixtureExport, Cartesian) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((testProvider.export_random(method, optionsExportRandom))->to_list()));
// TEST_P(FixtureExport, Random) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((testProvider.export_static(method, optionsExportStatic))->to_list()));
// TEST_P(FixtureExport, Static) {
//    std::cout << GetParam() << std::endl;
// }
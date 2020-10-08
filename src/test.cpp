#include "main.cpp"
#include "gtest/gtest.h"
#include <iostream>
#include<queue>

int n = 2;
int coverage = 100;
int length = 50;    // The generation should be stopped before reaching the limit.
bool duplicates = true;
bool adaptive = true;
ecfeed::TemplateType template_type = ecfeed::TemplateType::JSON;
std::set<std::string> constraints = {"constraint1"};    // Missing std::string options.
std::set<std::string> test_suites = {"suite1"};    // Missing std::string options.
std::map<std::string, std::set<std::string>> choices = {{"arg1", {"choice1", "choice2"}}, {"arg2", {"choice1"}}};   // Missing std::string options.

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

ecfeed::TestProvider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
std::string method = "QuickStart.test";

class FixtureGenerate : public ::testing::TestWithParam<TestArguments> {};

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((testProvider.generateNwise(method, optionsGenerateNWise))->toList()));
// TEST_P(FixtureGenerate, NWise) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((testProvider.generatePairwise(method, optionsGeneratePairwise))->toList()));
// TEST_P(FixtureGenerate, Pairwise) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((testProvider.generateCartesian(method, optionsGenerateCartesian))->toList()));
// TEST_P(FixtureGenerate, Cartesian) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((testProvider.generateRandom(method, optionsGenerateRandom))->toList()));
// TEST_P(FixtureGenerate, Random) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((testProvider.generateStatic(method, optionsGenerateStatic))->toList()));
// TEST_P(FixtureGenerate, Static) {
//    std::cout << GetParam() << std::endl;
// }

class FixtureExport : public ::testing::TestWithParam<std::string> {};

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((testProvider.exportNwise(method, optionsExportNWise))->toList()));
// TEST_P(FixtureExport, NWise) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((testProvider.exportPairwise(method, optionsExportPairwise))->toList()));
// TEST_P(FixtureExport, Pairwise) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((testProvider.exportCartesian(method, optionsExportCartesian))->toList()));
// TEST_P(FixtureExport, Cartesian) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((testProvider.exportRandom(method, optionsExportRandom))->toList()));
// TEST_P(FixtureExport, Random) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((testProvider.exportStatic(method, optionsExportStatic))->toList()));
// TEST_P(FixtureExport, Static) {
//    std::cout << GetParam() << std::endl;
// }
#include "gtest/gtest.h"
#include "ecfeed.hpp"
#include <iostream>
#include "config.hpp"

namespace test_generate {
    std::set<std::string> constraints = {"gender"};
    std::map<std::string, std::set<std::string>> choices = {{"firstName", {"male:short"}}};
    std::set<std::string> suites = {"default%20suite"};

    ecfeed::params_nwise _nwise = ecfeed::params_nwise().choices(choices).constraints(constraints).n(2).coverage(100);
    ecfeed::params_pairwise _pairwise = ecfeed::params_pairwise().choices(choices).constraints(constraints).coverage(100);
    ecfeed::params_random _random = ecfeed::params_random().choices(choices).constraints(constraints).length(50).duplicates(true).adaptive(true);
    ecfeed::params_cartesian _cartesian = ecfeed::params_cartesian().choices(choices).constraints(constraints);
    ecfeed::params_static _static = ecfeed::params_static().test_suites(suites);

    std::shared_ptr<ecfeed::test_provider> testProvider = config::Default::get_test_provider();
}

// -----------------------------------------------------------------------------------------------------------------------------

class FixtureGenerate : public ::testing::TestWithParam<ecfeed::test_arguments> {};

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider->generate_nwise(config::Default::F_LOAN_2, test_generate::_nwise))->to_list()));
// TEST_P(FixtureGenerate, NWise_Generate) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider->generate_pairwise(config::Default::F_LOAN_2, test_generate::_pairwise))->to_list()));
// TEST_P(FixtureGenerate, Pairwise_Generate) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider->generate_random(config::Default::F_LOAN_2, test_generate::_random))->to_list()));
// TEST_P(FixtureGenerate, Random_Generate) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider->generate_cartesian(config::Default::F_LOAN_2, test_generate::_cartesian))->to_list()));
// TEST_P(FixtureGenerate, Cartesian_Generate) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider->generate_static(config::Default::F_LOAN_2, test_generate::_static))->to_list()));
// TEST_P(FixtureGenerate, Static_Generate) {
//    std::cout << GetParam() << std::endl;
// }
#include "gtest/gtest.h"
#include "ecfeed.hpp"
#include <iostream>
#include "config.hpp"

namespace test_export {

   ecfeed::params_nwise _nwise = ecfeed::params_nwise().n(2).coverage(100).template_type(ecfeed::template_type::json);
   ecfeed::params_pairwise _pairwise = ecfeed::params_pairwise().coverage(100).template_type(ecfeed::template_type::csv);
   ecfeed::params_random _random = ecfeed::params_random().length(50).duplicates(true).adaptive(true).template_type(ecfeed::template_type::xml);
   ecfeed::params_cartesian _cartesian = ecfeed::params_cartesian().template_type(ecfeed::template_type::gherkin);
   ecfeed::params_nwise _raw = ecfeed::params_nwise().template_type(ecfeed::template_type::raw);

   std::shared_ptr<ecfeed::test_provider> testProvider = config::Default::get_test_provider();
}

// -----------------------------------------------------------------------------------------------------------------------------

class FixtureExport : public ::testing::TestWithParam<std::string> {};

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider->export_nwise(config::Default::F_QUICK, test_export::_nwise))->to_list()));
// TEST_P(FixtureExport, NWise_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider->export_pairwise(config::Default::F_QUICK, test_export::_pairwise))->to_list()));
// TEST_P(FixtureExport, Pairwise_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider->export_random(config::Default::F_QUICK, test_export::_random))->to_list()));
// TEST_P(FixtureExport, Random_Export) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider->export_cartesian(config::Default::F_QUICK, test_export::_cartesian))->to_list()));
// TEST_P(FixtureExport, Cartesian_Export) {
//    std::cout << GetParam() << std::endl;
// }

INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider->export_nwise(config::Default::F_QUICK, test_export::_raw))->to_list()));
TEST_P(FixtureExport, NWise_Export) {
   std::cout << GetParam() << std::endl;
}

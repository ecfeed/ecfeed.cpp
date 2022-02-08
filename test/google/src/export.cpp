#include "gtest/gtest.h"
#include "ecfeed.hpp"
#include <iostream>
#include "config.hpp"

namespace test_export {

   ecfeed::params_nwise _json = ecfeed::params_nwise().template_type(ecfeed::template_type::json);
   ecfeed::params_nwise _csv = ecfeed::params_nwise().template_type(ecfeed::template_type::csv);
   ecfeed::params_nwise _xml = ecfeed::params_nwise().template_type(ecfeed::template_type::xml);
   ecfeed::params_nwise _gherkin = ecfeed::params_nwise().template_type(ecfeed::template_type::gherkin);
   ecfeed::params_nwise _raw = ecfeed::params_nwise().template_type(ecfeed::template_type::raw);

   std::shared_ptr<ecfeed::test_provider> testProvider = config::Default::get_test_provider();
}

// -----------------------------------------------------------------------------------------------------------------------------

class FixtureExport : public ::testing::TestWithParam<std::string> {};

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider->export_nwise(config::Default::F_LOAN_2, test_export::_json))->to_list()));
// TEST_P(FixtureExport, NWise_JSON) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider->export_nwise(config::Default::F_LOAN_2, test_export::_csv))->to_list()));
// TEST_P(FixtureExport, NWise_CSV) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider->export_nwise(config::Default::F_LOAN_2, test_export::_xml))->to_list()));
// TEST_P(FixtureExport, NWise_XML) {
//    std::cout << GetParam() << std::endl;
// }

// INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider->export_nwise(config::Default::F_LOAN_2, test_export::_gherkin))->to_list()));
// TEST_P(FixtureExport, NWise_Gherkin) {
//    std::cout << GetParam() << std::endl;
// }

INSTANTIATE_TEST_CASE_P(Generate, FixtureExport, ::testing::ValuesIn((test_export::testProvider->export_nwise(config::Default::F_LOAN_2, test_export::_raw))->to_list()));
TEST_P(FixtureExport, NWise_Raw) {
   std::cout << GetParam() << std::endl;
}

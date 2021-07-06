#include "gtest/gtest.h"
#include "ecfeed.hpp"
#include <iostream>

namespace test_generate {

   ecfeed::test_provider testProvider("IMHL-K0DU-2U0I-J532-25J9");
   std::string method = "QuickStart.test";

   auto gen_random_quantity_single = ecfeed::params_random()
      .feedback(true).length(1).label("Random / Quantity - Single");

   auto gen_random_quantity_short = ecfeed::params_random()
      .feedback(true).length(100 + (rand() % 400)).label("Random / Quantity - Short");
   
   auto gen_random_quantity_long = ecfeed::params_random()
      .feedback(true).length(1000 + (rand() % 4000)).label("Random / Quantity - Long");

   auto gen_random = ecfeed::params_random()
      .feedback(true).label("Random");
   
   auto gen_random_adaptive = ecfeed::params_random()
      .feedback(true).adaptive(false).label("Random - Adaptive");

   auto gen_random_duplicates = ecfeed::params_random()
      .feedback(true).duplicates(true).label("Random - Duplicates");

   auto gen_nwise = ecfeed::params_nwise()
      .feedback(true).label("NWise");

   auto gen_nwise_n = ecfeed::params_nwise()
      .feedback(true).n(3).label("NWise - N");

   auto gen_nwise_coverage = ecfeed::params_nwise()
      .feedback(true).coverage(50).label("NWise - Coverage");

   auto gen_nwise_constraints_none = ecfeed::params_nwise()
      .feedback(true).constraints("NONE").label("NWise / Constraints - None");

   std::set<std::string> constraints = {"constraint1", "constraint2"};
   auto gen_nwise_constraints_selected = ecfeed::params_nwise()
      .feedback(true).constraints(constraints).label("NWise / Constraints - Selected");

   auto gen_nwise_choices_selected = ecfeed::params_nwise()
      .feedback(true).choices({{"arg1", {"choice1", "choice2"}}, {"arg2", {"choice2", "choice3"}}}).label("NWise / Choices - Selected");

   auto gen_cartesian = ecfeed::params_cartesian()
      .feedback(true).label("Cartesian");

   auto gen_static = ecfeed::params_static()
      .feedback(true).label("Static");

   auto gen_static_all = ecfeed::params_static()
      .feedback(true).test_suites("ALL").label("Static - All");

   std::set<std::string> test_suites = {"suite1"};
   auto gen_static_selected = ecfeed::params_static()
      .feedback(true).test_suites(test_suites).label("Static - Selected");

   auto gen_nwise_feedback = ecfeed::params_nwise()
      .feedback(true).label("NWise / Feedback");

   void validate(const ecfeed::test_arguments& test_arguments) {

      bool result = test_arguments.get_int(0) < 2;
      test_arguments.get_handle()->add_feedback(result);
      ASSERT_TRUE(result);
   }

   void validate_feedback(const ecfeed::test_arguments& test_arguments) {

      if (test_arguments.get_int(0) < 2) {
         test_arguments.get_handle()->add_feedback(false, 100 + (rand() % 200), "Failed - arg1 < 2", {{"key1", "value1"}, {"key2", "value2"}});
         FAIL();
      } 
     
      test_arguments.get_handle()->add_feedback(true);
   }
}

// -----------------------------------------------------------------------------------------------------------------------------

class FixtureGenerate : public ::testing::TestWithParam<ecfeed::test_arguments> {};

// INSTANTIATE_TEST_CASE_P(gen_random_quantity_single, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_random(test_generate::method, test_generate::gen_random_quantity_single))->to_list()));
// TEST_P(FixtureGenerate, gen_random_quantity_single) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_random_quantity_short, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_random(test_generate::method, test_generate::gen_random_quantity_short))->to_list()));
// TEST_P(FixtureGenerate, gen_random_quantity_short) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_random_quantity_long, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_random(test_generate::method, test_generate::gen_random_quantity_long))->to_list()));
// TEST_P(FixtureGenerate, gen_random_quantity_long) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_random, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_random(test_generate::method, test_generate::gen_random))->to_list()));
// TEST_P(FixtureGenerate, gen_random) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_random_adaptive, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_random(test_generate::method, test_generate::gen_random_adaptive))->to_list()));
// TEST_P(FixtureGenerate, gen_random_adaptive) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_random_duplicates, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_random(test_generate::method, test_generate::gen_random_duplicates))->to_list()));
// TEST_P(FixtureGenerate, gen_random_duplicates) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_nwise, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_nwise(test_generate::method, test_generate::gen_nwise))->to_list()));
// TEST_P(FixtureGenerate, gen_nwise) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_nwise_n, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_nwise(test_generate::method, test_generate::gen_nwise_n))->to_list()));
// TEST_P(FixtureGenerate, gen_nwise_n) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_nwise_coverage, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_nwise(test_generate::method, test_generate::gen_nwise_coverage))->to_list()));
// TEST_P(FixtureGenerate, gen_nwise_coverage) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_nwise_constraints_none, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_nwise(test_generate::method, test_generate::gen_nwise_constraints_none))->to_list()));
// TEST_P(FixtureGenerate, gen_nwise_constraints_none) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_nwise_constraints_selected, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_nwise(test_generate::method, test_generate::gen_nwise_constraints_selected))->to_list()));
// TEST_P(FixtureGenerate, gen_nwise_constraints_selected) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_nwise_choices_selected, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_nwise(test_generate::method, test_generate::gen_nwise_choices_selected))->to_list()));
// TEST_P(FixtureGenerate, gen_nwise_choices_selected) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_cartesian, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_cartesian(test_generate::method, test_generate::gen_cartesian))->to_list()));
// TEST_P(FixtureGenerate, gen_cartesian) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_static, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_static(test_generate::method, test_generate::gen_static))->to_list()));
// TEST_P(FixtureGenerate, gen_static) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_static_all, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_static(test_generate::method, test_generate::gen_static_all))->to_list()));
// TEST_P(FixtureGenerate, gen_static_all) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_static_selected, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_static(test_generate::method, test_generate::gen_static_selected))->to_list()));
// TEST_P(FixtureGenerate, gen_static_selected) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_nwise_feedback, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_nwise(test_generate::method, test_generate::gen_nwise_feedback))->to_list()));
// TEST_P(FixtureGenerate, gen_nwise_feedback) {
   
//    test_generate::validate_feedback(GetParam());
// }
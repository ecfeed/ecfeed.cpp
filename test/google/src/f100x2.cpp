#include "gtest/gtest.h"
#include "ecfeed.hpp"
#include <iostream>

namespace test_generate {

   ecfeed::test_provider testProvider("IMHL-K0DU-2U0I-J532-25J9");
   std::string method = "com.example.test.Playground.size_100x2";

   auto gen_random_quantity_single = ecfeed::params_random()
      .feedback(true).length(1).label("Random / Quantity - Single");

   auto gen_random_quantity_short = ecfeed::params_random()
      .feedback(true).length(100 + (rand() % 400)).label("Random / Quantity - Short");
   
   auto gen_random_quantity_long = ecfeed::params_random()
      .feedback(true).length(1000 + (rand() % 4000)).label("Random / Quantity - Long");

   auto gen_random_custom = ecfeed::params_random()
      .feedback(true).length(1).custom({{"key1", "value1"}, {"key2", "value2"}}).label("Random / Custom");

   auto gen_nwise = ecfeed::params_nwise()
      .feedback(true).label("NWise");

   auto gen_nwise_feedback = ecfeed::params_nwise()
      .feedback(true).label("NWise / Feedback");

   void validate(const ecfeed::test_arguments& test_arguments) {

      bool result = (test_arguments.get_string(0) != "a00") && (test_arguments.get_string(1) != "b00");
      test_arguments.get_handle()->add_feedback(result);
      ASSERT_TRUE(result);
   }

   void validate_feedback(const ecfeed::test_arguments& test_arguments) {

      if (test_arguments.get_string(0) == "a00") {
         test_arguments.get_handle()->add_feedback(false, 100 + (rand() % 200), "Failed - a", {{"key1", "value1"}, {"key2", "value2"}});
         FAIL();
      } else if (test_arguments.get_string(1) == "b00") {
         test_arguments.get_handle()->add_feedback(false, 100 + (rand() % 200), "Failed - b", {{"key1", "value1"}, {"key2", "value2"}});
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

// INSTANTIATE_TEST_CASE_P(gen_random_custom, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_random(test_generate::method, test_generate::gen_random_custom))->to_list()));
// TEST_P(FixtureGenerate, gen_random_custom) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_nwise, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_nwise(test_generate::method, test_generate::gen_nwise))->to_list()));
// TEST_P(FixtureGenerate, gen_nwise) {
   
//    test_generate::validate(GetParam());
// }

// INSTANTIATE_TEST_CASE_P(gen_nwise_feedback, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_nwise(test_generate::method, test_generate::gen_nwise_feedback))->to_list()));
// TEST_P(FixtureGenerate, gen_nwise_feedback) {
   
//    test_generate::validate_feedback(GetParam());
// }
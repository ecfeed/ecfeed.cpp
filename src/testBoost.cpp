#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include "ecfeed.h"
#include <iostream>
#include <vector>

// https://www.boost.org/doc/libs/1_64_0/libs/test/doc/html/boost_test/adv_scenarios/build_utf.html
// http://www.linuxfromscratch.org/blfs/view/svn/general/boost.html
// https://www.boost.org/doc/libs/1_64_0/libs/test/doc/html/boost_test/tests_organization/test_cases/param_test.html
// https://www.boost.org/users/download/

// clear && g++ -o test src/testBoost.cpp -lboost_unit_test_framework -std=c++17 -pthread -lcurl -lcrypto -lstdc++fs && ./test

using namespace boost::unit_test;

void present( ecfeed::TestArguments i )
{
  std::cout << i << std::endl;
}

test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    ecfeed::TestProvider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
    std::vector<ecfeed::TestArguments> data = testProvider.generateNwise("General.testMethod")->toList();

    framework::master_test_suite().add( BOOST_PARAM_TEST_CASE( &present, data.begin(), data.end() ) );

  return 0;
}
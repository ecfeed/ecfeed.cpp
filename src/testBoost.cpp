#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include "ecfeed.h"
#include <iostream>
#include <vector>

// conan install src/conanfile.txt -g compiler_args
// clear && g++ src/testCatch.cpp @conanbuildinfo.args -o test -std=c++17 && ./test

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
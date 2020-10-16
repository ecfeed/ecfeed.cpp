#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include "ecfeed.hpp"
#include <iostream>
#include <vector>

// conan install src/conanfile.txt -g compiler_args
// clear && g++ src/testCatch.cpp @conanbuildinfo.args -o test -std=c++17 && ./test

using namespace boost::unit_test;

void present( ecfeed::test_arguments i ) {
  std::cout << i << std::endl;
}

test_suite* init_unit_test_suite( int argc, char* argv[] ) {
    ecfeed::test_provider testProvider("V0G6-MHNQ-PDSR-G2WB-XOKV");
    std::vector<ecfeed::test_arguments> data = testProvider.generate_nwise("General.testMethod")->to_list();

    framework::master_test_suite().add( BOOST_PARAM_TEST_CASE( &present, data.begin(), data.end() ) );

  return 0;
}
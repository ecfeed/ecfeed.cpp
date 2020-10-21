#define BOOST_TEST_DYN_LINK
#include "../../src/ecfeed.hpp"
#include <boost/test/included/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include "nwise_generate.h"
#include "nwise_export.h"
#include <iostream>
#include <vector>

// conan install conanfile.txt -g compiler_args
// clear && g++ main.cpp nwise_generate.cpp nwise_export.cpp @conanbuildinfo.args -o output -std=c++17 && ./output
// -lstdc++fs

using namespace boost::unit_test;

void present( ecfeed::test_arguments i ) {
  std::cout << i << std::endl;
}

test_suite* init_unit_test_suite( int argc, char* argv[] ) {
    std::string model = "V0G6-MHNQ-PDSR-G2WB-XOKV";
    std::string method = "General.testMethod";

    std::vector<std::string> data_export = nwise_export(model, method);
    // auto data_generate = nwise_generate(model, method);

    // framework::master_test_suite().add( BOOST_PARAM_TEST_CASE( &present, data_export.begin(), data_export.end() ) );

  return 0;
}
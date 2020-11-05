#define BOOST_TEST_DYN_LINK
#include "ecfeed.hpp"
#include <boost/test/included/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include "nwise_generate.h"
#include "nwise_export.h"
#include <iostream>
#include <vector>

using namespace boost::unit_test;

void presentExport( std::string i ) {
  std::cout << i << std::endl;
}

void presentGenerate( ecfeed::test_arguments i ) {
  std::cout << i << std::endl;
}

test_suite* init_unit_test_suite( int argc, char* argv[] ) {
    std::string model = "V0G6-MHNQ-PDSR-G2WB-XOKV";
    std::string method = "General.testMethod";

    std::vector<std::string> data_export = nwise_export(model, method);
    auto data_generate = nwise_generate(model, method);

    framework::master_test_suite().add( BOOST_PARAM_TEST_CASE( &presentExport, data_export.begin(), data_export.end() ) );
    framework::master_test_suite().add( BOOST_PARAM_TEST_CASE( &presentGenerate, data_generate.begin(), data_generate.end() ) );
    
    return 0;
}
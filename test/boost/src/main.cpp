#define BOOST_TEST_DYN_LINK
#include "ecfeed.hpp"
#include <boost/test/included/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include <iostream>
#include <vector>

using namespace boost::unit_test;

void presentExport( std::string i ) {
  std::cout << i << std::endl;
}

void presentGenerate( ecfeed::test_arguments i ) {
  std::cout << i << std::endl;
}

std::vector<std::string> nwise_export(std::string model, std::string method) {
    ecfeed::test_provider testProvider(model);

    return testProvider.export_nwise(method)->to_list();
}

std::vector<ecfeed::test_arguments> nwise_generate(std::string model, std::string method) {
    ecfeed::test_provider testProvider(model);

    return testProvider.generate_pairwise(method)->to_list();
}

test_suite* init_unit_test_suite( int argc, char* argv[] ) {
    std::string model = "IMHL-K0DU-2U0I-J532-25J9";
    std::string method = "com.example.test.Playground.size_100x2";

    std::vector<std::string> data_export = nwise_export(model, method);
    auto data_generate = nwise_generate(model, method);

    framework::master_test_suite().add( BOOST_PARAM_TEST_CASE( &presentExport, data_export.begin(), data_export.end() ) );
    framework::master_test_suite().add( BOOST_PARAM_TEST_CASE( &presentGenerate, data_generate.begin(), data_generate.end() ) );
    
    return 0;
}
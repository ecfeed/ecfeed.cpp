#include <boost/test/included/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
using namespace boost::unit_test;
#include <iostream>
#include <vector>

// https://www.boost.org/doc/libs/1_64_0/libs/test/doc/html/boost_test/adv_scenarios/build_utf.html
// http://www.linuxfromscratch.org/blfs/view/svn/general/boost.html
// https://www.boost.org/doc/libs/1_64_0/libs/test/doc/html/boost_test/tests_organization/test_cases/param_test.html
// https://www.boost.org/users/download/

// clear && g++ -o test src/testBoost.cpp -lboost_unit_test_framework -static -std=c++17 && ./test

void present( int i )
{
  std::cout << i << std::endl;
}

test_suite* init_unit_test_suite( int /*argc*/, char* /*argv*/[] )
{
    std::vector<int> parameters_list;
    
    parameters_list.push_back( 1 );
	parameters_list.push_back( 5 );
	parameters_list.push_back( 6 );
	parameters_list.push_back( 7 );
	parameters_list.push_back( 15 );

    framework::master_test_suite().add( BOOST_PARAM_TEST_CASE( &present, parameters_list.begin(), parameters_list.end() ) );

  return 0;
}
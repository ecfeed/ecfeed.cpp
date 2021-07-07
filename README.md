# Integration with C++

## Introduction

The following tutorial is an introduction to the C++ runner. Note, that it does not cover the ecFeed basics. Therefore, if you want to learn how to create a sample model and generate a personal keystore, visit the tutorial section on our [webpage](https://ecfeed.com/tutorials).  

Prerequisites:
- Create a test model on the ecFeed webpage.
- Generate a personal keystore named 'security.p12' and put it in the \~/.ecfeed/ directory (Linux users) or in the \~/ecfeed/ directory (Windows users).  

For the complete documentation check the source directly at [GitHub](https://github.com/ecfeed/ecfeed.java).  

The ecFeed library can be found online in the [Conan Center](https://conan.io/center/).  

## Examples

Methods, used in the tutorial, are included the welcome model, which was created during the account registration process. If the model is missing, e.g. it has been deleted by the user, it can be downloaded from [here](https://s3-eu-west-1.amazonaws.com/resources.ecfeed.com/repo/tutorial/Welcome.ect).  

```cpp
#include "ecfeed.hpp"
#include <iostream>

int main() {
    
    std::string model = "XXXX-XXXX-XXXX-XXXX-XXXX";
    std::string method = "QuickStart.test";

    ecfeed::test_provider testProvider(model);

    for (auto& test : *testProvider.generate_nwise(method)) {
        std::cout << test << std::endl;
    }

}
```

Note, that the C++ runner requires additional libraries, namely 'libcurl' and 'openssl'. They can be installed on the system or downloaded from a remote package manager, for example [Conan Center](https://conan.io/center/). A sample 'conanfile.txt' might look as follows:

```
[requires]
    libcurl/7.72.0
    openssl/1.1.1c
    ecfeed/1.1.0
[generators]
    cmake
[options]
```

The CMake script (CMakeLists.txt), used in this example, was defined as:

```
cmake_minimum_required(VERSION 3.20)

project(ecfeed)

set(CMAKE_CXX_COMPILER_VERSION 8)

add_compile_options(-std=c++2a)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(ecfeed src/main.cpp)

target_link_libraries(${PROJECT_NAME} stdc++fs)
target_link_libraries(ecfeed CONAN_PKG::ecfeed CONAN_PKG::ecfeed)
target_link_libraries(ecfeed CONAN_PKG::libcurl CONAN_PKG::libcurl)
target_link_libraries(ecfeed CONAN_PKG::openssl CONAN_PKG::openssl)
```

Do not hesitate to experiment with the code and modify the welcome model.    

Also, have in mind that the ID of each model (including the welcome model) is unique. If you want to copy and paste the above example, be sure to update it accordingly.

## Google Test

The ecFeed library can be used to create test cases for GTest, which is one of the most common testing frameworks for C++:

```cpp
#include "gtest/gtest.h"
#include "ecfeed.hpp"
#include <iostream>

namespace test_generate {
   ecfeed::test_provider testProvider("XXXX-XXXX-XXXX-XXXX-XXXX");
}

class FixtureGenerate : public ::testing::TestWithParam<ecfeed::test_arguments> {};

INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn(test_generate::testProvider.generate_nwise("QuickStart.test")->to_list()));
TEST_P(FixtureGenerate, NWise_Generate) {
   std::cout << GetParam() << std::endl;
}
```

## Catch2 Test

Another common testing framework, which can be used with the C++ runner, is Catch2:

```cpp
#include "catch2/catch.hpp"
#include "ecfeed.hpp"
#include <iostream>

namespace test_generate {
    ecfeed::test_provider testProvider("XXXX-XXXX-XXXX-XXXX-XXXX");
}

TEST_CASE("EcFeed test generate", "[NWise generate]") {
    auto i = GENERATE(from_range(test_generate::testProvider.generate_nwise("QuickStart.test")->to_list()));
    std::cout << i << std::endl;
}
```

## Boost Test

Also, the Boost testing framework can be used:

```cpp
#define BOOST_TEST_DYN_LINK
#include <boost/test/included/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include "ecfeed.hpp"
#include <iostream>
#include <vector>

using namespace boost::unit_test;

void present( ecfeed::test_arguments i ) {
  std::cout << i << std::endl;
}

test_suite* init_unit_test_suite( int argc, char* argv[] ) {
    ecfeed::test_provider testProvider("XXXX-XXXX-XXXX-XXXX-XXXX");
    std::vector<ecfeed::test_arguments> data = testProvider.generate_nwise("QuickStart.test")->to_list();

    framework::master_test_suite().add( BOOST_PARAM_TEST_CASE( &present, data.begin(), data.end() ) );

  return 0;
}
```

## Feedback

To send feedback, you need to have a BASIC account type or be a member of a TEAM.  

An example looks as follows:
```cpp
#include "gtest/gtest.h"
#include "ecfeed.hpp"

namespace test_generate {

    ecfeed::test_provider testProvider("XXXX-XXXX-XXXX-XXXX-XXXX");
    std::string method = "com.example.test.Playground.size_10x10";

    auto gen_random_quantity_long = ecfeed::params_random()
      .feedback(true).length(1000 + (rand() % 4000)).label("Random / Quantity - Long");

    void validate(const ecfeed::test_arguments& test_arguments) {

        bool result = (test_arguments.get_string(0) != "a0") && (test_arguments.get_string(1) != "b1") && (test_arguments.get_string(7) != "h6");
        test_arguments.get_handle()->add_feedback(result);
        ASSERT_TRUE(result);
    }

    class FixtureGenerate : public ::testing::TestWithParam<ecfeed::test_arguments> {};

    INSTANTIATE_TEST_CASE_P(gen_random_quantity_long, FixtureGenerate, ::testing::ValuesIn((test_generate::testProvider.generate_random(test_generate::method, test_generate::gen_random_quantity_long))->to_list()));
    TEST_P(FixtureGenerate, gen_random_quantity_long) {
   
        test_generate::validate(GetParam());
    }
```

In this case, one additional field can be accessed from the 'test\_arguments' object, namely 'test\_handle' (using the 'get\_handle()' method). It contains the following public methods:  

```C++
  std::string add_feedback(bool status, int duration = -1, std::string comment = "", std::map<std::string, std::string> custom = std::map<std::string, std::string>());
  std::string add_feedback(bool status, int duration, std::map<std::string, std::string> custom);
  std::string add_feedback(bool status, std::string comment, std::map<std::string, std::string> custom);
```

The parameters are as follows:
- _status_ - The result of the test.
- _comment_ - The optional test execution description.
- _duration_ - The optional test execution time (in milliseconds).
- _custom_ - The optional map of custom key-value pairs.

Note, that each test execution must return a feedback, regardless whether it has passed or failed. Only the first execution of the 'add\_feedback' method takes effect.  

Additionally, to the test generation method one optional argument can be added, namely 'label'. It provides a short description of the generated test suite.  

After successfully sending the execution feedback, it can be accessed (and analyzed) through the website.

# test_provider class API

The library provides connectivity with the ecFeed test generation service using the 'test\_provider' class. It requires the model ID, the keystore location (optional), the keystore password (optional), and the generator service address (optional).  

## Constructor

The 'test\_provider' constructor takes one required and three optional parameters. If they are not provided, default values are used (which, for the vast majority of cases, are sufficient).  

- *model* - The model ID. It is a 20 digit number (grouped by 4) that can be found in the 'my projects' section on the 'ecfeed.com' page. It can be also found in the URL of the model editor page.
```cpp
ecfeed::test_provider testProvider("XXXX-XXXX-XXXX-XXXX-XXXX");
```
- *keystore_path* - The path to the keystore downloaded from the 'ecfeed.com' webpage ('Settings' -> 'Security'). The keystore contains the user certificate which is needed to authenticate the user at the generator service. By default, the constructor looks for the keystore in \~/.ecfeed/security.p12, except for Windows, where the default path is \~/ecfeed/security.p12.
```cpp
ecfeed::test_provider testProvider("XXXX-XXXX-XXXX-XXXX-XXXX", "home/user/security.p12");
```
- *genserver* - The URL of the ecfeed generator service. By default, it is 'gen.ecfeed.com'.
```cpp
ecfeed::test_provider testProvider("XXXX-XXXX-XXXX-XXXX-XXXX", "home/user/security.p12", "gen.ecfeed.com");
```
- *keystore_password* - The password for the keystore. The default value is 'changeit' which is the password used to encrypt the keystore downloaded form the 'ecfeed.com' page.
```cpp
ecfeed::test_provider testProvider("XXXX-XXXX-XXXX-XXXX-XXXX", "home/user/security.p12", "gen.ecfeed.com", "changeit");
```

## Generator calls

'test\_provider' can invoke five methods to access the ecFeed generator service. They produce data parsed to 'std::shared\_ptr<test\_queue<test\_arguments>>'. Additional parameters can be included in the configuration class.

### generate_nwise(const std::string& method, params_nwise options = params_nwise())

Generate test cases using the NWise algorithm.  

Arguments:
- *method (required)* - The full name of the method that will be used for generation (including the package). If the method is not overloaded, its parameters are not required.
- *n* - The 'N' value required in the NWise algorithm. The default is 2 (pairwise).
- *coverage* - The percentage of N-tuples that the generator will try to cover. The default is 100.
- *choices* - A map in which keys are names of method parameters. Their values define a list of choices that should be used during the generation process. If an argument is skipped, all choices are used.
- *constraints* - An array of constraints used for the generation. If not provided, all constraints are used. Additionally, two string values can be used instead, i.e. "ALL", "NONE".

```cpp
int main() {
    int n = 2;
    int coverage = 100;
    std::set<std::string> constraints = {"constraint1"};
    std::map<std::string, std::set<std::string>> choices = {{"arg1", {"choice1", "choice2"}}, {"arg2", {"choice1", "choice2"}}};

    ecfeed::params_nwise args = ecfeed::params_nwise().n(n).coverage(coverage).constraints(constraints).choices(choices);

    std::string model = "XXXX-XXXX-XXXX-XXXX-XXXX";
    std::string method = "QuickStart.test";

    ecfeed::test_provider testProvider(model);

    for (auto& test : *testProvider.generate_nwise(method, args)) {
        std::cout << test << std::endl;
    }
}
```

### generate_pairwise(const std::string& method, params_pairwise options = params_pairwise())

Calls nwise with n=2. For people that like being explicit. Uses the same arguments as 'generate\_nwise' excluding 'n'.

### generate_cartesian(const std::string& method, params_cartesian options = params_cartesian())

Generate test cases using the Cartesian product.  

Arguments:
- *method (required)* - See 'generate\_nwise'.
- *choices* - See 'generate\_nwise'.
- *constraints* - See 'generate\_nwise'.

### generate_random(const std::string& method, params_random options = params_random())

Generate randomized test cases.  

Arguments:
- *method (required)* - See 'generate\_nwise'.
- *length* - The number of tests to be generated. The default is 1.
- *duplicates* - If two identical tests are allowed to be generated. If set to 'false', the generator will stop after creating all allowed combinations. The default is 'true'.
- *adaptive* - If set to true, the generator will try to provide tests that are farthest (in the means of the Hamming distance) from the ones already generated. The default is 'false'.
- *choices* - See 'generate\_nwise'.
- *constraints* - See 'generate\_nwise'.

### generate_static(const std::string& method, params_static options = params_static())

Download generated test cases (do not use the generator).  

Arguments:
- *method (required)* - See 'generate\_nwise'.
- *test\_suites* - An array of test case names to be downloaded. Additionally, one string value can be used instead, i.e. "ALL".

## Export calls

Those methods look similarly to 'generate' methods. However, they return 'std::shared_ptr<test_queue<std::string>>', do not parse the data, and generate the output using templates. For this reason, they require one more argument which should be added to the configuration class, namely 'template' (if it is non-existent, the CSV format is used by default). The predefined values are: 'ecfeed::template_type::json', 'ecfeed::template_type::xml', 'ecfeed::template_type::gherkin', 'ecfeed::template_type::csv', 'ecfeed::template_type::raw'. 

```cpp
std::shared_ptr<test_queue<std::string>> export_nwise(const std::string& method, params_nwise options = params_nwise())
std::shared_ptr<test_queue<std::string>> export_pairwise(const std::string& method, params_pairwise options = params_pairwise())
std::shared_ptr<test_queue<std::string>> export_cartesian(const std::string& method, params_cartesian options = params_cartesian())
std::shared_ptr<test_queue<std::string>> export_random(const std::string& method, params_random options = params_random())
std::shared_ptr<test_queue<std::string>> export_static(const std::string& method, params_static options = params_static())
```

## Other methods

The following section describes helper methods.

### get_argument_types(const std::string& method)

Gets the types of the method parameters in the on-line model.

###  get_argument_names(const std::string& method)

Gets the names of the method parameters in the on-line model.

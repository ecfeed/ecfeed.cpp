# Integration with C++

## Contents

<div class="menu">
	<a class="plain" href="#introduction">Introduction</a></br>
	<a class="plain" href="#examples">Examples</a></br>
	<a class="plain" href="#google">Google Test</a></br>
    <a class="plain" href="#catch2">Catch2</a></br>
    <a class="plain" href="#boost">Boost Test</a></br>
</div>

### <a name="introduction">Introduction</a>

The following tutorial is an introduction to the C++ runner. Note, that it does not cover the ecFeed basics. Therefore, if you want to learn how to create a sample model and generate a personal keystore, visit the tutorial section on our [webpage](https://ecfeed.com/tutorials).  

Prerequisites:
- Create a test model on the ecFeed webpage.
- Generate a personal keystore named 'security.p12' and put it in the \~/.ecfeed/ directory (Linux users) or in the \~/ecfeed/ directory (Windows users).  

For the complete documentation check the source directly at [GitHub](https://github.com/ecfeed/ecfeed.java).  

The ecFeed library can be found online in the [Conan Center](https://conan.io/center/).  

### <a name="examples">Examples</a>

Methods, used in the tutorial, are included the welcome model, created during the account registration process. If the model is missing, e.g. it has been deleted by the user, it can be downloaded from [here](https://s3-eu-west-1.amazonaws.com/resources.ecfeed.com/repo/tutorial/Welcome.ect).  

```cpp
#include "ecfeed.hpp"
#include <iostream>

int main() {
    
    std::string model = "IMHL-K0DU-2U0I-J532-25J9";
    std::string method = "QuickStart.test";

    ecfeed::test_provider testProvider(model);

    for (auto& test : *testProvider.generate_nwise(method)) {
        std::cout << test << std::endl;
    }

}
```

Note, that the C++ runner requires additional libraries: 'libcurl' and 'openssl'. They can be installed on the system or downloaded from a remote package manager, for example [Conan Center](https://conan.io/center/). A sample 'conanfile.txt' can look as follows:

```
[requires]
    libcurl/7.72.0
    openssl/1.1.1c
    ecfeed/1.0
[generators]
    cmake
[options]
```

The CMake script (CMakeLists.txt), used in this example, was defined as:

```
cmake_minimum_required(VERSION 3.5)
project(ecfeed)

add_compile_options(-std=c++17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(ecfeed main.cpp)

target_link_libraries(ecfeed CONAN_PKG::ecfeed CONAN_PKG::ecfeed)
target_link_libraries(ecfeed CONAN_PKG::libcurl CONAN_PKG::libcurl)
target_link_libraries(ecfeed CONAN_PKG::openssl CONAN_PKG::openssl)
```

Do not hesitate to experiment with the code and modify the welcome mode. It can be recreated easily and there is no better way to learn than hands-on exercises.  
<br/>

Also, have in mind that the ID of each model (including the welcome model) is unique. If you want to copy and paste the above example, be sure to update it accordingly.

### <a name="google">Google Test</a>

The ecFeed library can be used to create test cases for GTest, which is one of the most common testing frameworks for C++:

```cpp
#include "gtest/gtest.h"
#include "ecfeed.hpp"
#include <iostream>

namespace test_generate {
   ecfeed::test_provider testProvider("IMHL-K0DU-2U0I-J532-25J9");
}

class FixtureGenerate : public ::testing::TestWithParam<ecfeed::test_arguments> {};

INSTANTIATE_TEST_CASE_P(Generate, FixtureGenerate, ::testing::ValuesIn(test_generate::testProvider.generate_nwise("QuickStart.test")->to_list()));
TEST_P(FixtureGenerate, NWise_Generate) {
   std::cout << GetParam() << std::endl;
}
```

### <a name="catch2">Catch2</a>

Another common testing framework, which can be used with the C++ runner, is Catch2:

```cpp
#include "catch2/catch.hpp"
#include "ecfeed.hpp"
#include <iostream>

namespace test_generate {
    ecfeed::test_provider testProvider("IMHL-K0DU-2U0I-J532-25J9");
}

TEST_CASE("EcFeed test generate", "[NWise generate]") {
    auto i = GENERATE(from_range(test_generate::testProvider.generate_nwise("QuickStart.test")->to_list()));
    std::cout << i << std::endl;
}
```

### <a name="boost">Boost Test</a>

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
    ecfeed::test_provider testProvider("IMHL-K0DU-2U0I-J532-25J9");
    std::vector<ecfeed::test_arguments> data = testProvider.generate_nwise("QuickStart.test")->to_list();

    framework::master_test_suite().add( BOOST_PARAM_TEST_CASE( &present, data.begin(), data.end() ) );

  return 0;
}
```

<br/><br/>

# test_provider class API

## Contents

<div class="menu">
	<a class="plain" href="#constructor">Constructor</a></br>
	<a class="plain" href="#generatorcalls">Generator calls</a></br>
	<a class="plain" href="#exportcalls">Export calls</a></br>
	<a class="plain" href="#othermethodsit">Other methods</a></br>
</div>

<br/><br/>

The library provides connectivity with the ecFeed test generation service using the 'test\_provider' class. It requires the model ID, the keystore location (optional), the keystore password (optional), and the generator service address (optional).  

### <a name="constructor">Constructor</a>

The 'test\_provider' constructor takes one required and three optional parameters. If they are not provided, default values are used (which, for the vast majority of cases, are sufficient).  

- *model* - The model ID. It is a 20 digit number (grouped by 4) that can be found in the 'My projects' page at 'ecfeed.com'. It can be also found in the URL of the model editor page.
```cpp
ecfeed::test_provider testProvider("IMHL-K0DU-2U0I-J532-25J9");
```
- *keystore_path* - The path to the keystore downloaded from the 'ecfeed.com' webpage ('Settings' -> 'Security'). The keystore contains the user certificate which is needed to authenticate the user at the generator service. By default, the constructor looks for the keystore in \~/.ecfeed/security.p12, except for Windows, where the default path is \~/ecfeed/security.p12.
```cpp
ecfeed::test_provider testProvider("IMHL-K0DU-2U0I-J532-25J9", "home/user/security.p12");
```
- *genserver* - The URL of the ecfeed generator service. By default, it is 'gen.ecfeed.com'.
```cpp
ecfeed::test_provider testProvider("IMHL-K0DU-2U0I-J532-25J9", "home/user/security.p12", "gen.ecfeed.com");
```
- *keystore_password* - The password for the keystore. The default value is 'changeit' which is the password used to encrypt the keystore downloaded form the 'ecfeed.com' page.
```cpp
ecfeed::test_provider testProvider("IMHL-K0DU-2U0I-J532-25J9", "home/user/security.p12", "gen.ecfeed.com", "changeit");
```

### <a name="generatorcalls">Generator calls</a>

'test\_provider' can invoke five methods to access the ecFeed generator service. They produce data parsed to 'std::shared\_ptr<test\_queue<test\_arguments>>'. Additional parameters can be included in the configuration map.
<br/><br/>

<div class="tableHeader">std::shared_ptr&lt;test_queue&lt;test_arguments&gt;&gt; generate_nwise(const std::string& method, std::map&lt;std::string, std::any&gt; options = {})</div>

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

    std::map<std::string, std::any> optionsGenerateNWise = {{"n", n}, {"coverage", coverage}, {"constraints", constraints}, {"choices", choices}};

    std::string model = "IMHL-K0DU-2U0I-J532-25J9";
    std::string method = "QuickStart.test";

    ecfeed::test_provider testProvider(model);

    for (auto& test : *testProvider.generate_nwise(method, optionsGenerateNWise)) {
        std::cout << test << std::endl;
    }
}
```
<br/><br/>

<div class="tableHeader">std::shared_ptr&lt;test_queue&lt;test_arguments&gt;&gt; generate_pairwise(const std::string& method, std::map&lt;std::string, std::any&gt; options = {})</div>

Calls nwise with n=2. For people that like being explicit. Uses the same arguments as 'generate\_nwise' excluding 'n'.

<br/><br/>

<div class="tableHeader">std::shared_ptr&lt;test_queue&lt;test_arguments&gt;&gt; generate_cartesian(const std::string& method, std::map&lt;std::string, std::any&gt; options = {})</div>

Generate test cases using the Cartesian product.  

Arguments:
- *method (required)* - See 'generate\_nwise'.
- *choices* - See 'generate\_nwise'.
- *constraints* - See 'generate\_nwise'.
<br/><br/>

<div class="tableHeader">std::shared_ptr&lt;test_queue&lt;test_arguments&gt;&gt; generate_random(const std::string& method, std::map&lt;std::string, std::any&gt; options = {})</div>

Generate randomized test cases.  

Arguments:
- *method (required)* - See 'generate\_nwise'.
- *length* - The number of tests to be generated. The default is 1.
- *duplicates* - If two identical tests are allowed to be generated. If set to 'false', the generator will stop after creating all allowed combinations. The default is 'true'.
- *adaptive* - If set to true, the generator will try to provide tests that are farthest (in the means of the Hamming distance) from the ones already generated. The default is 'false'.
- *choices* - See 'generate\_nwise'.
- *constraints* - See 'generate\_nwise'.
<br/><br/>
std::shared_ptr&lt;test_queue&lt;test_arguments&gt;&gt; generate_static(const std::string& method, std::map&lt;std::string, std::any&gt; options = {})</div>

Download generated test cases (do not use the generator).  

Arguments:
- *method (required)* - See 'generate\_nwise'.
- *test\_suites* - An array of test case names to be downloaded. Additionally, one string value can be used instead, i.e. "ALL".

### <a name="exportcalls">Export calls</a>


Those methods look similarly to 'generate' methods. However, they return 'std::shared_ptr<test_queue<std::string>>', do not parse the data, and generate the output using templates. For this reason, they require one more argument which should be added to the 'options' map, namely 'template' (if it is non-existent, the JSON format is used). The predefined values are: 'ecfeed::template_type::json', 'ecfeed::template_type::xml', 'ecfeed::template_type::gherkin', 'ecfeed::template_type::csv', 'ecfeed::template_type::raw'. 

```cpp
std::shared_ptr<test_queue<std::string>> export_nwise(const std::string& method, std::map<std::string, std::any> options = {})
std::shared_ptr<test_queue<std::string>> export_pairwise(const std::string& method, std::map<std::string, std::any> options = {})
std::shared_ptr<test_queue<std::string>> export_cartesian(const std::string& method, std::map<std::string, std::any> options = {})
std::shared_ptr<test_queue<std::string>> export_random(const std::string& method, std::map<std::string, std::any> options = {})
std::shared_ptr<test_queue<std::string>> export_random(const std::string& method, std::map<std::string, std::any> options = {})
```

### <a name="othermethods">Other methods</a>

The following section describes helper methods.<br/><br/>

<div class="tableHeader">std::vector&lt;std::string&gt; get_argument_types(const std::string& method)</div>

Gets the types of the method parameters in the on-line model.<br/><br/>

<div class="tableHeader">std::vector&lt;std::string&gt; get_argument_names(const std::string& method)</div>

Gets the names of the method parameters in the on-line model.
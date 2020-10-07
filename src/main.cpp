#include<curl/curl.h>
#include"picojson.h"
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<vector>
#include<list>
#include<map>
#include<set>
#include<queue>
#include<unordered_map>
#include<openssl/pem.h>
#include<openssl/err.h>
#include<openssl/pkcs12.h>
#include<cstdio>
#include<filesystem>
#include<cstdlib>
#include<ctime>
#include<typeindex>
#include<typeinfo>
#include<functional>
#include<regex>
#include<sstream>
#include<any>
#include<future>
#include<tuple>
#include "testArguments.cpp"

struct memory {
    char *response;
    size_t size;
};

namespace ecfeed
{

static bool _curl_initialized = false;

enum class TemplateType
{
    CSV = 1,
    XML = 2,
    Gherkin = 3,
    JSON = 4,
    RAW = 5,
};

static std::string template_type_url_param(const TemplateType& t)
{
    switch(t)
    {
    case TemplateType::CSV:
        return "CSV";
    case TemplateType::XML:
        return "XML";
    case TemplateType::Gherkin:
        return "Gherkin";
    case TemplateType::JSON:
        return "JSON";
    }

    return "UNKNOWN";
}

enum class DataSource
{
    STATIC_DATA = 1,
    NWISE = 2,
    CARTESIAN = 3,
    RANDOM = 4,
};

static std::string data_source_url_param(const DataSource& s)
{
    switch(s){
    case DataSource::STATIC_DATA:
        return "static";
    case DataSource::NWISE:
        return "genNWise";
    case DataSource::CARTESIAN:
        return "genCartesian";
    case DataSource::RANDOM:
        return "genRandom";
    }
    return "UNKNOWN";
}

namespace options{

    std::string serialize(const std::any&);
    std::string serialize(const std::pair<std::string, std::any>&v);
    static std::unordered_map<std::type_index, std::function<std::string(const std::any&)>> _serializers;

    template<typename T> 
    std::function<std::string(const std::any&)> single_value_serializer = [](const std::any& v){
        return "'" + std::to_string(std::any_cast<T>(v)) + "'";
    };

    template<typename COMPOSITE_TYPE>
    std::function<std::string(const std::any&)> composite_value_serializer = [](const std::any& v){
        std::string result = "[";
        std::string padding = "";
        for(auto& e : std::any_cast<COMPOSITE_TYPE>(v)){
            result += padding + serialize(e);
            padding = ",";
        }
        return result + "]";
    };

    template<typename VALUE_TYPE>
    std::function<std::string(const std::any&)> map_serializer = [](const std::any& v){
        std::string result = "{";
        std::string padding = "";
        auto casted = std::any_cast<std::map<std::string, VALUE_TYPE>>(v);
        for(auto& e : casted){
            result += padding + "'" + e.first + "':" + serialize(e.second);
            padding = ",";
        }
        return result + "}";
    };


    void init_serializers()
    {
        _serializers[std::type_index(typeid(unsigned))] = single_value_serializer<unsigned>;
        _serializers[std::type_index(typeid(int))] = single_value_serializer<int>;
        _serializers[std::type_index(typeid(bool))] = [](const std::any& v){
            return std::string("'") + (std::any_cast<bool>(v) ? "true" : "false") + "'";
        };
        _serializers[std::type_index(typeid(std::string))] = [](const std::any& v){
            return "'" + std::any_cast<std::string>(v) + "'";
        };
        _serializers[std::type_index(typeid(DataSource))] = [](const std::any& v){
            return "'" + data_source_url_param(std::any_cast<DataSource>(v)) + "'";
        };
        _serializers[std::type_index(typeid(TemplateType))] = [](const std::any& v){
            return "'" + template_type_url_param(std::any_cast<TemplateType>(v)) + "'";
        };
        _serializers[std::type_index(typeid(std::set<std::string>))] = composite_value_serializer<std::set<std::string>>;
        _serializers[std::type_index(typeid(std::list<std::string>))] = composite_value_serializer<std::list<std::string>>;
        _serializers[std::type_index(typeid(std::vector<std::string>))] = composite_value_serializer<std::vector<std::string>>;

        _serializers[std::type_index(typeid(std::map<std::string, std::set<std::string>>))] = [](const std::any& v){
            std::string result = "{";
            std::string padding = "";
            auto casted = std::any_cast<std::map<std::string, std::set<std::string>>>(v);
            for(auto& e : casted){
                //          for(auto& e : std::any_cast<std::map<std::string, std::set<std::string>>(v)){
                result += padding + "'" + e.first + "':" + serialize(e.second);
                padding = ",";
            }
            return result + "}";
        };
        _serializers[std::type_index(typeid(std::map<std::string, std::any>))] = [](const std::any& v){
            std::string result = "{";
            std::string padding = "";
            auto casted = std::any_cast<std::map<std::string, std::any>>(v);
            for(auto& e : casted){
                //          for(auto& e : std::any_cast<std::map<std::string, std::set<std::string>>(v)){
                result += padding + "'" + e.first + "':" + serialize(e.second);
                padding = ",";
            }
            return result + "}";
        };
    }

    std::string serialize(const std::any& v){
        if(auto serializer = _serializers.find(v.type()); serializer != _serializers.end()){
            return serializer->second(v);
        }
        else{
            std::cerr << "Cannot serialize option. Serializer for given type not registered.\n";
            return "";
        }
    };

    std::string serialize(const std::pair<std::string, std::any>&v){
        return "'" + v.first + "':" + serialize(v.second);
    }

}

static size_t curl_data_callback(void *data, size_t size, size_t nmemb, void *userp){
    auto callback = static_cast<std::function<size_t(void *data, size_t size, size_t nmemb)>*>(userp);
    return callback->operator()(data, size, nmemb);
}

class TestProvider;

#include "testQueue.cpp"
#include "testProvider.cpp"

}//namespace ecfeed

// int main(int argc, char** argv){
// g++-8 -pthread -std=c++17 -o ecfeed src/main.cpp -lcurl -lcrypto -lstdc++fs && ./ecfeed
// auto q_0 = tp.exportNwise("QuickStart.test"); // Ambiguous.

    // ecfeed::TestProvider tp(
    //     "ZCPH-DFYI-R7R7-R6MM-89L8", 
    //     "/home/krzysztof/Desktop/git/ecfeed.java/com.ecfeed.runner/src/test/resources/security.p12", 
    //     "develop-gen.ecfeed.com"
    // );

    // for(std::string test : *tp.exportNwise("QuickStart.test", ecfeed::TemplateType::CSV)) { std::cout << test << std::endl; }
    // for(auto test : *tp.exportNwise("QuickStart.test", ecfeed::TemplateType::XML)) { std::cout << test << std::endl; }
    // for(auto test : *tp.exportNwise("QuickStart.test", ecfeed::TemplateType::JSON)) { std::cout << test << std::endl; }
    // for(auto test : *tp.exportNwise("QuickStart.test", ecfeed::TemplateType::Gherkin)) { std::cout << test << std::endl; }
    // for(auto test : *tp.exportNwise("QuickStart.test", ecfeed::TemplateType::RAW)) { std::cout << test << std::endl; }

    // int n = 2;
    // int coverage = 100;
    // int length = 50;    // The generation should be stopped before reaching the limit.
    // bool duplicates = true;
    // bool adaptive = true;
    // ecfeed::TemplateType template_type = ecfeed::TemplateType::CSV;
    // std::set<std::string> constraints = {"constraint1"};    // Missing std::string options.
    // std::set<std::string> test_suites = {"suite1"};    // Missing std::string options.
    // std::map<std::string, std::set<std::string>> choices = {{"arg1", {"choice1", "choice2"}}, {"arg2", {"choice1"}}};   // Missing std::string options.
    
    // std::map<std::string, std::any> optionsNWise = {{"template", template_type}, {"coverage", coverage}, {"n", n}, {"constraints", constraints}, {"choices", choices}};
    // for(auto test : *tp.exportNwise("QuickStart.test", template_type, n, coverage, constraints, choices)) { std::cout << test << std::endl; }
    // for(auto test : *tp.exportNwise("QuickStart.test", optionsNWise)) { std::cout << test << std::endl; }
    
    // std::map<std::string, std::any> optionsPairwise = {{"template", template_type}, {"coverage", coverage}, {"constraints", constraints}, {"choices", choices}};
    // for(auto test : *tp.exportPairwise("QuickStart.test", optionsPairwise)) { std::cout << test << std::endl; }

    // std::map<std::string, std::any> optionsRandom = {{"template", template_type}, {"length", length}, {"duplicates", duplicates}, {"adaptive", adaptive}, {"constraints", constraints}, {"choices", choices}};
    // for(auto test : *tp.exportRandom("QuickStart.test", optionsRandom)) { std::cout << test << std::endl; }
    
    // std::map<std::string, std::any> optionsCartesian = {{"template", template_type}, {"constraints", constraints}, {"choices", choices}};
    // for(auto test : *tp.exportCartesian("QuickStart.test", optionsCartesian)) { std::cout << test << std::endl; }
    
    // for(auto test : *tp.exportStatic("QuickStart.test", test_suites)) { std::cout << test << std::endl; }   // Shouldn't it be like everything else, i.e 'options'?
    // We can use test suites with constraints/choices.


//     std::map<std::string, std::any> optionsNWise = {{"coverage", coverage}, {"n", n}, {"constraints", constraints}, {"choices", choices}};
//     for(auto test : *tp.generateNwise("QuickStart.test", optionsNWise)) { 
      
//         std::cout << "test" << std::endl;
//      }

//     return 0;
// }

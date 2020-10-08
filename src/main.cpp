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
#include <unistd.h>
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

}

// int main(int argc, char** argv){

//     ecfeed::TestProvider tp("V0G6-MHNQ-PDSR-G2WB-XOKV");

//     for (auto test : *tp.generateNwise("QuickStart.test")) { 
//         std::cout << test << std::endl;
//      }

//     return 0;
// }
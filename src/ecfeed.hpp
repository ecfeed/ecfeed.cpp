#pragma once
#include "picojson.h"
#include <curl/curl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <unordered_map>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>
#include <cstdio>
#include <filesystem>
#include <cstdlib>
#include <ctime>
#include <typeindex>
#include <typeinfo>
#include <functional>
#include <regex>
#include <sstream>
#include <any>
#include <future>
#include <tuple>
#include <unistd.h>

#define DEBUG

#ifdef DEBUG 
#define VERBOSE(x) (x)
#else 
#define VERBOSE(x) do { } while(0)
#endif

struct memory 
{
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
        switch(s) {
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

    namespace options
    {
        std::string serialize(const std::any&);
        std::string serialize(const std::pair<std::string, std::any>&v);
        static std::unordered_map<std::type_index, std::function<std::string(const std::any&)>> _serializers;

        template<typename T> 
        std::function<std::string(const std::any&)> single_value_serializer = [](const std::any& v)
        {
            return "'" + std::to_string(std::any_cast<T>(v)) + "'";
        };

        template<typename COMPOSITE_TYPE>
        std::function<std::string(const std::any&)> composite_value_serializer = [](const std::any& v)
        {
            std::string result = "[";
            std::string padding = "";
            for (auto& e : std::any_cast<COMPOSITE_TYPE>(v)) {
                result += padding + serialize(e);
                padding = ",";
            }
            return result + "]";
        };

        template<typename VALUE_TYPE>
        std::function<std::string(const std::any&)> map_serializer = [](const std::any& v)
        {
            std::string result = "{";
            std::string padding = "";
            auto casted = std::any_cast<std::map<std::string, VALUE_TYPE>>(v);
            for (auto& e : casted) {
                result += padding + "'" + e.first + "':" + serialize(e.second);
                padding = ",";
            }
            return result + "}";
        };


        void init_serializers()
        {
            _serializers[std::type_index(typeid(unsigned))] = single_value_serializer<unsigned>;
            _serializers[std::type_index(typeid(int))] = single_value_serializer<int>;
            _serializers[std::type_index(typeid(bool))] = [](const std::any& v) {
                return std::string("'") + (std::any_cast<bool>(v) ? "true" : "false") + "'";
            };
            _serializers[std::type_index(typeid(std::string))] = [](const std::any& v) {
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
            _serializers[std::type_index(typeid(std::map<std::string, std::set<std::string>>))] = [](const std::any& v) {
                std::string result = "{";
                std::string padding = "";
                auto casted = std::any_cast<std::map<std::string, std::set<std::string>>>(v);
                for (auto& e : casted) {
                    result += padding + "'" + e.first + "':" + serialize(e.second);
                    padding = ",";
                }
                return result + "}";
            };
            _serializers[std::type_index(typeid(std::map<std::string, std::any>))] = [](const std::any& v){
                std::string result = "{";
                std::string padding = "";
                auto casted = std::any_cast<std::map<std::string, std::any>>(v);
                for(auto& e : casted) {
                    result += padding + "'" + e.first + "':" + serialize(e.second);
                    padding = ",";
                }
                return result + "}";
            };
        }

        std::string serialize(const std::any& v)
        {
            if (auto serializer = _serializers.find(v.type()); serializer != _serializers.end()) {
                return serializer->second(v);
            }
            else {
                std::cerr << "Cannot serialize option. Serializer for given type not registered.\n";
                return "";
            }
        };

        std::string serialize(const std::pair<std::string, std::any>&v)
        {
            return "'" + v.first + "':" + serialize(v.second);
        }

    }

    static size_t curl_data_callback(void *data, size_t size, size_t nmemb, void *userp)
    {
        auto callback = static_cast<std::function<size_t(void *data, size_t size, size_t nmemb)>*>(userp);
        return callback->operator()(data, size, nmemb);
    }

    struct MethodInfo
    {
        std::vector<std::string> arg_names;
        std::vector<std::string> arg_types;
    };

    class TestArguments 
    {
        std::vector<std::tuple<std::string, std::string, std::string>> core;

        friend std::ostream& operator<<(std::ostream& os, const TestArguments& testArguments);

    public:

        void add(std::string name, std::string type, std::string value) 
        {
            core.push_back(std::make_tuple(name, type, value));
        }

        template<typename T>
        T get(int index) const 
        {
            std::tuple<std::string, std::string, std::string> argument = core.at(index);

            return parse<T>(std::get<1>(argument), std::get<2>(argument));
        }

        template<typename T>
        T get(std::string name) const {
            for (int i = 0 ; i < core.size() ; i++) {
                if (std::get<0>(core.at(i)) == name) {
                    return get<T>(i);
                }
            }

            throw std::invalid_argument("Invalid argument name");
        }

        std::string getString(int index) const 
        { 
            return get<std::string>(index); 
        }

        bool getBool(int index) const 
        { 
            return get<bool>(index); 
        }

        double getDouble(int index) const 
        { 
            return get<double>(index); 
        }

        double getFloat(int index) const 
        { 
            return get<float>(index); 
        }

        int getLong(int index) const 
        { 
            return get<long>(index); 
        }

        int getInt(int index) const 
        { 
            return get<int>(index); 
        }

        std::string getString(std::string name) const 
        { 
            return get<std::string>(name); 
        }

        bool getBool(std::string name) const 
        { 
            return get<bool>(name); 
        }

        double getDouble(std::string name) const 
        { 
            return get<double>(name); 
        }

        double getFloat(std::string name) const 
        { 
            return get<float>(name); 
        }

        int getLong(std::string name) const 
        { 
            return get<long>(name); 
        }

        int getInt(std::string name) const 
        { 
            return get<int>(name); 
        }

    private:

        template<typename T>
        T parse(std::string type, std::string value) const 
        {

            if (type == "String") return std::any_cast<T>(value);
            else if (type == "char") return std::any_cast<T>(static_cast<char>(value.at(0)));
            else if (type == "short") return std::any_cast<T>(static_cast<short>(std::stoi(value)));
            else if (type == "byte") return std::any_cast<T>(std::stoi(value));
            else if (type == "int") return std::any_cast<T>(std::stoi(value));
            else if (type == "long") return std::any_cast<T>(std::stol(value));
            else if (type == "float") return std::any_cast<T>(std::stof(value));
            else if (type == "double") return std::any_cast<T>(std::stod(value));
            else if (type == "boolean") return std::any_cast<T>(value == "true");
            else 
            {
                std::cerr << "Unknown parameter type: " << type << ". Converting the parameter to String\n";
                return std::any_cast<T>(value);
            }
        }
    };

    std::ostream& operator<<(std::ostream& os, const TestArguments& testArguments)
    {
        for (auto &x : testArguments.core) {
            os << std::get<1>(x) << " " << std::get<0>(x) << " = " << std::get<2>(x) << "; ";
        }

        return os;
    }

    template<typename T>
    class TestQueue
    {
        class const_iterator : public std::iterator<std::forward_iterator_tag, TestQueue>
        {
            const bool END_ITERATOR;
            TestQueue<T>& _queue;
            
        public:
            
            const_iterator(TestQueue<T>& queue, bool end = false) :
                _queue(queue), END_ITERATOR(end)
            {}

            const_iterator(const const_iterator& other) :
                _queue(other._queue), END_ITERATOR(other.END_ITERATOR)
            {}

            bool operator==(const const_iterator& other) const
            {
                if (END_ITERATOR && other.END_ITERATOR) {
                    return true;
                }

                if (END_ITERATOR || other.END_ITERATOR) {
                    return _queue.done();
                }

                return false;
            }

            bool operator!=(const const_iterator& other) const
            {
                return (*this == other) == false;
            }

            const_iterator& operator++()
            {
                _queue.next();
                return *this;
            }

            const T& operator*()
            {
                return _queue.current_element();
            }
        };

        bool _done;
        std::vector<T> _data;
        const_iterator _begin;
        const_iterator _end;
        std::mutex _mutex;
        std::mutex _cv_mutex;
        std::condition_variable _cv;

        MethodInfo _method_info;
        bool _method_info_ready;

    public:

        typedef T value_type;

        TestQueue() :
            _done(false),
            _begin(*this),
            _end(*this, true),
            _method_info_ready(false)
        {}

        const_iterator begin() const {return _begin;}
        const_iterator end() const {return _end;}

        friend class const_iterator;
        friend class TestProvider;

        bool done() {
            std::unique_lock<std::mutex> cv_lock(_mutex);

            if (_data.size() != 0) {
                return false;
            }
            else if (_done) {
                return true;
            }

            _cv.wait(cv_lock);
            return _done;
        }

        void next() 
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _data.erase(_data.begin());
        }

        void insert(const T& element)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            _data.push_back(element);
            _cv.notify_one();
        }

        T& current_element() 
        {
            std::unique_lock<std::mutex> cv_lock(_mutex);
            if (_data.size() > 0) {
                return _data[0];
            }
            _cv.wait(cv_lock);
            return _data[0];
        }

        void finish()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _done = true;
            _cv.notify_one();
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _data.size() == 0;
        }

        std::vector<T> toList() 
        {
            std::unique_lock<std::mutex> cv_lock(_mutex);

            while (!_done) {
                _cv.wait(cv_lock);
            }

            return _data;
        }

        std::vector<std::string> getArgumentTypes()
        {
            std::unique_lock<std::mutex> cv_lock(_mutex);

            while (!getMethodInfoReady()) {
                _cv.wait(cv_lock);
            }

            return _method_info.arg_types;
        }
        
        std::vector<std::string> getArgumentNames()
        {
            std::unique_lock<std::mutex> cv_lock(_mutex);

            while (!getMethodInfoReady()) {
                _cv.wait(cv_lock);
            }

            return _method_info.arg_names;
        }

    private:

        void setMethodInfoReady()
        {
            _method_info_ready = true;
        }

        bool& getMethodInfoReady() 
        {
            return _method_info_ready;
        }

        MethodInfo& getMethodInfo()
        {
            return _method_info;
        }

    };

    class TestProvider
    {
        std::string _keystore_path;
        const std::string _genserver;
        const std::string _keystore_password;

        std::filesystem::path _temp_dir;
        std::filesystem::path _pkey_path;
        std::filesystem::path _cert_path;
        std::filesystem::path _ca_path;

        CURL* _curl_handle;
        std::list<std::future<void>> _running_requests;

        std::mutex _mutex;

    public:
        std::string model;

        TestProvider(const std::string& model,
                    const std::filesystem::path& keystore_path = "",
                    const std::string& genserver = "gen.ecfeed.com",
                    const std::string& keystore_password = "changeit") :
            model(model), _genserver(genserver),
            _keystore_password(keystore_password),

            _temp_dir(std::filesystem::temp_directory_path()),
            _pkey_path(_temp_dir / _randomFilename()),
            _cert_path(_temp_dir / _randomFilename()),
            _ca_path(_temp_dir   / _randomFilename())
        {
            _keystore_path = getKeyStore(keystore_path);

            OpenSSL_add_all_algorithms();
            ERR_load_CRYPTO_strings();

            if (_curl_initialized == false) {
                curl_global_init(CURL_GLOBAL_ALL);
                _curl_initialized = true;
            }

            _curl_handle = curl_easy_init();

            _dumpKeystore();

            options::init_serializers();
        }

        ~TestProvider()
        {
            std::filesystem::remove(_pkey_path);
            std::filesystem::remove(_cert_path);
            std::filesystem::remove(_ca_path);

            curl_easy_cleanup(_curl_handle);
        }

        std::vector<std::string> getArgumentNames(const std::string& method, const std::string& model = "")
        {
            return generateRandom(method, {{"length", 0}, {"adaptive", false}, {"duplicates", true}})->getArgumentNames();
        }

        std::vector<std::string> getArgumentTypes(const std::string& method, const std::string& model = "")
        {
            return generateRandom(method, {{"length", 0}, {"adaptive", false}, {"duplicates", true}})->getArgumentTypes();
        }

        std::shared_ptr<TestQueue<std::string>> exportNwise(const std::string& method, std::map<std::string, std::any> options = {})
        {
            std::lock_guard<std::mutex> lock(_mutex);

            std::map<std::string, std::any> opt = setup(options, setupNwise(options), DataSource::NWISE, true);
        
            return _export(method, opt);
        }

        std::shared_ptr<TestQueue<TestArguments>> generateNwise(const std::string& method, std::map<std::string, std::any> options = {})
        {
            std::lock_guard<std::mutex> lock(_mutex);

            std::map<std::string, std::any> opt = setup(options, setupNwise(options), DataSource::NWISE, false);

            return _generate(method, opt);
        }

        std::shared_ptr<TestQueue<std::string>> exportPairwise(const std::string& method, std::map<std::string, std::any> options = {})
        {
            std::lock_guard<std::mutex> lock(_mutex);

            std::map<std::string, std::any> opt = setup(options, setupPairwise(options), DataSource::NWISE, true);

            return _export(method, opt);
        }

        std::shared_ptr<TestQueue<TestArguments>> generatePairwise(const std::string& method, std::map<std::string, std::any> options = {})
        {
            std::lock_guard<std::mutex> lock(_mutex);

            std::map<std::string, std::any> opt = setup(options, setupPairwise(options), DataSource::NWISE, false);

            return _generate(method, opt);
        }

        std::shared_ptr<TestQueue<std::string>> exportRandom(const std::string& method, std::map<std::string, std::any> options = {})
        {
            std::lock_guard<std::mutex> lock(_mutex);

            std::map<std::string, std::any> opt = setup(options, setupRandom(options), DataSource::RANDOM, true);

            return _export(method, opt);
        }

        std::shared_ptr<TestQueue<TestArguments>> generateRandom(const std::string& method, std::map<std::string, std::any> options = {})
        {
            std::lock_guard<std::mutex> lock(_mutex);

            std::map<std::string, std::any> opt = setup(options, setupRandom(options), DataSource::RANDOM, false);

            return _generate(method, opt);
        }

        std::shared_ptr<TestQueue<std::string>> exportCartesian(const std::string& method, std::map<std::string, std::any> options = {})
        {
            std::lock_guard<std::mutex> lock(_mutex);

            std::map<std::string, std::any> opt = setup(options, setupCartesian(options), DataSource::CARTESIAN, true);

            return _export(method, opt);
        }

        std::shared_ptr<TestQueue<TestArguments>> generateCartesian(const std::string& method, std::map<std::string, std::any> options = {})
        {
            std::lock_guard<std::mutex> lock(_mutex);

            std::map<std::string, std::any> opt = setup(options, setupCartesian(options), DataSource::CARTESIAN, false);

            return _generate(method, opt);
        }

        std::shared_ptr<TestQueue<std::string>> exportStatic(const std::string& method, std::map<std::string, std::any> options = {})
        {
            std::lock_guard<std::mutex> lock(_mutex);

            std::map<std::string, std::any> opt = setup(options, setupStatic(options), DataSource::STATIC_DATA, true);
            opt["testSuites"] = options["testSuites"];

            return _export(method, opt);
        }

        std::shared_ptr<TestQueue<TestArguments>> generateStatic(const std::string& method, std::map<std::string, std::any> options = {})
        {
            std::lock_guard<std::mutex> lock(_mutex);

            std::map<std::string, std::any> opt = setup(options, setupStatic(options), DataSource::STATIC_DATA, false);
            opt["testSuites"] = options["testSuites"];

            return _generate(method, opt);
        }
        
    private:

        std::string getKeyStore(std::string keystore_path = "") {

            if (keystore_path == "") {
                std::string homepath = getenv("HOME");
                if ( access( (homepath + "/.ecfeed/security.p12").c_str(), F_OK ) != -1 ) {
                    return homepath + "/.ecfeed/security.p12";
                } else if (access( (homepath + "/ecfeed/security.p12").c_str(), F_OK ) != -1) {
                    return homepath + "/ecfeed/security.p12";
                } else {
                    throw std::invalid_argument("Invalid key store path");
                }
            } else {
                return keystore_path;
            }
        }

        std::map<std::string, std::any> setupNwise(std::map<std::string, std::any>& options) 
        {
            std::map<std::string, std::any> properties;

            properties["n"] = options.count("n") ? options["n"] : 2; options.erase("n");
            properties["coverage"] = options.count("coverage") ? options["coverage"] : 100; options.erase("coverage");

            return properties;
        }

        std::map<std::string, std::any> setupPairwise(std::map<std::string, std::any>& options) 
        {
            std::map<std::string, std::any> properties;

            properties["coverage"] = options.count("coverage") ? options["coverage"] : 100; options.erase("coverage");

            return properties;
        }
        
        std::map<std::string, std::any> setupRandom(std::map<std::string, std::any>& options) 
        {
            std::map<std::string, std::any> properties;

            properties["length"] = options.count("length") ? options["length"] : 100; options.erase("length");
            properties["duplicates"] = options.count("duplicates") ? options["duplicates"] : std::set<std::string>({}); options.erase("duplicates");
            properties["adaptive"] = options.count("adaptive") ? options["adaptive"] : std::map<std::string, std::set<std::string>>({}); options.erase("adaptive");

            return properties;
        }

        std::map<std::string, std::any> setupCartesian(std::map<std::string, std::any>& options) 
        {
            std::map<std::string, std::any> properties;

            return properties;
        }

        std::map<std::string, std::any> setupStatic(std::map<std::string, std::any>& options) 
        {
            std::map<std::string, std::any> properties;

            return properties;
        }

        std::map<std::string, std::any> setup(std::map<std::string, std::any> options, std::map<std::string, std::any> properties, DataSource source, bool tmp) 
        {
            std::map<std::string, std::any> opt;

            if (options.count("constraints")) {
                opt["constraints"] =  options["constraints"];
                options.erase("constraints");
            }
            
            if (options.count("choices")) {
                opt["choices"] =  options["choices"];
                options.erase("choices");
            }

            opt["dataSource"] = source;
            opt["properties"] = properties;

            if (tmp) {
                opt["template"] = options.count("template") ? options["template"] : TemplateType::CSV;  options.erase("template");
            }

            if (options.size()) {
                std::cerr << "Unknown options: ";
                for (const auto& option : options) {
                    std::cerr << " " << option.first << " " ;
                }
            }

            return opt;
        }

        void _performRequest(const std::string& url, const std::function<size_t(void *data, size_t size, size_t nmemb)>* data_callback)
        {
            char error_buf[128];
            curl_easy_setopt(_curl_handle, CURLOPT_SSLCERT, _cert_path.string().c_str());
            curl_easy_setopt(_curl_handle, CURLOPT_SSLCERTTYPE, "pem");
            curl_easy_setopt(_curl_handle, CURLOPT_SSLKEY, _pkey_path.string().c_str());
            curl_easy_setopt(_curl_handle, CURLOPT_CAINFO, _ca_path.string().c_str());
            curl_easy_setopt(_curl_handle, CURLOPT_BUFFERSIZE, 8);

            curl_easy_setopt(_curl_handle, CURLOPT_WRITEFUNCTION, curl_data_callback);
            curl_easy_setopt(_curl_handle, CURLOPT_WRITEDATA, (void *)data_callback);

            curl_easy_setopt(_curl_handle, CURLOPT_ERRORBUFFER, error_buf);

            curl_easy_setopt(_curl_handle, CURLOPT_URL, url.c_str());

            auto success = curl_easy_perform(_curl_handle);

            if (success != 0) {
                std::cout << error_buf << std::endl;
            }

        }

        std::shared_ptr<TestQueue<std::string>> _export(const std::string& method, std::map<std::string, std::any>& options)
        {
            auto url = _buildExportUrl(method, options);
            std::shared_ptr<TestQueue<std::string>> result(new TestQueue<std::string>());

            std::function<size_t(void *data, size_t size, size_t nmemb)> data_cb = [result](void *data, size_t size, size_t nmemb) -> size_t {
                if (nmemb > 0) {
                    std::string test((char*) data, (char*) data + nmemb - 1);
                    result->insert(test);
                }
                return nmemb;
            };

            _running_requests.push_back(std::async(std::launch::async, [result, url, data_cb, this](){
                _performRequest(url, &data_cb);
                result->finish();
            }));

            return result;
        }

        std::shared_ptr<TestQueue<TestArguments>> _generate(const std::string& method, std::map<std::string, std::any>& options)
        {
            auto url = _buildGenerateUrl(method, options);

            std::vector<std::string> types;
            std::shared_ptr<TestQueue<TestArguments>> result(new TestQueue<TestArguments>());

            std::function<size_t(void *data, size_t size, size_t nmemb)> data_cb = [this, result](void *data, size_t size, size_t nmemb) -> size_t {
                if (nmemb > 0) {
                    std::string test((char*) data, (char*) data + nmemb - 1);

                    auto [name, value] = _parseTestLine(test);
                    if (name == "info" && value.to_str() != "alive" && !result->getMethodInfoReady()) {
                        std::string method_signature = value.to_str();
                        std::replace(method_signature.begin(), method_signature.end(), '\'', '\"');

                        auto [name_1, value_1] = _parseTestLine(method_signature);
                        if (name_1 == "method") {
                            if (_parseMethodInfo(value_1.to_str(), result->getMethodInfo())) {
                                result->setMethodInfoReady();
                            }
                        }
                    }
                    else if(name == "testCase" && result->getMethodInfoReady()) {
                        result->insert(_parseTestCase(value, result->getMethodInfo()));
                    }

                }
                return nmemb;
            };

            _running_requests.push_back(std::async(std::launch::async, [result, url, data_cb, this]() {
                _performRequest(url, &data_cb);
                result->finish();
            }));

            return result;
        }

        void _dumpKeystore()
        {
            std::FILE* pkey_file = fopen(_pkey_path.string().c_str(), "w");
            std::FILE* cert_file = fopen(_cert_path.string().c_str(), "w");
            std::FILE* ca_file = fopen(_ca_path.string().c_str(), "w");

            FILE* fp;
            EVP_PKEY* pkey;
            X509* cert;
            STACK_OF(X509)* ca = nullptr;
            PKCS12* p12;

            if ((fp = fopen(_keystore_path.c_str(), "rb")) == nullptr) {
                std::cerr << "Can't open the keystore file: " << _keystore_path << "\n";
                exit(1);
            }

            p12 = d2i_PKCS12_fp(fp, nullptr);
            fclose(fp);

            if (!p12) {
                std::cerr << "Error reading keystore file\n";
                ERR_print_errors_fp(stderr);
                exit (1);
            }

            if (!PKCS12_parse(p12, _keystore_password.c_str(), &pkey, &cert, &ca)) {
                fprintf(stderr, "Error parsing keystore file\n");
                ERR_print_errors_fp(stderr);
                exit (1);
            }

            PKCS12_free(p12);

            if (pkey) {
                PEM_write_PrivateKey(pkey_file, pkey, NULL, NULL, 0, NULL, NULL);

                fclose(pkey_file);
            }

            if (cert) {
                PEM_write_X509(cert_file, cert);
                fclose(cert_file);
            }

            if (ca && sk_X509_num(ca)) {
                for (unsigned i = 0; i < sk_X509_num(ca); i++) {
                    PEM_write_X509(ca_file, sk_X509_value(ca, i));
                }
                fclose(ca_file);
            }

            sk_X509_pop_free(ca, X509_free);

            X509_free(cert);

            EVP_PKEY_free(pkey);
        }

        std::string _randomFilename()
        {

            std::string result = "tmp";

            for (unsigned i = 0; i < 10; ++i) {
                result += static_cast<char>((std::rand() % ('z' - 'a')) + 'a');
            }

            return result;
        }

        std::string _buildExportUrl(const std::string& method, std::map<std::string, std::any>& options)
        {
            if (options.count("template") > 0 && std::any_cast<TemplateType>(options["template"]) == TemplateType::RAW) {
                options.erase("template");
                return _buildRequestUrl(method, "requestData", options);
            }
            
            return _buildRequestUrl(method, "requestExport", options);
        }

        std::string _buildGenerateUrl(const std::string& method, std::map<std::string, std::any>& options)
        {
            if (options.count("template") > 0) {
                std::cerr << "Unexpected option: template\n";
                options.erase("template");
            }

            return _buildRequestUrl(method, "requestData", options);
        }

        std::string _buildRequestUrl(const std::string& method, const std::string& requestType, std::map<std::string, std::any>& opt)
        {
            std::string url;
            url += "https://" + _genserver + "/testCaseService";
            url += "?requestType=" + requestType;
            url += "&client=cpp";

            url += "&request={\"method\":\"" + method + "\"";
            if (opt.count("model") == 0) {
                url += ",\"model\":\"" + model + "\"";
            }
            else {
                url += ",\"model\":\"" + std::any_cast<std::string>(opt["model"]) + "\"";
                opt.erase("model");
            }

            if (opt.count("template") > 0) {
                url += ",\"template\":\"" + template_type_url_param(std::any_cast<TemplateType>(opt["template"])) + "\"";
                opt.erase("template");
            }
            else if (requestType == "requestExport") {
                url += ",\"template\":\"CSV\"";
            }

            if (opt.size() != 0) {
                url += ",\"userData\":\"{";
                std::string padding = "";
                for (const std::pair<std::string, std::any>& option : opt) {
                    url += padding + options::serialize(option);
                    padding = ",";
                }
                url += "}\"";
            }

            url += "}";

            try {
                url = std::regex_replace(url, std::regex("\\\""), "%22");
                url = std::regex_replace(url, std::regex("'"),  "%27");
                url = std::regex_replace(url, std::regex("\\{"),  "%7B");
                url = std::regex_replace(url, std::regex("\\}"),  "%7D");
                url = std::regex_replace(url, std::regex("\\["),  "%5B");
                url = std::regex_replace(url, std::regex("\\]"),  "%5D");
            }
            catch (const std::regex_error& e) {
                std::cerr << e.what() << std::endl;
            }

            VERBOSE(std::cout << "url:" << url << std::endl);

            return url;
        }

        bool _parseMethodInfo(const std::string& line, MethodInfo& method_info) 
        {
            auto begin = line.find_first_of("(");
            auto end = line.find_last_of(")");
            auto args = line.substr(begin + 1, end - begin - 1);

            std::stringstream ss(args);
            std::string token;
            while (std::getline(ss, token, ',')) {
                token = token.substr(token.find_first_not_of(" "));
                std::string arg_type = token.substr(0, token.find(" "));
                std::string arg_name = token.substr(token.find(" ") + 1);
                method_info.arg_names.push_back(arg_name);
                method_info.arg_types.push_back(arg_type);
            }

            return true;
        }

        TestArguments _parseTestCase(picojson::value test, MethodInfo& method_info) 
        {
            TestArguments result;

            if (test.is<picojson::array>()) {
                auto test_array = test.get<picojson::array>();
                unsigned arg_index = 0;
                for (auto element : test_array) {
                    
                    try {                    
                        std::string value = element.get<picojson::object>()["value"].to_str();
                        result.add(method_info.arg_names[arg_index], method_info.arg_types[arg_index], value);
                    } 
                    catch(const std::exception& e) {
                    std::cerr << "Exception caught: " << e.what() << ". Too many parameters in the test: " << test.to_str() << std::endl;
                    }

                    arg_index++;
                }
            }

        if (! test.is<picojson::array>()) {
            std::cerr << "Error: test case should be a JSON array" << std::endl;
            return result;
        }

            return result;
        }

        std::tuple<std::string, picojson::value> _parseTestLine(std::string line) 
        {
            picojson::value v;
            std::string err = picojson::parse(v, line);
            picojson::value nothing;

            if (false == err.empty()) {
                std::cerr << "Cannot parse test line '" << line << "': " << err << std::endl;
                return std::tie("", nothing);
            }

            if (! v.is<picojson::object>()) {
                std::cerr << "Error: received line is not a JSON object" << std::endl;
                return std::tie("", nothing);
            }

            const picojson::value::object& obj = v.get<picojson::object>();
            
            if (obj.size() > 0) {
                auto value = *obj.begin();
                return std::tuple<std::string, picojson::value>(value.first, value.second);
            }

            return std::tie("", nothing);
        }
    };
}
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
#include<any>
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
}//namespace options

static size_t curl_data_callback(void *data, size_t size, size_t nmemb, void *userp){
    auto callback = static_cast<std::function<size_t(void *data, size_t size, size_t nmemb)>*>(userp);
    return callback->operator()(data, size, nmemb);
}

class TestProvider;

template<typename T>
class TestQueue{
    class const_iterator{
        const bool END_ITERATOR;
        TestQueue<T>& _queue;
        
    public:
        const_iterator(TestQueue<T>& queue, bool end = false) :
            _queue(queue), END_ITERATOR(end)
        {}

        const_iterator(const const_iterator& other) :
            _queue(other._queue), END_ITERATOR(other.END_ITERATOR)
        {}

        bool operator==(const const_iterator& other) const{
            if(END_ITERATOR && other.END_ITERATOR){
                return true;
            }

            if(END_ITERATOR || other.END_ITERATOR){
                return _queue.done();
            }
            return false;
        }

        bool operator!=(const const_iterator& other) const{
            return (*this == other) == false;
        }

        const_iterator& operator++(){
            _queue.next();
            return *this;
        }

        const T& operator*(){
            return _queue.current_element();
        }
    };
    // std::cout << "line: " << __LINE__ << std::endl;

    bool _done;
    std::vector<T> _data;
    const_iterator _begin;
    const_iterator _end;
    std::mutex _mutex;
    std::mutex _cv_mutex;
    std::condition_variable _cv;

public:
    TestQueue() :
        _done(false),
        _begin(*this),
        _end(*this, true)
    {}

    const_iterator begin() const {return _begin;}
    const_iterator end() const {return _end;}

    friend class const_iterator;
    friend class TestProvider;

    // private:
    bool done() {
        std::unique_lock<std::mutex> cv_lock(_mutex);

        if(_data.size() != 0){
            return false;
        }
        else if(_done){
            return true;
        }

        _cv.wait(cv_lock);
        return _done;
    }

    void next() {
        std::lock_guard<std::mutex> lock(_mutex);
        _data.erase(_data.begin());
    }

    void insert(const T& element){
        std::lock_guard<std::mutex> lock(_mutex);

        _data.push_back(element);
        _cv.notify_one();
    }

    T& current_element() {
        std::unique_lock<std::mutex> cv_lock(_mutex);
        if(_data.size() > 0){
            return _data[0];
        }
        _cv.wait(cv_lock);
        return _data[0];
    }
    void finish(){
        std::lock_guard<std::mutex> lock(_mutex);
        _done = true;
        _cv.notify_one();
    }

    bool empty(){
        std::lock_guard<std::mutex> lock(_mutex);
        return _data.size() == 0;
    }

};

class TestProvider{

    struct MethodInfo{
        std::vector<std::string> arg_names;
        std::vector<std::string> arg_types;
    };

    const std::string _genserver;
    const std::string _keystore_path;
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
                 const std::filesystem::path& keystore_path,
                 const std::string& genserver = "gen.ecfeed.com",
                 const std::string& keystore_password = "changeit") :
        model(model), _genserver(genserver),
        _keystore_password(keystore_password),
        _keystore_path(keystore_path),

        _temp_dir(std::filesystem::temp_directory_path()),
        _pkey_path(_temp_dir / _randomFilename()),
        _cert_path(_temp_dir / _randomFilename()),
        _ca_path(_temp_dir   / _randomFilename())
    {
        OpenSSL_add_all_algorithms();
        ERR_load_CRYPTO_strings();

        if(_curl_initialized == false)
        {
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

    std::vector<std::string> getArgumentNames(const std::string& method,
                                              const std::string& model = "")
    {
        std::lock_guard<std::mutex> lock(_mutex);

        exportRandom(method, {{"template", TemplateType::RAW},
                              {"length", 0},
                              {"adaptive", false},
                              {"duplicates", true}});

        return std::vector<std::string>();
    }

    std::vector<std::string> getArgumentTypes(const std::string& method,
                                              const std::string& model = "")
    {
        std::lock_guard<std::mutex> lock(_mutex);

        return std::vector<std::string>();
    }

    std::shared_ptr<TestQueue<std::string>> exportNwise(const std::string& method,
                                                        std::map<std::string, std::any> options = {})
    {
        std::map<std::string, std::any> gen_properties;

        gen_properties["n"] = options.count("n") ? options["n"] : 2; options.erase("n");
        gen_properties["coverage"] = options.count("coverage") ? options["coverage"] : 100; options.erase("coverage");

        std::map<std::string, std::any> opt;
        if(options.count("constraints")) {
            opt["constraints"] =  options["constraints"];
            options.erase("constraints");
        }
        if(options.count("choices")) {
            opt["choices"] =  options["choices"];
            options.erase("choices");
        }
        opt["dataSource"] = DataSource::NWISE;
        opt["properties"] = gen_properties;
        opt["template"] = options.count("template") ? options["template"] : TemplateType::CSV;  options.erase("template");

        if(options.size())
        {
            std::cerr << "Unknown options: ";
            for(const auto& option : options){
                std::cerr << " " << option.first << " " ;
            }
        }

        return _export(method, opt);
    }

    std::shared_ptr<TestQueue<std::vector<std::any>>> generateNwise(const std::string& method,
                                                                    std::map<std::string, std::any> options = {})
    {
        std::map<std::string, std::any> gen_properties;

        gen_properties["n"] = options.count("n") ? options["n"] : 2; options.erase("n");
        gen_properties["coverage"] = options.count("coverage") ? options["coverage"] : 100; options.erase("coverage");

        std::map<std::string, std::any> opt;
        if(options.count("constraints")) {
            opt["constraints"] =  options["constraints"];
            options.erase("constraints");
        }
        if(options.count("choices")) {
            opt["choices"] =  options["choices"];
            options.erase("choices");
        }
        opt["dataSource"] = DataSource::NWISE;
        opt["properties"] = gen_properties;

        if(options.size())
        {
            std::cerr << "Unknown options: ";
            for(const auto& option : options){
                std::cerr << " " << option.first << " " ;
            }
        }

        return _generate(method, opt);
    }

    std::shared_ptr<TestQueue<std::string>> exportPairwise(const std::string& method,
                                                           std::map<std::string, std::any> options = {})
    {
        std::lock_guard<std::mutex> lock(_mutex);

        std::map<std::string, std::any> gen_properties;

        gen_properties["coverage"] = options.count("coverage") ? options["coverage"] : 100; options.erase("coverage");
       
        std::map<std::string, std::any> opt;
        opt["constraints"] = options.count("constraints") ? options["constraints"] : std::set<std::string>({}); options.erase("constraints");
        opt["choices"] = options.count("choices") ? options["choices"] : std::map<std::string, std::any>({}); options.erase("choices");
        opt["dataSource"] = DataSource::NWISE;
        opt["properties"] = gen_properties;
        opt["template"] = options.count("template") ? options["template"] : TemplateType::CSV;  options.erase("template");

        if(options.size())
        {
            std::cerr << "Unknown options: ";
            for(const auto& option : options){
                std::cerr << " " << option.first << " " ;
            }
        }

        return _export(method, opt);
    }

    std::shared_ptr<TestQueue<std::string>> exportRandom(const std::string& method,
                                                         std::map<std::string, std::any> options = {})
    {
        std::lock_guard<std::mutex> lock(_mutex);

        std::map<std::string, std::any> gen_properties;

        gen_properties["length"] = options.count("length") ? options["length"] : 100; options.erase("length");
        gen_properties["duplicates"] = options.count("duplicates") ? options["duplicates"] : std::set<std::string>({}); options.erase("duplicates");
        gen_properties["adaptive"] = options.count("adaptive") ? options["adaptive"] : std::map<std::string, std::set<std::string>>({}); options.erase("adaptive");

        if(options.size())
        {
            std::cerr << "Unknown options: ";
            for(const auto& option : options){
                std::cerr << " " << option.first << " " ;
            }
        }

        std::map<std::string, std::any> opt;
        opt["properties"] = gen_properties;
        opt["dataSource"] = DataSource::RANDOM;
        if(options.count("constraints")) {
            opt["constraints"] =  options["constraints"];
            options.erase("constraints");
        }
        if(options.count("choices")) {
            opt["choices"] =  options["choices"];
            options.erase("choices");
        }
        opt["template"] = options.count("template") ? options["template"] : TemplateType::CSV;  options.erase("template");

        return _export(method, opt);
    }

    std::shared_ptr<TestQueue<std::string>> exportCartesian(const std::string& method,
                                                            std::map<std::string, std::any> options = {})
    {
        std::lock_guard<std::mutex> lock(_mutex);

        std::map<std::string, std::any> gen_properties;

       
        if(options.size())
        {
            std::cerr << "Unknown options: ";
            for(const auto& option : options){
                std::cerr << " " << option.first << " " ;
            }
        }

        std::map<std::string, std::any> opt;
        opt["constraints"] = options.count("constraints") ? options["constraints"] : std::set<std::string>({}); options.erase("constraints");
        opt["choices"] = options.count("choices") ? options["choices"] : std::map<std::string, std::set<std::string>>({}); options.erase("choices");
        opt["dataSource"] = DataSource::CARTESIAN;
        opt["properties"] = gen_properties;
        opt["template"] = options.count("template") ? options["template"] : TemplateType::CSV;  options.erase("template");

        return _export(method, opt);
    }

    std::shared_ptr<TestQueue<std::string>> exportStatic(const std::string& method,
                                                         std::set<std::string> test_suites = {})
    {
        std::lock_guard<std::mutex> lock(_mutex);

        std::map<std::string, std::any> gen_properties;

        std::map<std::string, std::any> opt = {};
        opt["testSuites"] = test_suites;
        opt["dataSource"] = DataSource::STATIC_DATA;
        opt["properties"] = gen_properties;

        return _export(method, opt);
    }


private:
    void _performRequest(const std::string& url,
                         const std::function<size_t(void *data, size_t size, size_t nmemb)>* data_callback){
        // std::cout << "Request URL: " << url << std::endl << std::endl;

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

        if(success != 0){
            std::cout << error_buf << std::endl;
        }

    }

    std::shared_ptr<TestQueue<std::string>> _export(const std::string& method, std::map<std::string, std::any>& options)
    {
        auto url = _buildExportUrl(method, options);
        std::shared_ptr<TestQueue<std::string>> result(new TestQueue<std::string>());

        std::function<size_t(void *data, size_t size, size_t nmemb)> data_cb = [result](void *data, size_t size, size_t nmemb) -> size_t{
            if(nmemb > 0)
            {
                std::string test((char*) data, (char*) data + nmemb - 1); //last byte seem to be a new line character
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

    std::shared_ptr<TestQueue<std::vector<std::any>>> _generate(const std::string& method, std::map<std::string, std::any>& options){
        auto url = _buildGenerateUrl(method, options);

        std::vector<std::string> types;
        std::shared_ptr<TestQueue<std::vector<std::any>>> result(new TestQueue<std::vector<std::any>>());
        std::shared_ptr<MethodInfo> method_info(new MethodInfo);
        std::shared_ptr<bool> method_info_ready(new bool(false));
 //       std::cout << "Method info size: " << method_info->arg_types.size() << std::endl;

        std::function<size_t(void *data, size_t size, size_t nmemb)> data_cb = [this, result, method_info, method_info_ready](void *data, size_t size, size_t nmemb) -> size_t{
            if(nmemb > 0)
            {
                std::string test((char*) data, (char*) data + nmemb - 1); //last byte seem to be a new line character
      //          std::cout << "Received line: " << test << std::endl;

                auto [name, value] = _parseTestLine(test);
                if(name == "info" && *method_info_ready == false){
                    std::cout << "Parsing test info line: " << value << std::endl;
                    std::string method_signature = value.to_str();
                    std::replace(method_signature.begin(), method_signature.end(), '\'', '\"');
                    auto [name_1, value_1] = _parseTestLine(method_signature);
                    if(name_1 == "method"){
                        std::cout << "Parsing method info: " << value_1 << std::endl;
                        std::cout << "Method info size: " << method_info->arg_types.size() << std::endl;
                        if(_parseMethodInfo(value_1.to_str(), method_info)){
                            *method_info_ready = true;
                        }
                    }
                }else if(name == "testCase" && *method_info_ready){
                    result->insert(_parseTestCase(value, method_info));
                }

                //result->insert(std::vector<std::any>({5, 1}));
                // std::vector<std::any> test((char*) data, (char*) data + nmemb - 1); //last byte seem to be a new line character
                // result->insert(test);
            }
            return nmemb;
        };

        _running_requests.push_back(std::async(std::launch::async, [result, url, data_cb, this](){
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

        if((fp = fopen(_keystore_path.c_str(), "rb")) == nullptr)
        {
            std::cerr << "Can't open the keystore file: " << _keystore_path << "\n";
            exit(1);
        }
        p12 = d2i_PKCS12_fp(fp, nullptr);
        fclose(fp);

        if (!p12)
        {
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
            for (unsigned i = 0; i < sk_X509_num(ca); i++)
                PEM_write_X509(ca_file, sk_X509_value(ca, i));
            fclose(ca_file);
        }
        sk_X509_pop_free(ca, X509_free);

        X509_free(cert);

        EVP_PKEY_free(pkey);


    }

    std::string _randomFilename()
    {

        std::string result = "tmp";
        for(unsigned i = 0; i < 10; ++i)
        {
            result += static_cast<char>((std::rand() % ('z' - 'a')) + 'a');
        }
        return result;
    }

    std::string _buildExportUrl(const std::string& method, std::map<std::string, std::any>& options)
    {
        if(options.count("template") > 0 && std::any_cast<TemplateType>(options["template"]) == TemplateType::RAW) {
            options.erase("template");
            return _buildRequestUrl(method, "requestData", options);
        }
        
        return _buildRequestUrl(method, "requestExport", options);
    }

    std::string _buildGenerateUrl(const std::string& method, std::map<std::string, std::any>& options)
    {
        if(options.count("template") > 0){
            std::cerr << "Unexpected option: template\n";
            options.erase("template");
        }
        //    options["requestType"] = std::string("requestData");
        return _buildRequestUrl(method, "requestData", options);
    }

    std::string _buildRequestUrl(const std::string& method, const std::string& requestType, std::map<std::string, std::any>& opt)
    {
        std::string url;
        url += "https://" + _genserver + "/testCaseService";
        url += "?requestType=" + requestType;
        url += "&client=python";

        url += "&request={\"method\":\"" + method + "\"";
        if(opt.count("model") == 0)
        {
            url += ",\"model\":\"" + model + "\"";
        }
        else
        {
            url += ",\"model\":\"" + std::any_cast<std::string>(opt["model"]) + "\"";
            opt.erase("model");
        }

        if(opt.count("template") > 0){
            url += ",\"template\":\"" + template_type_url_param(std::any_cast<TemplateType>(opt["template"])) + "\"";
            opt.erase("template");
        }
        else if(requestType == "requestExport"){
            url += ",\"template\":\"CSV\"";
        }

        if(opt.size() != 0){
            url += ",\"userData\":\"{";
            std::string padding = "";
            for(const std::pair<std::string, std::any>& option : opt){
                std::cout << "Serializing: " << option.first << std::endl;
                url += padding + options::serialize(option);
                padding = ",";
            }
            url += "}\"";
        }


        // url += "%7D";
        url += "}";

        try{
            url = std::regex_replace(url, std::regex("\\\""), "%22");
            url = std::regex_replace(url, std::regex("'"),  "%27");
            url = std::regex_replace(url, std::regex("\\{"),  "%7B");
            url = std::regex_replace(url, std::regex("\\}"),  "%7D");
            url = std::regex_replace(url, std::regex("\\["),  "%5B");
            url = std::regex_replace(url, std::regex("\\]"),  "%5D");
        }catch (const std::regex_error& e){
            std::cerr << e.what() << std::endl;
        }

        std::cout << "url:" << url << std::endl;

        return url;
    }

    bool _parseMethodInfo(const std::string& line, std::shared_ptr<MethodInfo> method_info){

        auto begin = line.find_first_of("(");
        auto end = line.find_last_of(")");
        auto args = line.substr(begin + 1, end - begin - 1);
//        std::cout << "Parsing: " << args << std::endl;
//        std::cout << "Method info size: " << method_info->arg_types.size() << std::endl;
        std::stringstream ss(args);
        std::string token;
        while(std::getline(ss, token, ',')){
            token = token.substr(token.find_first_not_of(" "));
            std::string arg_type = token.substr(0, token.find(" "));
            std::string arg_name = token.substr(token.find(" ") + 1);
            method_info->arg_names.push_back(arg_name);
            method_info->arg_types.push_back(arg_type);
            std::cout << "token: '" << token << "', arg type: '" << arg_type << "', arg name: '" << arg_name << "'" << std::endl;
        }

        return true;
    }

    std::vector<std::any> _parseTestCase(picojson::value test, std::shared_ptr<MethodInfo> method_info){
 //       std::cout << "Parsing test case: " << test.to_str() << std::endl;
        std::vector<std::any> result;
        if(test.is<picojson::array>())
        {
            auto test_array = test.get<picojson::array>();
            unsigned arg_index = 0;
            for(auto element : test_array){
//                auto test_element = element.get<picojson::object>();
                std::string parameter = element.get<picojson::object>()["value"].to_str();
 //               std::cout << "element: " << parameter << std::endl;

                try{
                    result.push_back(_castTestParameter(parameter, method_info->arg_types[arg_index]));
                }
                catch(const std::exception& e){
                    std::cerr << "Exception caught: " << e.what() << ". Too many parameters in the test: " << test << std::endl;
                }
                arg_index++;
            }
        }

//        if (! test.is<picojson::array>()) {
//            std::cerr << "Error: test case should be a JSON array" << std::endl;
//            return std::vector<std::any>();
//        }
//        const picojson::value::object& obj = test.get<picojson::array>();
//        if(obj.size() > 0)
//        {
//            auto value = obj[""];
////            return std::tuple<std::string, std::string>(value.first, value.second.to_str());
//        }


        return result;
    }

    std::any _castTestParameter(std::string value, std::string type){
        if(type == "String"){
            return std::any(value);
        }
        else if(value.length() == 0){
            std::cerr << "Parameter value cannot be empty string for types other than String\n";
            return std::any();
        }
        else if(type == "char"){
            return std::any(value.at(0));
        }
        else if(type == "byte"){
            return std::any(static_cast<char>(std::stoi(value)));
        }
        else if(type == "short"){
            return std::any(static_cast<short>(std::stoi(value)));
        }
        else if(type == "int"){
            return std::any(std::stoi(value));
        }
        else if(type == "long"){
            return std::any(std::stoll(value));
        }
        else if(type == "float"){
            return std::any(std::stof(value));
        }
        else if(type == "double"){
            return std::any(std::stold(value));
        }
        else {
            std::cerr << "Unknown parameter type: " << type << ". Converting the parameter to String\n";
            return std::any(value);
        }
    }

    std::tuple<std::string, picojson::value> _parseTestLine(std::string line){
        picojson::value v;
        std::string err = picojson::parse(v, line);
        picojson::value nothing;

        if(false == err.empty()){
            std::cerr << "Cannot parse test line '" << line << "': " << err << std::endl;
        }
        if (! v.is<picojson::object>()) {
            std::cerr << "Error: received line is not a JSON object" << std::endl;
            return std::tie("", nothing);
        }
        const picojson::value::object& obj = v.get<picojson::object>();
        if(obj.size() > 0)
        {
            auto value = *obj.begin();
            return std::tuple<std::string, picojson::value>(value.first, value.second);
        }
        return std::tie("", nothing);
    }
    //Deprecated:
public:
    std::shared_ptr<TestQueue<std::string>> exportNwise(const std::string& method,
                                                        TemplateType templ = TemplateType::CSV,
                                                        const unsigned n = 2,
                                                        const unsigned coverage = 100,
                                                        std::set<std::string> constraints = {},
                                                        std::map<std::string, std::set<std::string>> choices = {})
    {
        std::lock_guard<std::mutex> lock(_mutex);

        std::map<std::string, std::any> gen_properties;
        gen_properties["n"] = n;
        gen_properties["coverage"] = coverage;

        std::map<std::string, std::any> opt = {};
        if(constraints.size() > 0) opt["constraints"] = constraints;
        if(choices.size() > 0) opt["choices"] = choices;
        opt["dataSource"] = DataSource::NWISE;
        opt["properties"] = gen_properties;
        opt["template"] = templ;

        return _export(method, opt);
    }

};


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


    // std::map<std::string, std::any> optionsNWise = {{"coverage", coverage}, {"n", n}, {"constraints", constraints}, {"choices", choices}};
    // for(auto test : *tp.generateNwise("QuickStart.test", optionsNWise)) { 
    //     for(auto element : test){
    //        std::cout << element.type() << ":" << std::any_cast<int>(element) << ", ";
    //     }
    //     std::cout << std::endl;
    //  }

    // return 0;0
// }

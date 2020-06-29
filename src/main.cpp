#include<curl/curl.h>
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<vector>
#include<list>
#include<map>
#include<set>
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

void init_serializers()
{
    _serializers[std::type_index(typeid(unsigned))] = [](const std::any& v){
      return "'" + std::to_string(std::any_cast<unsigned>(v)) + "'";
    };
    _serializers[std::type_index(typeid(int))] = [](const std::any& v){
      return "'" + std::to_string(std::any_cast<int>(v)) + "'";
    };
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
    _serializers[std::type_index(typeid(std::list<std::any>))] = [](const std::any& v){
      std::string result = "[";
      for(auto& e : std::any_cast<std::list<std::any>>(v)){
        result += serialize(e);
      }
      result.erase(result.find_last_of(","));
      return result + "]";
    };
    _serializers[std::type_index(typeid(std::map<std::string, std::any>))] = [](const std::any& v){
      std::string result = "{";
      for(auto& e : std::any_cast<std::map<std::string, std::any>>(v)){
        result += "'" + e.first + "':" + serialize(e.second) + ",";
      }
      result.erase(result.find_last_of(","));
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


class TestProvider{
  const std::string _genserver;
  const std::string _keystore_path;
  const std::string _keystore_password;

  std::filesystem::path _temp_dir;
  std::filesystem::path _pkey_path;
  std::filesystem::path _cert_path;
  std::filesystem::path _ca_path;

  CURL* _curl_handle;


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
      // _pkey_path(_temp_dir / std::tmpnam(nullptr)),
      // _cert_path(_temp_dir / std::tmpnam(nullptr)),
      // _ca_path(_temp_dir / std::tmpnam(nullptr))
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
      return std::vector<std::string>();
    }

    std::vector<std::string> getArgumentTypes(const std::string& method, 
                                              const std::string& model = "")
    {
      return std::vector<std::string>();
    }

    void exportNwise(const std::string& method, 
                     const unsigned n = 2, 
                     const unsigned coverage = 100,
                     std::set<std::string> constraints = {},
                     std::map<std::string, std::set<std::string>> choices = {})
    {
      std::map<std::string, std::any> gen_properties;
      gen_properties["n"] = n;
      gen_properties["coverage"] = coverage;
      if(constraints.size() > 0) gen_properties["constraints"] = constraints;
      if(choices.size() > 0)     gen_properties["choices"] = choices;

      std::map<std::string, std::any> opt = {};
      opt["dataSource"] = DataSource::NWISE;
      opt["properties"] = gen_properties;

      _export(method, opt);
    }

    void exportPairwise(const std::string& method, 
                        const unsigned coverage = 100,
                        std::set<std::string> constraints = {},
                        std::map<std::string, std::set<std::string>> choices = {})
    {
      exportNwise(method, 2, coverage, constraints, choices);
    }

    void exportRandom(const std::string& method, 
                      const unsigned length = 1, 
                      const bool duplicates = false,
                      const bool adaptive = true,
                      std::set<std::string> constraints = {},
                      std::map<std::string, std::set<std::string>> choices = {})
    {
      std::map<std::string, std::any> gen_properties;
      gen_properties["length"] = length;
      gen_properties["duplicates"] = duplicates;
      gen_properties["adaptive"] = adaptive;
      if(constraints.size() > 0) gen_properties["constraints"] = constraints;
      if(choices.size() > 0)     gen_properties["choices"] = choices;

      std::map<std::string, std::any> opt = {};
      opt["dataSource"] = DataSource::RANDOM;
      opt["properties"] = gen_properties;

      _export(method, opt);
    }

    void exportCartesian(const std::string& method,
                         std::set<std::string> constraints = {},
                         std::map<std::string, std::set<std::string>> choices = {})
    {
      std::map<std::string, std::any> gen_properties;
      if(constraints.size() > 0) gen_properties["constraints"] = constraints;
      if(choices.size() > 0)     gen_properties["choices"] = choices;

      std::map<std::string, std::any> opt = {};
      opt["dataSource"] = DataSource::CARTESIAN;
      opt["properties"] = gen_properties;

      _export(method, opt);
    }

    void exportStatic(const std::string& method, 
                      std::set<std::string> test_suites = {})
    {
      std::map<std::string, std::any> opt = {};
      opt["dataSource"] = DataSource::STATIC_DATA;

      _export(method, opt);
    }

private:
  void _performRequest(const std::string& url){
    std::cout << url << std::endl << std::endl;

    char error_buf[128];
    curl_easy_setopt(_curl_handle, CURLOPT_SSLCERT, _cert_path.string().c_str());
    curl_easy_setopt(_curl_handle, CURLOPT_SSLCERTTYPE, "pem");
    curl_easy_setopt(_curl_handle, CURLOPT_SSLKEY, _pkey_path.string().c_str());
    curl_easy_setopt(_curl_handle, CURLOPT_CAINFO, _ca_path.string().c_str());
    curl_easy_setopt(_curl_handle, CURLOPT_BUFFERSIZE, 8);

    curl_easy_setopt(_curl_handle, CURLOPT_ERRORBUFFER, error_buf);

    curl_easy_setopt(_curl_handle, CURLOPT_URL, url.c_str());
    auto success = curl_easy_perform(_curl_handle);

    if(success != 0){
        std::cout << error_buf << std::endl;
    }
    
  }

  void _export(const std::string& method, std::map<std::string, std::any>& options)
  {   
    auto url = _buildExportUrl(method, options);
    _performRequest(url);
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

  void _initCurl()
  {


    // curl_easy_setopt(handle, CURLOPT_KEYPASSWD, "changeit");
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
    return _buildRequestUrl(method, "requestExport", options);
  }

  std::string _buildGenerateUrl(const std::string& method, std::map<std::string, std::any>& options)
  {
    options["requestType"] = std::string("requestData");
    return _buildRequestUrl(method, "requestData", options);
  }

  std::string _buildRequestUrl(const std::string& method, const std::string& requestType, std::map<std::string, std::any>& opt)
  {
    
    // "https://gen.ecfeed.com/testCaseService?requestType=requestData&request=%7B%22method%22:%22void%20TestClass.method%22,%22model%22:%222037-6847-2110-8251-1296%22,%22userData%22:%22%7B%27dataSource%27:%27genNWise%27,%20%27constraints%27:%27NONE%27%7D%22,%22sessionId%22:0%7D"
    std::string url = "https://" + _genserver + "/testCaseService";
    url += "?requestType=" + requestType;
    url += "&client=python";

    // url += "&request=%7B%22method%22:%22" + method + "%22";
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

    if(opt.size() != 0){
      url += ",\"userData\":\"{";// + ::options::serialize(opt);
      for(const std::pair<std::string, std::any>& option : opt){
        url += options::serialize(option) + ","; 
      }
      url.erase(url.find_last_of(","));
      url += "}\"";
    }

    if(opt.count("template") > 0){
      url += ",\"template\":\"" + template_type_url_param(std::any_cast<TemplateType>(opt["template"])) + "\"";
    }
    else{
      url += ",\"template\":\"CSV\"";
    }
       
    // url += "%7D";
    url += "}";

    try{
      url = std::regex_replace(url, std::regex("\\\""), "%22");
      url = std::regex_replace(url, std::regex("'"),  "%27");
      url = std::regex_replace(url, std::regex("\\{"),  "%7B");
      url = std::regex_replace(url, std::regex("\\}"),  "%7D");
    }catch (const std::regex_error& e){
      std::cerr << e.what() << std::endl;
    }

    return url;
  }
};


}//namespace ecfeed

//  static size_t cb(void *data, size_t size, size_t nmemb, void *userp)
//  {
//    size_t realsize = size * nmemb;
//   //  struct memory *mem = (struct memory *)userp;
    
//     std::cout << (char*)data << std::endl; 

//   //  char *ptr = (char*)malloc(mem->size + realsize + 1);
//   //  if(ptr == NULL)
//   // {     
//   //   std::cout << "Failed to allocate " << mem->size << "+" << realsize << "+" << 1 << " bytes\n";
//   //    return 0;  /* out of memory! */
//   // } 
//   //  mem->response = ptr;
//   //  memcpy(&(mem->response[mem->size]), data, realsize);
//   //  mem->size += realsize;
//   //  mem->response[mem->size] = 0;
 
//    return realsize;
//  }
 

int main(int argc, char** argv){



  ecfeed::TestProvider tp("2037-6847-2110-8251-1296", "/home/patryk/ecfeed/security.p12");

    
  tp.exportRandom("TestClass.method", 5, true, false);
  tp.exportNwise("TestClass.method", 3);
  tp.exportPairwise("TestClass.method", 50);
  tp.exportCartesian("TestClass.method");




    // std::srand(std::time(nullptr));
    // struct memory chunk;

    // FILE* fp;
    // EVP_PKEY* pkey;
    // X509* cert;
    // STACK_OF(X509)* ca = nullptr;
    // PKCS12* p12;

    // auto dir = std::filesystem::temp_directory_path();
    // auto pkey_path = dir / random_filename();
    // auto cert_path = dir / random_filename();
    // auto ca_path = dir / random_filename();

    // std::FILE* pkey_file = fopen(pkey_path.string().c_str(), "w"); 
    // std::FILE* cert_file = fopen(cert_path.string().c_str(), "w"); 
    // std::FILE* ca_file = fopen(ca_path.string().c_str(), "w"); 

    // std::cout << "pkey file:" << pkey_path << std::endl;
    // std::cout << "cert file:" << cert_path << std::endl;
    // std::cout << "ca file:" << ca_path << std::endl;

    // OpenSSL_add_all_algorithms();
    // ERR_load_CRYPTO_strings();

    // if((fp = fopen("/home/.patryk/ecfeed/security.p12", "rb")) == nullptr)
    // {
    //   std::cerr << "Cant open the keystore file\n";
    //   exit(1);
    // }
    // p12 = d2i_PKCS12_fp(fp, nullptr);
    // fclose(fp);

    // if (!p12) 
    // {
    //   std::cerr << "Error reading PKCS#12 file\n";
    //   ERR_print_errors_fp(stderr);
    //   exit (1);
    // }

    // if (!PKCS12_parse(p12, "changeit", &pkey, &cert, &ca)) {
    //     fprintf(stderr, "Error parsing PKCS#12 file\n");
    //     ERR_print_errors_fp(stderr);
    //     exit (1);
    // }
    // PKCS12_free(p12);

    // if (!(fp = fopen("cycki", "w"))) {
    //     fprintf(stderr, "Error opening file %s\n", argv[1]);
    //     exit(1);
    // }
    // if (pkey) {
    //     // fprintf(fp, "***Private Key***\n");
    //     PEM_write_PrivateKey(pkey_file, pkey, NULL, NULL, 0, NULL, NULL);
    //     fclose(pkey_file);
    // }
    // if (cert) {
    //     // fprintf(fp, "***User Certificate***\n");
    //     PEM_write_X509(cert_file, cert);
    //     fclose(cert_file);
    // }
    // if (ca && sk_X509_num(ca)) {
    //     // fprintf(fp, "***Other Certificates***\n");
    //     for (unsigned i = 0; i < sk_X509_num(ca); i++) 
    //         PEM_write_X509(ca_file, sk_X509_value(ca, i));
    //     fclose(ca_file);
    // }

    // sk_X509_pop_free(ca, X509_free);
    // X509_free(cert);
    // EVP_PKEY_free(pkey);

    // fclose(fp);


    // curl_global_init(CURL_GLOBAL_ALL);
    // auto handle = curl_easy_init();
    // char error_buf[128];


    // // curl_easy_setopt(handle, CURLOPT_URL, "http://www.jcraft.com/jsch/");
    // // curl_easy_setopt(handle, CURLOPT_URL, "https://gen.ecfeed.com");
    // curl_easy_setopt(handle, CURLOPT_URL, "https://gen.ecfeed.com/testCaseService?requestType=requestData&request=%7B%22method%22:%22void%20TestClass.method%22,%22model%22:%222037-6847-2110-8251-1296%22,%22userData%22:%22%7B%27dataSource%27:%27genNWise%27,%20%27constraints%27:%27NONE%27%7D%22,%22sessionId%22:0%7D");
    // curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, cb);
    // curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
    // curl_easy_setopt(handle, CURLOPT_SSLCERT, cert_path.string().c_str());
    // curl_easy_setopt(handle, CURLOPT_SSLCERTTYPE, "pem");
    // curl_easy_setopt(handle, CURLOPT_SSLKEY, pkey_path.string().c_str());
    // // curl_easy_setopt(handle, CURLOPT_KEYPASSWD, "changeit");
    // curl_easy_setopt(handle, CURLOPT_CAINFO, ca_path.string().c_str());
    // curl_easy_setopt(handle, CURLOPT_BUFFERSIZE, 8);

    // curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, error_buf);
    // auto success = curl_easy_perform(handle);

    // std::cout << "Success: " << success << std::endl;

    // if(success != 0){
    //     std::cout << error_buf << std::endl;
    // }
    // curl_easy_cleanup(handle);

    // std::filesystem::remove(pkey_path);
    // std::filesystem::remove(cert_path);
    // std::filesystem::remove(ca_path);


    return 0;
}

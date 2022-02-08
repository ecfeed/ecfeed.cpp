# pragma once

#include "ecfeed.hpp"
#include <memory>
#include <string>

namespace config
{
    class Default 
    {
        public:

            static std::string KEYSTORE_PATH_DEVELOP;
            static std::string KEYSTORE_PATH_PROD;
            static std::string KEYSTORE_PASSWORD_DEVELOP;
            static std::string KEYSTORE_PASSWORD_PROD;
            static std::string GEN_DEVELOP;
            static std::string GEN_PROD;
            static std::string MODEL_DEVELOP;
            static std::string MODEL_PROD;
            static std::string MODEL_DUMMY;
            static std::string F_QUICK;
            static std::string F_10x10;
            static std::string F_100x2;
            static std::string F_LOAN_2;

            static std::shared_ptr<ecfeed::test_provider> get_test_provider(bool prod = false)
            {
                return prod ? Default::get_test_provider_prod() : Default::get_test_provider_develop();
            }

        private:

            static std::shared_ptr<ecfeed::test_provider> get_test_provider_develop()
            {
                return std::make_shared<ecfeed::test_provider>(Default::MODEL_DEVELOP, Default::KEYSTORE_PATH_DEVELOP, Default::GEN_DEVELOP, Default::KEYSTORE_PASSWORD_DEVELOP);
            }

            static std::shared_ptr<ecfeed::test_provider> get_test_provider_prod()
            {
                return std::make_shared<ecfeed::test_provider>(Default::MODEL_PROD, Default::KEYSTORE_PATH_PROD, Default::GEN_PROD, Default::KEYSTORE_PASSWORD_PROD);
            }
    };

    std::string Default::KEYSTORE_PATH_DEVELOP = "/home/krzysztof/.ecfeed/security.p12";
    std::string Default::KEYSTORE_PATH_PROD = "/home/krzysztof/.ecfeed/security_prod.p12";
    std::string Default::KEYSTORE_PASSWORD_DEVELOP = "changeit";
    std::string Default::KEYSTORE_PASSWORD_PROD = "changeit";
    std::string Default::GEN_DEVELOP = "develop-gen.ecfeed.com";
    std::string Default::GEN_PROD = "gen.ecfeed.com";
    std::string Default::MODEL_DEVELOP = "QERK-K7BW-ME4G-W3TT-NT32";
    std::string Default::MODEL_PROD = "IMHL-K0DU-2U0I-J532-25J9";
    std::string Default::MODEL_DUMMY = "XXXX-XXXX-XXXX-XXXX-XXXX";
    std::string Default::F_QUICK = "QuickStart.test";
    std::string Default::F_10x10 = "com.example.test.Playground.size_10x10";
    std::string Default::F_100x2 = "com.example.test.Playground.size_100x2";
    std::string Default::F_LOAN_2 = "com.example.test.LoanDecisionTest2.generateCustomerData";
}
#include "nwise_generate.h"

std::vector<ecfeed::test_arguments> nwise_generate(std::string model, std::string method) {
    ecfeed::test_provider testProvider(model);

    return testProvider.generate_pairwise(method)->to_list();
}
#include "nwise_export.h"
#include "ecfeed.hpp"

std::vector<std::string> nwise_export(std::string model, std::string method) {
    ecfeed::test_provider testProvider(model);

    return testProvider.export_cartesian(method)->to_list();
}
#pragma once

#include <yaml-cpp/yaml.h>

template<typename T>
T yaml_read(const YAML::Node& node, const std::string& key) {
    auto n = node[key];
    if (!n) {
        std::cerr << "ERROR! Key " << key << " not found in node\n";
        exit(1);
    }
    return n.as<T>();
}

#pragma once

#include <yaml-cpp/yaml.h>
#include "glm/glm.hpp"

namespace YAML {
    // std::string
    template <>
    struct convert<glm::vec2> {
        static Node encode(const glm::vec2& rhs) { return Node(std::vector<float>(rhs.x, rhs.y)); }

        static bool decode(const Node& node, glm::vec2& rhs) {
            if (node.IsScalar())
                return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };
}

template<typename T>
T _ycast(const YAML::Node& node) {
    return node.as<T>();
}

template<typename T>
T yaml_read(const YAML::Node& node, const std::string& key) {
    auto n = node[key];
    if (!n) {
        std::cerr << "ERROR reading YAML! Key <" << key << "> not found in node\n";
        exit(1);
    }
    return _ycast<T>(n);
}

template<typename T>
T yaml_read(const YAML::Node& node, const std::string& key, const T defaultValue) {
    auto n = node[key];
    if (!n) {
        return defaultValue;
    }
    return _ycast<T>(n);
}



template<>
glm::vec4 _ycast(const YAML::Node& node) {
    auto vec = node.as<std::vector<float>>();
    assert(vec.size() == 4);
    return glm::vec4(vec[0], vec[1], vec[2], vec[3]);
}

template<>
glm::vec3 _ycast(const YAML::Node& node) {
    auto vec = node.as<std::vector<float>>();
    assert(vec.size() == 3);
    return glm::vec3(vec[0], vec[1], vec[2]);
}

template<>
glm::vec2 _ycast(const YAML::Node& node) {
    auto vec = node.as<std::vector<float>>();
    assert(vec.size() == 2);
    return glm::vec2(vec[0], vec[1]);
}



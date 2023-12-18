#include "animation.h"

using namespace monkey::skeletal;

SkeletalAnimation::SkeletalAnimation(const YAML::Node& node) {
    m_loop = node["loop"].as<bool>(true);
    //auto joints = node["joints"].as<std::vector<std::string>>();
    auto time_factor = node["time_factor"].as<float>(1.f);
    m_length = time_factor * node["length"].as<float>(0.f);
    int index = 0;

    for (const auto& keyFrame : node["key_frames"]) {
        auto t = time_factor * keyFrame["t"].as<float>();
        auto poseIn = keyFrame["pose"];
        std::unordered_map<std::string, JointTransform> pose;
        for (const auto& joint : poseIn) {
            auto jointId = joint.first.as<std::string>();
            auto data = joint.second.as<std::vector<float>>();
            JointTransform tr;
            tr.rotation = glm::angleAxis(glm::radians(data[0]), glm::vec3(0.f, 0.f, 1.f));
            if (data.size() > 1) {
                tr.translation = glm::vec3(data[1], data[2], 0.f);
            }
            if (data.size() > 3) {
                tr.scale = glm::vec3(data[3]);
            }
            pose[jointId] = tr;
        }
        m_keyFrames.push_back(std::make_shared<KeyFrame>(index, t, pose));
        index++;
    }
    if (!m_loop) {
        m_length = m_keyFrames.back()->getTimeStamp();
    }

//    // attack boxes
//    if (node["attacks"]) {
//        for (const auto& attack : node["attacks"]) {
//            auto startTime = time_factor * attack[0].as<float>();
//            auto endTime = time_factor * attack[1].as<float>();
//            // TODO m_attacks.emplace_back(startTime, endTime);
//        }
//    }
}




std::tuple<KeyFrame*, KeyFrame*, float> SkeletalAnimation::getPreviousAndNextKeyFrames(float t) {
    if (t < 0 || t > m_length) {
        return std::make_tuple(nullptr, nullptr, 0.0f);
    }
    for (int i = 1; i < m_keyFrames.size(); ++i) {
        if (m_keyFrames[i]->getTimeStamp() > t) {
            float progression = (t - m_keyFrames[i-1]->getTimeStamp()) / (m_keyFrames[i]->getTimeStamp() -m_keyFrames[i-1]->getTimeStamp());
            return std::make_tuple(m_keyFrames[i-1].get(), m_keyFrames[i].get(), progression);
        }
    }
    // if we get here, we are in a loop
    float progression = (t - m_keyFrames.back()->getTimeStamp()) / (m_length - m_keyFrames.back()->getTimeStamp());
    return std::make_tuple(m_keyFrames.back().get(), m_keyFrames.front().get(), progression);

}

//int SkeletalAnimation::getAttack(float u) const {
//    int id = 0;
//    for (const auto& t : m_attacks) {
//        if (u >= t.first && u <= t.second) {
//            return id;
//        }
//        if (u > t.second) {
//            return -1;
//        }
//        id++;
//    }
//    return -1;
//}



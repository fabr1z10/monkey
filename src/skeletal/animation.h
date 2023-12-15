#pragma once

#include "keyframe.h"
#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

namespace monkey::skeletal {

    class SkModel;

    class SkeletalAnimation {
    public:
        SkeletalAnimation(const YAML::Node &node);

        /**
         * @return The length of the animation in seconds.
         */
        float getLength() const;

        const std::vector<std::shared_ptr<KeyFrame>> &getKeyFrames();

        std::tuple<KeyFrame *, KeyFrame *, float> getPreviousAndNextKeyFrames(float t);

        // TODO const std::vector<std::pair<float, float>>& getAttacks() const;
        // TODO int getAttack(float t) const;
        // TODO bool hasAttacks() const;
        std::unordered_map<std::string, glm::mat4> getAnimTransform(float, SkModel *);

        bool loop() const;

    private:
        bool m_loop;
        float m_length;
        std::vector<std::shared_ptr<KeyFrame>> m_keyFrames;
        //std::vector<std::pair<float, float>> m_attacks;
    };

    inline float SkeletalAnimation::getLength() const {
        return m_length;
    }
    inline bool SkeletalAnimation::loop() const {
        return m_loop;
    }

    inline const std::vector<std::shared_ptr<KeyFrame>>& SkeletalAnimation::getKeyFrames() {
        return m_keyFrames;
    }
}
//inline bool SkeletalAnimation::hasAttacks() const {
//    return !m_attacks.empty();
//}



//inline const std::vector<std::pair<float, float>> & SkeletalAnimation::getAttacks() const {
//    return m_attacks;
//}
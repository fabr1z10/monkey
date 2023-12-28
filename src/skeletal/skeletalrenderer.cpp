#include "skeletalrenderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

using namespace monkey::skeletal;

SkeletalRenderer::SkeletalRenderer(const pybind11::kwargs& args) : BatchRenderer<ProvaBatch>(args), m_complete(false),
    _model(nullptr), m_currentAnimation(nullptr) {
    _castShadow = py_get_dict<bool>(args, "cast_shadow", false);
    _shadowAlpha = py_get_dict<float>(args, "shadow_alpha", 0.5f);
    _shadowScale = py_get_dict<float>(args, "shadow_scale", 1.0f);
//	_shaderType = SHADER_SKELETAL;
}

std::unordered_map<int, JointTransform> SkeletalRenderer::interpolatePoses(
        KeyFrame* previousFrame, KeyFrame* nextFrame, float progression) {
    std::unordered_map<int, JointTransform> currentPose;
    const auto& nf = nextFrame->getJointKeyFrames();
    for (const auto& p : previousFrame->getJointKeyFrames()) {
        // the model might not have this joint ... in this case nothing to do
        int jointId = _model->getJointId(p.first);
        if (jointId == -1) {
            continue;
        }

        // previousTransform is p.second
        JointTransform nextTransform = nf.at(p.first);
        //JointTransform currentTransform = m_model->getRestTransform(p.first);
        auto localTransform = JointTransform::interpolate(p.second, nextTransform, progression);
        //currentTransform += JointTransform::interpolate(p.second, nextTransform, progression);
        //currentTransform.z = 0;
        //std::cout << m_animationTime << " . " << currentTransform.alpha << "\n";
        currentPose.insert(std::make_pair(jointId, localTransform));
    }
    return currentPose;
}

void SkeletalRenderer::update(double dt) {
    std::unordered_map<int, JointTransform> pose;
    if (m_currentAnimation != nullptr) {
        m_animationTime += static_cast<float>(dt);
        if (m_animationTime >= m_currentAnimation->getLength()) {
            m_complete = true;
            if (m_currentAnimation->loop()) {
                m_animationTime = fmod(m_animationTime, m_currentAnimation->getLength());
            }  else {
                m_animationTime = m_currentAnimation->getLength() - 0.001f;
            }
        }
        // compute current pose
        auto frames = m_currentAnimation->getPreviousAndNextKeyFrames(m_animationTime);
        pose = interpolatePoses(std::get<0>(frames), std::get<1>(frames), std::get<2>(frames));
    }

    _bones = _model->calculateCurrentPose(pose);

    // apply offset
    const auto& offsetPoints = _model->getOffsetPoints();
    glm::vec3 offset(0.0f);
    if (!offsetPoints.empty()) {

        //std::cout << "no of offset points: " << offsetPoints.size() << "\n";
        for (const auto &a : offsetPoints) {
            // find coordinates of offset pointg
            glm::vec4 p = _bones[a.first] * glm::vec4(a.second, 1.0f);
            offset.y = std::max(-p.y, offset.y);
        }
        //std::cerr << offset.y << "\n";
        setTransform(glm::translate(offset));
    }


    // compute height box
    const auto* hp = _model->getCollidePoint(0);
    auto sz = _model->getSizeXZ();

    if (hp != nullptr) {
        glm::vec4 p = _bones[hp->first] * glm::vec4(hp->second, 1.0f);
        _size.y = offset.y + p.y;
        // std::cout << "HEAD IS AT " << _height << "\n";
    }
    _size.x = sz.x;
    _size.z = sz.y;



}

void SkeletalRenderer::innerDraw(Shader * s, int l2m ,int weightIndex, int pz) {
    int n{0};
    auto worldMatrix =m_node->getWorldMatrix() * m_rendererTransform;
    int jointMatrixLoc = glGetUniformLocation(s->getProgId(), "model");
    glUniformMatrix4fv(jointMatrixLoc, 1, GL_FALSE, glm::value_ptr(worldMatrix[0]));
    for (const auto& model : _model->getModels()) {
        auto restTransform = _model->getRestTransform(n);
        const auto& jinfo = _model->getJointInfo(n++);
        auto weightIndices = jinfo.weightIndex;// m_skeletalModel->getWeightIndex(n++);
        glUniformMatrix4fv(l2m, 1, false, &restTransform[0][0]);
        glUniform3iv(weightIndex, 1, &weightIndices[0]);
        glUniform1f(pz, jinfo.z);
        model->draw(s);
    }

}

void SkeletalRenderer::draw(Shader * s) {

    glm::vec4 white(1.f);
    auto mcol = glGetUniformLocation(s->getProgId(), "mult_color");
	auto boneLoc = glGetUniformLocation(s->getProgId(), "Bone");
	auto l2m = glGetUniformLocation(s->getProgId(), "local_to_model");
	auto weightIndex = glGetUniformLocation(s->getProgId(), "weightIndex");
	auto pz = glGetUniformLocation(s->getProgId(), "z");
	glUniformMatrix4fv(boneLoc, _bones.size(), false, &_bones[0][0][0]);
    glUniform4fv(mcol, 1, &white[0]);
	innerDraw(s, l2m, weightIndex, pz);

    if (_castShadow) {
        glm::vec4 shade(0.f, 0.f, 0.f, _shadowAlpha);
        auto tr = getRendererTransform();
        auto p = glm::rotate(glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(1.f, _shadowScale, 1.f)) * tr;
        setTransform(p);
        glUniform4fv(mcol, 1, &shade[0]);
        innerDraw(s, l2m, weightIndex, pz);
        //renderer.second->draw(s);
        setTransform(tr);
    }

}

void SkeletalRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
	Renderer::setModel(model, args);
	_model = dynamic_cast<SkeletalModel*>(model.get());
	assert(_model != nullptr);
	_batch->addRenderer(this);
}

SkeletalRenderer::~SkeletalRenderer() noexcept {
	_batch->removeRenderer(this);
}


void SkeletalRenderer::setAnimation(const std::string & id) {
    if (id == m_animation) {
        return;
    }

    m_complete = false;
    m_currentAnimation = _model->getAnimation(id);
    m_animation = id;
    m_animationTime = 0.0f;
}
#include <iostream>
#include "room.h"
#include "components/renderer.h"
#include "engine.h"
#include <glm/glm.hpp>

Room::Room(const std::string& id) : m_id(id) {
    m_root = std::make_shared<Node>();
}

Room::~Room() {
}

void Room::cleanUp() {
    m_root = nullptr;
    m_runners.clear();
}


std::shared_ptr<Node> Room::getRoot() {
    return m_root;
}

void Room::iterate_dfs(std::function<void(Node*)> f) {
    std::vector<Node*> li;
    li.push_back(m_root.get());
    while (!li.empty()) {
        auto current = li.back();
        li.pop_back();
        f(current);
        for (auto const &[k, v] : current->getChildren()) {
            li.push_back(v.get());
        }
    }
}

void Room::update(double dt) {
    //for (const auto& m : m_root->m_children) std::cout << "cane: " << m.second.use_count() << "\n";
    std::vector<Node*> li;
    li.push_back(m_root.get());
    while (!li.empty()) {
        auto current = li.back();
        li.pop_back();
        current->update(dt);
        // update world transform
        for (auto const & [k, v] : current->getChildren()) {
            li.push_back(v.get());
        }
    }
    for (const auto& r : m_runners) {
        r.second->update(dt);
    }
}

void Room::draw(Shader* s) {
    typedef void(Node::*BarkFunction)(void);

    std::vector<std::pair<int, glm::mat4>> m_matrices;
    std::vector<std::pair<int, std::shared_ptr<Camera>>> camStack;
    std::vector<Node*> li;
    li.push_back(m_root.get());
    glm::mat4 viewMatrix(1.0f);
    while (!li.empty()) {
        auto current = li.back();
        int currentIndex = li.size() - 1;
        li.pop_back();
        if (!current->active()) {
            continue;
        }

        // check if current node has a camera
        auto cam = current->getCamera();
        bool changeCam = false;
        while (!camStack.empty() && currentIndex < camStack.back().first) {
            // need to pop cam from stack
            changeCam = true;
            camStack.pop_back();
        }
        if (cam != nullptr) {
            changeCam = true;
            camStack.emplace_back(currentIndex, cam);
        }
        if (changeCam && !camStack.empty()) {
            camStack.back().second->init(s);
            viewMatrix = camStack.back().second->getViewMatrix();
            s->setMat4("view", viewMatrix);
        }


        while (!m_matrices.empty() && currentIndex < m_matrices.back().first) {
            // need to pop last matrix from stack
            m_matrices.pop_back();
        }

        // setup modelview
        auto renderer = current->getComponent<Renderer>();
        if (renderer != nullptr && renderer->getShaderType() == s->getShaderType()) {
            //s->preDraw(current);
            if (renderer->setup(s) == 0) {
                renderer->draw(s);

            }
        }
        for (const auto& [k, v] : current->getChildren()) {
            li.push_back(v.get());
        }

    }

}

void Room::setOnStart(pybind11::function f) {
    m_onStart = f;
}

void Room::setOnEnd(pybind11::function f) {
    m_onEnd = f;
}

void Room::start() {
    if (m_onStart) m_onStart();
}
void Room::end() {
    if (m_onEnd) m_onEnd();
}

//void Room::addLight(std::shared_ptr<Light> light) {
//    m_lights.push_back(light);
//}
//
//void Room::useLights(Shader * s) {
//    s->setFloat("ambient", m_ambientStrength);
//    for (const auto& light : m_lights) {
//        light->setup(s);
//    }
//}
//
//void Room::setAmbientStrength(float ambient) {
//    m_ambientStrength = ambient;
//}
#include <iostream>
#include "room.h"
#include "components/renderer.h"
#include "engine.h"
#include <glm/glm.hpp>
#include "batch/quadbatch.h"
#include "batch/linebatch.h"

Room::Room() : m_mainCamera(nullptr), m_clearColor(0.f, 0.f, 0.f, 255.f) {
    m_root = std::make_shared<Node>();
    _batches.emplace_back();
    _batches.emplace_back();
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

void Room::addRunner(std::shared_ptr<Runner> c) {
	m_runners[c->getType()] = c;
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

void Room::setMainCam(std::shared_ptr<Camera> cam) {
	m_mainCamera = cam.get();
}

void Room::addCamera(const std::string &id, std::shared_ptr<Camera> cam) {
    if (m_cameras.empty()) {
        m_mainCamera = cam.get();
    }
    m_cameras[id] = cam;
}

void Room::addSpriteBatch(const std::string &spriteSheet, const std::string &camName, int maxElements) {
    auto ptr = std::make_shared<QuadBatch>(maxElements, m_cameras.at(camName), spriteSheet);
    _batches[0].push_back(ptr);

}
void Room::addLinesBatch(const std::string &camName, int maxElements) {
    auto ptr = std::make_shared<LineBatch>(maxElements, m_cameras.at(camName));
    _batches[1].push_back(ptr);

}

IBatch * Room::getBatch(int shader, int id) {
    return _batches[shader][id].get();
}

Camera* Room::getCamera(const std::string &id) {
    return m_cameras.at(id).get();
}


void Room::update(double dt) {

    //auto* cam = Engine::instance().getBatch(0)->getCamera();
    Bounds currentBounds;
    if (m_mainCamera != nullptr) {
        currentBounds = m_mainCamera->getViewingBounds();
    }

    currentBounds.scale(2.f, 2.f);

    int m_nUpdates{0};
    //for (const auto& m : m_root->m_children) std::cout << "cane: " << m.second.use_count() << "\n";
    std::vector<Node*> li;
	li.push_back(m_root.get());
	//std::vector<std::pair<int, std::shared_ptr<Camera>>> camStack;

    while (!li.empty()) {
        auto current = li.back();
		//int currentIndex = li.size() - 1;
		li.pop_back();
		// check if current node has a camera
		//auto cam = current->getCamera();
		bool changeCam = false;
        auto b = current->getBounds();
        if (currentBounds.intersect2D(b)) {
			current->update(dt);
			for (auto const &[k, v] : current->getChildren()) {
				li.push_back(v.get());
			}
		}
    }
    for (const auto& r : m_runners) {
        r.second->update(dt);
    }
    //std::cout << "# updates: " << m_nUpdates << std::endl;
}

void Room::configure(Shader * s, int i ) {
    for (const auto& batch : _batches[i]) {
        batch->configure(s);
    }

}
void Room::draw(Shader* s, int i) {

    for (const auto& batch : _batches[i]) {
		batch->draw(s);
    }
//    typedef void(Node::*BarkFunction)(void);
//
//    std::vector<std::pair<int, glm::mat4>> m_matrices;
//    std::vector<std::pair<int, std::shared_ptr<Camera>>> camStack;
//    std::vector<Node*> li;
//    li.push_back(m_root.get());
//    glm::mat4 viewMatrix(1.0f);
//    while (!li.empty()) {
//        auto current = li.back();
//        int currentIndex = li.size() - 1;
//        li.pop_back();
//        if (!current->active()) {
//            continue;
//        }
//
//        // check if current node has a camera
//        auto cam = current->getCamera();
//        bool changeCam = false;
//        while (!camStack.empty() && currentIndex < camStack.back().first) {
//            // need to pop cam from stack
//            changeCam = true;
//            camStack.pop_back();
//        }
//        if (cam != nullptr) {
//            changeCam = true;
//            camStack.emplace_back(currentIndex, cam);
//        }
//        if (changeCam && !camStack.empty()) {
//            camStack.back().second->init(s);
//            viewMatrix = camStack.back().second->getViewMatrix();
//            s->setMat4("view", viewMatrix);
//        }
//
//
//        while (!m_matrices.empty() && currentIndex < m_matrices.back().first) {
//            // need to pop last matrix from stack
//            m_matrices.pop_back();
//        }
//
//        // setup modelview
//        auto renderer = current->getComponent<Renderer>();
//        if (renderer != nullptr && renderer->getShaderType() == s->getShaderType()) {
//            s->init(current);
//        	//s->preDraw(current);
//            if (renderer->setup(s) == 0) {
//                //renderer->draw(s);
//
//            }
//        }
//        for (const auto& [k, v] : current->getChildren()) {
//            li.push_back(v.get());
//        }
//
//    }
//
}

void Room::setOnStart(pybind11::function f) {
    m_onStart = f;
}

void Room::setOnEnd(pybind11::function f) {
    m_onEnd = f;
}

void Room::setClearColor(int r, int g, int b) {
    m_clearColor = glm::vec4(r, g, b, 255) / 255.f;

}

void Room::start() {
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

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
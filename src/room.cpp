#include <iostream>
#include "room.h"
#include "components/renderer.h"
#include "engine.h"
#include <glm/glm.hpp>
#include "batch/quadbatch.h"
#include "batch/linebatch.h"
#include "error.h"
#include <glm/gtc/type_ptr.hpp>
#include "assetmanager.h"
#include "spritesheet.h"



Room::Room() : m_mainCamera(nullptr), m_clearColor(0.f, 0.f, 0.f, 255.f) {
    m_root = std::make_shared<Node>();
    //Engine::instance().addNode(m_root);

	//addLinesBatch();

//    _batches.emplace_back();
//    _batches.emplace_back();
}

bool Room::hasBatch(const std::string& id) {
	return _batchMap.count(id) > 0;
}

void Room::addBatch(const std::string &batchId, std::shared_ptr<IBatch> batch) {
	_batches.push_back(batch);
	_batchMap[batchId] = batch.get();
}

IBatch * Room::getBatch(const std::string & id) {
	return _batchMap.at(id);
}

//void Room::addSpritesheet(const std::string &sheet) {
//	auto s = AssetManager::instance().getSpritesheet(sheet);
//	//_quadBatches[s->getId()] = s->getBatch();
//
//}

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
    std::list<Node*> li {m_root.get()};
    while (!li.empty()) {
        auto* current = li.front();
        li.pop_front();
        f(current);
        for (const auto& i : current->getChildren()) {
            li.push_front(i.get());
        }
    }
}

void Room::setMainCam(std::shared_ptr<Camera> cam) {
	m_mainCamera = cam.get();
}

void Room::addCamera(std::shared_ptr<Camera> cam) {
    m_cameras.push_back(cam);
    //_batches.emplace_back();
}

//QuadBatch* Room::addSpriteBatch(const std::string &spriteSheet, int maxElements) {
////	auto it = _quadBatches.find(spriteSheet);
////	if (it != _quadBatches.end()) {
////		return it->second.get();
////	}
////	auto ptr = std::make_shared<QuadBatch>(maxElements, spriteSheet);
////	_quadBatches[spriteSheet] = ptr;
////	return ptr.get();
//}

//void Room::addLinesBatch(int maxElements) {
//    _lineBatch = std::make_shared<LineBatch>(maxElements);
//    //_batches[1].push_back(ptr);
//
//}

//IBatch * Room::getBatch(int shader, int id) {
//    return _batches[shader][id].get();
//}


void Room::update(double dt) {

    //auto* cam = Engine::instance().getBatch(0)->getCamera();
    Bounds currentBounds;
//    if (m_mainCamera != nullptr) {
//        currentBounds = m_mainCamera->getViewingBounds();
//    }
//
//    currentBounds.scale(2.f, 2.f);

    int m_nUpdates{0};
    //for (const auto& m : m_root->m_children) std::cout << "cane: " << m.second.use_count() << "\n";
    std::list<Node*> li;
	li.push_back(m_root.get());
	//std::vector<std::pair<int, std::shared_ptr<Camera>>> camStack;
	//_currentCamera.clear();
	for (const auto& r : m_runners) {
		r.second->initialUpdate();
	}

    while (!li.empty()) {
        auto current = li.front();
		li.pop_front();

        //auto b = current->getBounds();
        //if (currentBounds.intersect2D(b)) {
		//if (true) {
		current->update(dt);
		for (auto const &c : current->getChildren()) {
			li.push_front(c.get());
		}
		//}
    }

    iterate_dfs([] (Node* n) { n->postProcess(); });

    for (const auto& r : m_runners) {
        r.second->update(dt);
    }

    // process input
    while(!_callbacks.empty()) {
    	_callbacks.front()();
    	_callbacks.pop_front();

    }


    //std::cout << "# updates: " << m_nUpdates << std::endl;
}

void Room::configure(Shader * s, int i ) {

	for (auto& batch : _batches) {
		batch->configure(s);
	}

//    for (const auto& batch : _quadBatches) {
//        batch.second->configure(s);
//    }
//    if (_lineBatch != nullptr) {
//    	_lineBatch->configure(s);
//    }

}
void Room::draw(Shader* s) {

//	for (size_t i = 0; i < _batches.size(); ++i) {
//		// setup camera
//		auto vp = m_cameras[i]->getViewport();
//		glViewport(vp.x, vp.y, vp.z, vp.w);
//		int jointMatrixLoc = glGetUniformLocation(s->getProgId(), "pv_mat");
//		auto pvMatrix = m_cameras[i]->getProjectionMatrix() * m_cameras[i]->getViewMatrix();
//		glUniformMatrix4fv(jointMatrixLoc, 1, GL_FALSE, glm::value_ptr(pvMatrix[0]));
//		for (auto& batch : _batches[i]) {
//			batch->draw(s);
//		}
//	}



//	auto vp = Engine::instance().getActualDeviceViewport();
//	glViewport(vp.x, vp.y, vp.z, vp.w);
//
//	std::vector<glm::mat4> pvMatrices;
//	for (const auto& camera : m_cameras) {
//		pvMatrices.push_back(camera->getProjectionMatrix() * camera->getViewMatrix());
//	}
//	int jointMatrixLoc = glGetUniformLocation(s->getProgId(), "pv_mat");
//	glUniformMatrix4fv(jointMatrixLoc, 2, GL_FALSE, glm::value_ptr(pvMatrices[0]));
//
//
//	for (const auto& batch : _quadBatches) {
//		batch.second->draw(s);
//	}
//
//
//	if (_lineBatch != nullptr) {
//		_lineBatch->draw(s);
//	}
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

void Room::addOnStart(pybind11::function f) {
    m_onStart.push_back(f);
}

void Room::setOnEnd(pybind11::function f) {
    m_onEnd = f;
}

void Room::setClearColor(int r, int g, int b) {
    m_clearColor = glm::vec4(r, g, b, 255) / 255.f;

}

void Room::start() {
	for (auto& b : _batches) {
		b->init();
	}

    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
	for (const auto& runner : m_runners) {
		runner.second->start();
	}
    iterate_dfs([] (Node* n) { n->start(); });

    if (!m_onStart.empty()) {
        for (const auto& f : m_onStart) {
            f();
        }
    }

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
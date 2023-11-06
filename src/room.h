#pragma once

#include <vector>
#include <memory>
#include "node.h"
#include "runner.h"
#include "batch/quadbatch.h"
#include "batch/linebatch.h"
//#include "light.h"

struct CurrentCamera {
	Camera* cam;
	glm::mat4 pvMatrix;
	Node* endNode;

	void clear() {
		cam = nullptr;
		endNode = nullptr;
	}
};

class Room {
public:
    Room();
    void addSpritesheet(const std::string& sheet);
    void setClearColor(int r, int g, int b);
    void addCamera( std::shared_ptr<Camera>);
    //QuadBatch* addSpriteBatch(const std::string& spriteSheet, int maxElements = 1000);
    //void addLinesBatch(int maxElements = 1000);
    void addBatch(const std::string& batchId, std::shared_ptr<IBatch>);
    IBatch* getBatch(const std::string&);
    int getBatchCount() const;
//	LineBatch* getLineBatch() {
//		return _lineBatch.get();
//	}
    //IBatch* getBatch(int shader, int id);
    Camera* getCamera(int id);
	const std::vector<std::shared_ptr<IBatch>>& getBatches();
    ~Room();
    void update(double);
    void configure(Shader*, int);
    void draw(Shader*);
    std::string id() const;
    std::shared_ptr<Node> getRoot();
    void iterate_dfs(std::function<void(Node*)> f);
    void addRunner(std::shared_ptr<Runner> c);
    void cleanUp();

    template <typename T>
    T* getRunner() {
        auto it = m_runners.find(std::type_index(typeid(T)));
        if (it != m_runners.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    void setOnStart(pybind11::function);
    void setOnEnd(pybind11::function);

    void start();
    void end();

    //void addLight(std::shared_ptr<Light>);
    //void setAmbientStrength(float);
    //void useLights(Shader*);
    void setMainCam(std::shared_ptr<Camera>);

    const CurrentCamera& getCurrentCamera();
    int getCameraCount() const;
    void addCallback(pybind11::function f) {_callbacks.push_back(f);}
private:
	CurrentCamera _currentCamera;
	Camera* m_mainCamera;
    std::string m_id;
    std::shared_ptr<Node> m_root;
    std::vector<std::shared_ptr<Camera>> m_cameras;
    std::unordered_map<std::type_index, std::shared_ptr<Runner> > m_runners;
    float m_ambientStrength;
    //std::vector<std::shared_ptr<Light>> m_lights;
    glm::vec4 m_clearColor;
    pybind11::function m_onStart;
    pybind11::function m_onEnd;

	std::list<pybind11::function> _callbacks;
    //std::shared_ptr<LineBatch> _lineBatch;

    //std::unordered_map<std::string, std::shared_ptr<QuadBatch>> _quadBatches;

    /// batches
	std::vector<std::shared_ptr<IBatch>> _batches;
	std::unordered_map<std::string, IBatch*> _batchMap;
};

inline const CurrentCamera & Room::getCurrentCamera() {
	return _currentCamera;
}

inline std::string Room::id() const {
    return m_id;
}

inline int Room::getCameraCount() const {
	return m_cameras.size();
}

inline Camera* Room::getCamera(int id) {
	return m_cameras[id].get();
}

inline const std::vector<std::shared_ptr<IBatch>>& Room::getBatches() {
	return _batches;
}

inline int Room::getBatchCount() const {
	return _batches.size();
}
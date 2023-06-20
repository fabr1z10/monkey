#pragma once

#include <vector>
#include <memory>
#include "node.h"
#include "runner.h"
//#include "light.h"

class Room {
public:
    Room();
    void setClearColor(int r, int g, int b);
    void addCamera(const std::string& id, std::shared_ptr<Camera>);
    void addSpriteBatch(const std::string& spriteSheet, const std::string& camName, int maxElements = 1000);
    void addLinesBatch(const std::string& camName, int maxElements = 1000);
    IBatch* getBatch(int shader, int id);
    Camera* getCamera(const std::string& id);
    ~Room();
    void update(double);
    void configure(Shader*, int);
    void draw(Shader*, int);
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
private:
	Camera* m_mainCamera;
    std::string m_id;
    std::shared_ptr<Node> m_root;
    std::unordered_map<std::string, std::shared_ptr<Camera>> m_cameras;
    std::unordered_map<std::type_index, std::shared_ptr<Runner> > m_runners;
    float m_ambientStrength;
    //std::vector<std::shared_ptr<Light>> m_lights;
    glm::vec4 m_clearColor;
    pybind11::function m_onStart;
    pybind11::function m_onEnd;


    std::vector<std::vector<std::shared_ptr<IBatch>>> _batches;

};


inline std::string Room::id() const {
    return m_id;
}
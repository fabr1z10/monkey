#pragma once


// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
#include <pybind11/pybind11.h>
#include <glm/glm.hpp>
#include "node.h"
#include "room.h"
#include "keylistener.h"

namespace py = pybind11;


class Engine {
public:
    static Engine& instance() {
        static Engine instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    //Engine();
    long getNextId();
    void start();
    void closeRoom();
    //void load(pybind11::object obj);
    void shutdown();
    //std::shared_ptr<Node> getNode(int);
    //void addNode(std::shared_ptr<Node>);
    //Shader* getShader(int);
    double getDeviceAspectRatio() const;
    glm::ivec2 getDeviceSize() const;
    glm::ivec2 getWindowSize() const;
    glm::vec4 getActualDeviceViewport() const;
    void setActualDeviceViewport(glm::vec4) ;
    static void WindowResizeCallback(GLFWwindow* win, int width, int height);
    static void cursor_pos_callback(GLFWwindow*, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow*, int, int, int);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	std::shared_ptr<Room> getRoom();

    // node handling
    std::shared_ptr<Node> getNode(int);
    void addNode(std::shared_ptr<Node>);
    void scheduleForRemoval(Node*);

    pybind11::object getConfig();
    bool isRunning() const;

    int getPixelScale() const;

	void registerToKeyboardEvent(KeyboardListener*);
	void unregisterToKeyboardEvent(KeyboardListener*);

private:
	template<typename T=Shader>
	std::shared_ptr<T> create_shader(ShaderType type, const std::string& vertex, const std::string& fragment, const std::string& vertexFormat) {
		auto shader = std::make_shared<T>(type, vertex, fragment, vertexFormat);
		return shader;
	}


    //void setupFramebufferRendering();
    Engine();
    void loadRoom();
    void loadShaders();
    pybind11::object m_game;

    std::string m_title;
    std::string m_roomId;
    glm::ivec2 m_windowSize;
    glm::ivec2 m_deviceSize;
    glm::vec4 m_actualDeviceViewport;
    glm::vec4 m_windowViewport;
    glm::vec4 m_clearColor;
    double m_deviceAspectRatio;
    // the current room

    // the current room
    std::shared_ptr<Room> m_room;





    double m_frameTime;
    double m_timeLastUpdate;

    bool m_run;
    bool m_shutdown;
    long m_nextId;
    bool m_enableMouse;

    // frambuffer stuff
    //GLuint _fb, _color, _depth;
    //unsigned int quadVAO, quadVBO;
    int m_pixelScaleFactor;

    // node management
    std::unordered_map<int, std::weak_ptr<Node>> m_allNodes;
    std::vector<Node*> m_scheduledForRemoval;
    std::unordered_map<int, std::function<std::shared_ptr<Shader>()>> m_shaderBuilders;
    std::vector<std::shared_ptr<Shader>> m_shaders;

	std::unordered_set<KeyboardListener*> m_keyboardListeners;

};

inline int Engine::getPixelScale() const {
    return m_pixelScaleFactor;
}

Engine& getEngine();

inline bool Engine::isRunning() const {
    return m_run;
}

inline pybind11::object Engine::getConfig() {
    return m_game;
}

inline long Engine::getNextId() {
    return m_nextId++;
}


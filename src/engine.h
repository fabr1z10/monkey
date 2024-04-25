#pragma once


// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
#include <pybind11/pybind11.h>
#include <glm/glm.hpp>
//#include "node.h"
#include "room.h"
#include "keylistener.h"
#include "mouselistener.h"
#include "batch.h"
#include "enginedraw.h"

namespace py = pybind11;


class Engine {
public:
    static Engine& instance() {
        static Engine instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    //Engine();
    void initialize();
    long getNextId();
    void start(py::module&);
    void run();
    void closeRoom();
    //void load(pybind11::object obj);
    void shutdown();
    //std::shared_ptr<Node> getNode(int);
    //void addNode(std::shared_ptr<Node>);
    //void addNode(std::shared_ptr<Node>);
    Shader* getShader(ShaderType);
    double getDeviceAspectRatio() const;
    glm::ivec2 getDeviceSize() const;
    glm::ivec2 getWindowSize() const;
    glm::vec4 getActualDeviceViewport() const;
    glm::vec4 getWindowViewport() const;
    void setActualDeviceViewport(glm::vec4) ;
    static void WindowResizeCallback(GLFWwindow* win, int width, int height);
    static void cursor_pos_callback(GLFWwindow*, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow*, int, int, int);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    std::shared_ptr<Room> getRoom();

    // node handling
    Node* getNode(int);
    std::unordered_set<Node*> getNodes(const std::string&);
    void addNode(Node*);
    void rmNode(Node*);
    void scheduleForRemoval(Node*);
    pybind11::object getConfig();
    bool isRunning() const;

    int getPixelScale() const;

	void registerToKeyboardEvent(KeyboardListener*);
	void unregisterToKeyboardEvent(KeyboardListener*);
	void registerToMouseEvent(MouseListener*);
	void unregisterToMouseEvent(MouseListener*);
	//IBatch* getBatch(int);

	//pybind11::function getScript(const std::string& name) const;
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
    //pybind11::object m_game;
    pybind11::module _factory;
    pybind11::object m_settings;
	pybind11::object _main;
    //pybind11::object m_scripts;
    std::string _title;
    std::string _roomId;
    glm::ivec2 _windowSize;
    glm::ivec2 _deviceSize;
    glm::vec4 m_actualDeviceViewport;
    glm::vec4 m_windowViewport;

    double _deviceAspectRatio;

    // the current room
    std::shared_ptr<Room> _room;





    double _frameTime;
    double _timeLastUpdate;

    bool m_run;
    bool m_shutdown;
    long m_nextId;
    bool _enableMouse;

    // frambuffer stuff
    //GLuint _fb, _color, _depth;
    //unsigned int quadVAO, quadVBO;
    int m_pixelScaleFactor;

    // node management
    std::unordered_map<int, Node*> m_allNodes;
    std::unordered_map<std::string, std::unordered_set<Node*>> m_labeledNodes;
    std::vector<Node*> m_scheduledForRemoval;


	std::unordered_set<KeyboardListener*> m_keyboardListeners;
	std::unordered_set<MouseListener*> m_mouseListeners;


    std::unique_ptr<EngineDraw> _engineDraw;
};

inline int Engine::getPixelScale() const {
    return m_pixelScaleFactor;
}

Engine& getEngine();

inline bool Engine::isRunning() const {
    return m_run;
}

inline pybind11::object Engine::getConfig() {
    return m_settings;
}

inline long Engine::getNextId() {
    return m_nextId++;
}


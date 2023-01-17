#include "engine.h"
#include <iostream>
#include "pyhelper.h"



GLFWwindow* window;

namespace py = pybind11;


Engine::Engine() : m_nextId(0), m_pixelScaleFactor(1) {

}



void Engine::load(pybind11::object obj) {


}


void Engine::start() {
	
	py::object settings = py::module::import("settings");
	if (settings) {
	} else {
		std::cout << " don't know\n";
	}

	m_title = py_get<std::string>(settings, "title");
	m_windowSize = py_get<glm::ivec2>(settings.attr("window_size"));
	m_deviceSize = py_get<glm::ivec2>(settings.attr("device_size"));
	assert(m_deviceSize[1] > 0);
	m_deviceAspectRatio = static_cast<double>(m_deviceSize[0]) / m_deviceSize[1];
	m_roomId = py_get<std::string>(settings, "room");
	m_frameTime = 1.0 / 60.0;
	m_timeLastUpdate = 0.0;
	m_enableMouse = py_get<bool>(settings, "enable_mouse", false);
	if (pybind11::hasattr(settings, "init")) {
		settings.attr("init").cast<pybind11::function>()();
	}
	
	exit(1);
	
	
	
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( m_windowSize[0], m_windowSize[1], m_title.c_str(), NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    // note: we are setting a callback for the frame buffer resize event,
    // so the dimensions we will get will be in pixels and NOT screen coordinates!
    glfwSetFramebufferSizeCallback(window, Engine::WindowResizeCallback);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        exit(1);
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    Engine::WindowResizeCallback(window, m_windowSize[0], m_windowSize[1]);
    glfwSetKeyCallback(window, key_callback );
    if (m_enableMouse) {
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
        //glfwSetScrollCallback(window, scroll_callback);
        //exit(1);
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_POINT_SMOOTH);
    // setupFramebufferRendering();

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    loadShaders();
    m_shutdown = false;
    glfwTerminate();
}


void Engine::shutdown() {
    //	// Close OpenGL window and terminate GLFW
    m_shutdown = true;
    //glfwTerminate();
}

void Engine::closeRoom() {
    m_run = false;
}

void Engine::loadRoom() {
}

void Engine::loadShaders() {



}

void Engine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

void Engine::mouse_button_callback(GLFWwindow* win, int button, int action, int mods) {

}

void Engine::cursor_pos_callback(GLFWwindow * win, double xpos, double ypos) {

}




// width and height will be pixels!!
void Engine::WindowResizeCallback(GLFWwindow* win, int width, int height) {
    // notify cameras
    if (height == 0) height = 1;
    std::cout << "(" << width << ", " << height << ")\n";
    float winAspectRatio = static_cast<float>(width) / height;
    auto& engine = Engine::instance();
    engine.m_windowSize = glm::ivec2(width, height);
    auto deviceSize = engine.getDeviceSize();
    auto dar = engine.getDeviceAspectRatio();
    int vx, vy, vw, vh;
    int sx = width / deviceSize.x;
    int sy = height / deviceSize.y;
    engine.m_pixelScaleFactor = std::min(sx, sy);
//	if (winAspectRatio > dar) {
//		// vertical bands
//		vw = (int) (height * dar);
//		vh = (int) (vw / dar);
//		vx = (int) ((width - vw) / 2);
//		vy = (int) ((height - vh) / 2);
//	} else {
//		// horizontal bands
//		vw = width;
//		vh = (int) (width / dar);
//		vx = 0;
//		vy = (int) ((height - vh) / 2);
//	}
    vw = deviceSize.x * engine.m_pixelScaleFactor;
    vh = deviceSize.y * engine.m_pixelScaleFactor;
    vx = (int) ((width - vw) / 2.);
    vy = (int) ((height - vh) / 2.);
    glPointSize(engine.m_pixelScaleFactor);
    glLineWidth(engine.m_pixelScaleFactor);
    engine.m_windowViewport = glm::vec4(vx, vy, vw, vh);
    //std::cout << "cazzocane: " << vw << ", " << vh << "\n";
    engine.setActualDeviceViewport(glm::vec4(vx, vy, static_cast<float>(vw) / deviceSize[0], static_cast<float>(vh) / deviceSize[1]));

}




double Engine::getDeviceAspectRatio() const {
    return m_deviceAspectRatio;
}

glm::ivec2 Engine::getDeviceSize() const {
    return m_deviceSize;
}

glm::ivec2 Engine::getWindowSize() const {
    return m_windowSize;
}

glm::vec4 Engine::getActualDeviceViewport() const {
    return m_actualDeviceViewport;
}

void Engine::setActualDeviceViewport(glm::vec4 viewport) {
    m_actualDeviceViewport = viewport;
    std::cout << m_actualDeviceViewport[2] << ", " << m_actualDeviceViewport[3] << "\n";
}




#include "engine.h"
#include <iostream>
#include "pyhelper.h"
#include "shaders/shaders.h"
#include "shaders/lightshader.h"



GLFWwindow* window;

namespace py = pybind11;


Engine::Engine() : m_nextId(0), m_pixelScaleFactor(1) {
    m_shaderBuilders[0] = [&] () { return create_shader(ShaderType::SHADER_COLOR, color_vs, color_fs, "3f4f"); };
    m_shaderBuilders[1] = [&] () { return create_shader(ShaderType::SHADER_TEXTURE, tex_vs, tex_fs, "3f2f4f"); };
    m_shaderBuilders[2] = [&] () { return create_shader(ShaderType::SHADER_TEXTURE_PALETTE, tex_vs, tex_pal_fs, "3f2f4f"); };
	// light shader
	m_shaderBuilders[3] = [&] () { return create_shader<LightShader>(
			ShaderType::SHADER_TEXTURE_LIGHT, tex_light_vs, tex_light_fs, "3f2f3f");};
	m_shaderBuilders[ShaderType::QUAD_SHADER] = [&] () { return create_shader(ShaderType::QUAD_SHADER, quad_vs, quad_fs, "2f1I"); };
	m_shaderBuilders[ShaderType::QUAD_SHADER] = [&] () { return create_shader(ShaderType::QUAD_SHADER, quad_vs, quad_fs, "2f1I"); };
    m_shaderBuilders[ShaderType::LINE_SHADER] = [&] () { return create_shader(ShaderType::LINE_SHADER, line_vs, line_fs, "1f1I"); };
}




//
//void Engine::load(pybind11::object obj) {
//
//
//}


void Engine::start() {
    m_game = py::module::import("game");
	py::object settings = py::module::import("game.settings");
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
	m_settings = settings;
	m_clearColor = py_get<glm::vec4>(settings, "clear_color", glm::vec4(0.f, 0.f, 0.4f, 0.f));
	if (pybind11::hasattr(settings, "init")) {
		settings.attr("init").cast<pybind11::function>()();
	}
	

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
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
    loadShaders();

}

void Engine::run() {
    m_shutdown = false;

    // init shaders
    for (size_t i = 0; i < m_shaders.size(); ++i) {
    	auto current = m_shaders[i].get();
    	current->use();
    	for (const auto& batch : _batches[i]) {
    		batch->configure(current);
    	}
    }

    // main loop
    while (!m_shutdown) {
        m_roomId = py_get<std::string>(m_settings, "room");
        std::cout << " loading room: " << m_roomId << std::endl;
        loadRoom();
        // start up all nodes and components
        m_room->iterate_dfs([](Node *n) { n->start(); });
        m_run = true;
        m_room->start();
        do {
            double currentTime = glfwGetTime();
            /// note: if I run the update only every frame time CPU goes to 100%. If I run it on
            /// every iter, it doesn't. Tried move the glfwSwapBuffers call (and successive) out of the loop
            /// and that seems to work.
            if (true || currentTime - m_timeLastUpdate >= m_frameTime) {
                m_timeLastUpdate = currentTime;

                // remove all entities scheduled for removal
                if (!m_scheduledForRemoval.empty()) {
                    for (auto & g : m_scheduledForRemoval) {
                        g->getParent()->removeChild(g->getId());
                        m_allNodes.erase(g->getId());
                    }
                    m_scheduledForRemoval.clear();
                }



                // restore if you want framebuffer rendering
                // glBindFramebuffer(GL_FRAMEBUFFER, _fb);
                // glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Draw nothing, see you in tutorial 2 !
                m_room->update(m_frameTime);


				for (size_t i = 0; i < m_shaders.size(); ++i) {
					auto current = m_shaders[i].get();
					current->use();
					for (const auto& batch : _batches[i]) {
						batch->draw(current);
					}
				}


                glfwSwapBuffers(window);
                glfwPollEvents();

                // Swap buffers
            }
            //m_shutdown = !(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            //             glfwWindowShouldClose(window) == 0);
            m_shutdown = !(glfwWindowShouldClose(window) == 0);

        } // Check if the ESC key was pressed or the window was closed
        while (m_run && !m_shutdown);
        m_room->end();
        m_allNodes.clear();
        if (m_room) {
            m_room->cleanUp();
        }
        m_room = nullptr;
        for (size_t i = 0; i < _batches.size(); ++i) {
            for (const auto& batch : _batches[i]) {
				batch->cleanUp();
			}
        }
    }
	//m_settings.attr("on_close")();

    _batches.clear();
    m_shaders.clear();
	m_game.release();
	m_settings.release();

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
    // generate current room
    std::cout << "sucalo\n";


    // create a batch
    // _batches.push_back(std::make_shared<SpriteBatch>(100, "smb1.png"));

    m_room = m_game.attr(m_roomId.c_str())().cast<std::shared_ptr<Room>>();


}

void Engine::addBatch(int shaderIndex, std::shared_ptr<IBatch> batch) {
    _batches[shaderIndex].push_back(batch);
}

void Engine::loadShaders() {

    auto shaders = py_get<std::vector<int>>(m_settings, "shaders");
    for (auto shaderId : shaders) {
        //auto flags = sh.second.cast<unsigned>();
        auto shader = m_shaderBuilders[shaderId]();
        //shader->setFlags(flags);
        //  modify here
        //m_shaderTypeToIndex[shader->getShaderType()] = m_shaders.size();
        m_shaders.push_back(shader);
        _batches.push_back(std::vector<std::shared_ptr<IBatch>>());
    }
    //auto batches = py_get<std::vector<pybind11::dict>>(m_settings, "sprite_batches");

//    for (const auto batch : batches) {
//        //auto b = std::make_shared<SpriteBatch>(batch);
//        auto bb = batch.cast<std::shared_ptr<Batch>>();
//        _batches.push_back(bb);
//    }
//
//    for (const auto& shader : m_shaders) {
//        shader->use();
//        for (const auto& batch : _batches) {
//            batch->configure(shader.get());
//        }
//    }


}

void Engine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	for (auto& s : Engine::instance().m_keyboardListeners) {
		s->keyCallback(window, key, scancode, action, mods);
	}
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

void Engine::addNode(std::shared_ptr<Node> node) {
    m_allNodes[node->getId()] = node;
}

std::shared_ptr<Node> Engine::getNode(int id) {
    auto it = m_allNodes.find(id);
    if (it != m_allNodes.end()) {
        if (auto d = it->second.lock()) {
            return d;
        }
    }
    return nullptr;
}

void Engine::scheduleForRemoval(Node * node) {
    m_scheduledForRemoval.push_back(node);
}

std::shared_ptr<Room> Engine::getRoom() {
	return m_room;
}

void Engine::registerToKeyboardEvent(KeyboardListener * listener) {
	m_keyboardListeners.insert(listener);
}

void Engine::unregisterToKeyboardEvent(KeyboardListener * listener) {
	m_keyboardListeners.erase(listener);
}

//IBatch * Engine::getBatch(int id) {
//    return _batches[id].get();
//}
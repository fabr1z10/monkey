#include "engine.h"
#include <iostream>
#include "pyhelper.h"
#include "monkeyfu.h"
//#include "shaders/lightshader.h"
//#include "batch/quadbatch.h"
#include "assetmanager.h"
#include "error.h"
#include "batch/linebatch.h"



GLFWwindow* window;

namespace py = pybind11;


Engine::Engine() : m_nextId(0), m_pixelScaleFactor(1) {
//    m_shaderBuilders[0] = [&] () { return create_shader(ShaderType::SHADER_COLOR, color_vs, color_fs, "3f4f"); };
//    m_shaderBuilders[1] = [&] () { return create_shader(ShaderType::SHADER_TEXTURE, tex_vs, tex_fs, "3f2f4f"); };
//    m_shaderBuilders[2] = [&] () { return create_shader(ShaderType::SHADER_TEXTURE_PALETTE, tex_vs, tex_pal_fs, "3f2f4f"); };
//	// light shader
//	m_shaderBuilders[3] = [&] () { return create_shader<LightShader>(
//			ShaderType::SHADER_TEXTURE_LIGHT, tex_light_vs, tex_light_fs, "3f2f3f");};
//	m_shaderBuilders[ShaderType::QUAD_SHADER] = [&] () { return create_shader(ShaderType::QUAD_SHADER, quad_vs, quad_fs, "2f1I"); };
//	m_shaderBuilders[ShaderType::QUAD_SHADER] = [&] () { return create_shader(ShaderType::QUAD_SHADER, quad_vs, quad_fs, "2f1I"); };
//    m_shaderBuilders[ShaderType::LINE_SHADER] = [&] () { return create_shader(ShaderType::LINE_SHADER, line_vs, line_fs, "1f1I"); };
}




//
//void Engine::load(pybind11::object obj) {
//
//
//}

pybind11::function Engine::getScript(const std::string &name) const {

//	return py_get<pybind11::function>(m_scripts, name);
}


void Engine::start() {
    // read the settings PY file
    try {


    	_factory = py::module::import("factory");
        auto settings = py::module::import("settings");
        _deviceSize = py_get<glm::ivec2>(settings, "device_size");
        assert(_deviceSize[1] > 0);
        _windowSize = py_get<glm::ivec2>(settings, "window_size", _deviceSize);
        _deviceAspectRatio = static_cast<double>(_deviceSize[0]) / _deviceSize[1];
        _roomId = py_get<std::string>(settings, "room");
        _frameTime = 1.0 / 60.0;
        _timeLastUpdate = 0.0;
        _enableMouse = py_get<bool>(settings, "enable_mouse", false);
        _title = py_get<std::string>(settings, "title", "Unknown");
        m_settings = settings;
		if (pybind11::hasattr(_factory, "init")) {
			_factory.attr("init")();
		}

		auto assetDirs = py_get<std::vector<std::string>>(m_settings, "asset_directories", std::vector<std::string>());
		for (const auto& dir : assetDirs) {
			AssetManager::instance().addDirectory(dir);
		}

    } catch (std::runtime_error& err) {
        GLIB_FAIL(err.what());
    }
//		// check if spritesheet has been loaded
////        if (m_spriteSheets.count(sheetName) == 0) {
////        	// load spritesheet
////        	m_spriteSheets.insert(std::make_pair(sheetName, std::make_shared<SpriteSheet>(f)));
////        }
////		auto sheet = m_spriteSheets.at(sheetName);
//		//auto sheetFile = f["sheet"].as<std::string>();
//		auto spritesNode = f["sprites"];
//		auto sheet = f["sheet"].as<std::string>();
//		auto * batch = Engine::instance().getRoom()->addSpriteBatch(sheet);





//	m_scripts = py::module::import("scripts");


    //auto useFrameBuffer = py_get<bool>(settings,"enable_framebuffer", false);






//	auto dataFiles = py_get<pybind11::dict>(settings, "data", pybind11::dict());
//	for (const auto& d : dataFiles) {
//	    readDataFile(d.first.cast<std::string>(), d.second.cast<std::string>());
//	}
	

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
    window = glfwCreateWindow( _windowSize[0], _windowSize[1], _title.c_str(), NULL, NULL);
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
    Engine::WindowResizeCallback(window, _windowSize[0], _windowSize[1]);
    glfwSetKeyCallback(window, key_callback );
    if (_enableMouse) {
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
//    if (useFrameBuffer) {
    _engineDraw = std::make_unique<FrameBufferEngineDraw>();
//    } else {
//        _engineDraw = std::make_unique<BasicEngineDraw>();
//    }
    // Dark blue background
    //loadShaders();

    auto shaders = py_get<std::vector<int>>(m_settings, "shaders");
    for (const auto& shader : shaders) {
		_engineDraw->addShader(shader);
    }
//
    _engineDraw->initShaders();

//    // load fonts
//    auto fonts = py_get<py::dict>(m_settings, "fonts", py::dict());
//    for (const auto& font : fonts) {
//        auto name = font.first.cast<std::string>();
//        auto file = font.second.cast<std::string>();
//        std::cout << name << ", " << file << "\n";
//        AssetManager::instance().getFont(name, file);
//    }
}

void Engine::initialize() {
	// WHY LOADING ALL SPRITESHEETS? no sense
//	auto sheets = py_get<pybind11::dict>(m_settings, "spritesheets", pybind11::dict());
//	for (const auto& sheet : sheets) {
//
//		auto id = sheet.first.cast<std::string>();
//		auto file = sheet.second.cast<std::string>();
//		std::cout << "READING SPRITESHEET " << id << " AT " << file << "\n";
//		AssetManager::instance().readSpritesheet(id, file);
//
//	}
	// check game initialization function
//	auto onStartup = py_get<pybind11::function>(m_settings, "on_startup", pybind11::function());
//	if (onStartup) {
//		onStartup();
//	}


}

void Engine::run() {
    m_shutdown = false;



    initialize();

    // main loop
    while (!m_shutdown) {
        _roomId = py_get<std::string>(m_settings, "room");
		m_scheduledForRemoval.clear();
        std::cout << "Loading room: " << _roomId << std::endl;
        loadRoom();
        // start up all nodes and components
//        m_room->iterate_dfs([](Node *n) { n->start(); });
        m_run = true;
        _room->start();

        do {
            double currentTime = glfwGetTime();
            /// note: if I run the update only every frame time CPU goes to 100%. If I run it on
            /// every iter, it doesn't. Tried move the glfwSwapBuffers call (and successive) out of the loop
            /// and that seems to work.
            if (true || currentTime - _timeLastUpdate >= _frameTime) {
                _timeLastUpdate = currentTime;

                // remove all entities scheduled for removal
                if (!m_scheduledForRemoval.empty()) {
                    for (auto & g : m_scheduledForRemoval) {
                        g->getParent()->removeChild(g->getId());
                        m_allNodes.erase(g->getId());
                    }
                    m_scheduledForRemoval.clear();
                }



                // restore if you want framebuffer rendering
                //glBindFramebuffer(GL_FRAMEBUFFER, _fb);
                //glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

                _room->update(_frameTime);

                _engineDraw->draw(_room.get());
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
            //m_shutdown = !(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            //             glfwWindowShouldClose(window) == 0);
            m_shutdown = !(glfwWindowShouldClose(window) == 0);

        } // Check if the ESC key was pressed or the window was closed
        while (m_run && !m_shutdown);
        _room->end();
        m_allNodes.clear();
        if (_room) {
            _room->cleanUp();
        }
        //m_room = nullptr;
//        for (size_t i = 0; i < _batches.size(); ++i) {
//            for (const auto& batch : _batches[i]) {
//				batch->cleanUp();
//			}
//        }
    }
	//m_settings.attr("on_close")();

    //_batches.clear();
    //_engineDraw->shutdown();
	//m_game.release();
	m_settings.release();
    _factory.release();
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
//    // generate current room
//    std::cout << "sucalo\n";
//
//
//    // create a batch
//    // _batches.push_back(std::make_shared<SpriteBatch>(100, "smb1.png"));
//    //for (auto& batch : _batches) batch.clear();
    _room = std::make_shared<Room>();
    auto roomBuilder = _factory.attr("create_room");
    if (!roomBuilder) {
    	GLIB_FAIL("no function create_room found in factory module!");
    }
    roomBuilder(_room);
//    m_game.attr(_roomId.c_str())(m_room);
	_engineDraw->init(_room.get());
//
//	//m_room = m_game.attr(m_roomId.c_str())().cast<std::shared_ptr<Room>>();
//    // init shaders


}

Shader* Engine::getShader(ShaderType type) {
    return nullptr;
	//return _engineDraw->getShader(type);
}



void Engine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	for (auto& s : Engine::instance().m_keyboardListeners) {
		auto retval = s->keyCallback(window, key, scancode, action, mods);
		if (retval == 1) {
		    break;
		}
	}
}

void Engine::mouse_button_callback(GLFWwindow* win, int button, int action, int mods) {
	for (auto &listener : Engine::instance().m_mouseListeners) {
		listener->mouseButtonCallback(win, button, action, mods);
	}
}

void Engine::cursor_pos_callback(GLFWwindow * win, double xpos, double ypos) {
	for (auto &listener : Engine::instance().m_mouseListeners) {
		listener->cursorPosCallback(win, xpos, ypos);
	}
}

void Engine::registerToMouseEvent(MouseListener* listener) {
	m_mouseListeners.insert(listener);
}

void Engine::unregisterToMouseEvent(MouseListener * listener) {
	m_mouseListeners.erase(listener);

}


// width and height will be pixels!!
void Engine::WindowResizeCallback(GLFWwindow* win, int width, int height) {
    // notify cameras
    if (height == 0) height = 1;
    //std::cout << "(" << width << ", " << height << ")\n";
    float winAspectRatio = static_cast<float>(width) / height;
    auto& engine = Engine::instance();
    engine._windowSize = glm::ivec2(width, height);
    auto deviceSize = engine.getDeviceSize();
    auto dar = engine.getDeviceAspectRatio();
    int vx, vy, vw, vh;
    int sx = width / deviceSize.x;
    int sy = height / deviceSize.y;
    engine.m_pixelScaleFactor = std::min(sx, sy);
	if (winAspectRatio > dar) {
		// vertical bands
		vw = (int) (height * dar);
		vh = (int) (vw / dar);
		vx = (int) ((width - vw) / 2);
		vy = (int) ((height - vh) / 2);
	} else {
		// horizontal bands
		vw = width;
        vh = (int) (width / dar);
		vx = 0;
		vy = (int) ((height - vh) / 2);
	}
//    vw = deviceSize.x * engine.m_pixelScaleFactor;
//    vh = deviceSize.y * engine.m_pixelScaleFactor;
//    vx = (int) ((width - vw) / 2.);
//    vy = (int) ((height - vh) / 2.);
//    glPointSize(engine.m_pixelScaleFactor);
//    glLineWidth(engine.m_pixelScaleFactor);
    engine.m_windowViewport = glm::vec4(vx, vy, vw, vh);
    //std::cout << "cazzocane: " << vw << ", " << vh << "\n";
    engine.setActualDeviceViewport(glm::vec4(0, 0, deviceSize.x, deviceSize.y));//glm::vec4(vx, vy, static_cast<float>(vw) / deviceSize[0], static_cast<float>(vh) / deviceSize[1]));

}




double Engine::getDeviceAspectRatio() const {
    return _deviceAspectRatio;
}

glm::ivec2 Engine::getDeviceSize() const {
    return _deviceSize;
}

glm::ivec2 Engine::getWindowSize() const {
    return _windowSize;
}

glm::vec4 Engine::getActualDeviceViewport() const {
    return m_actualDeviceViewport;
}
glm::vec4 Engine::getWindowViewport() const {
    return m_windowViewport;
}

void Engine::setActualDeviceViewport(glm::vec4 viewport) {
    m_actualDeviceViewport = viewport;
    //std::cout << m_actualDeviceViewport[2] << ", " << m_actualDeviceViewport[3] << "\n";
}

void Engine::addNode(std::shared_ptr<Node> node) {
    m_allNodes[node->getId()] = node.get();
    auto label = node->getTag();
    if (!label.empty()) {
    	if (m_labeledNodes.count(label) == 0) {
    		m_labeledNodes[label] = std::unordered_set<Node*>();
    	}
    	m_labeledNodes.at(label).insert(node.get());
    }
}

void Engine::rmNode(Node* node) {
	auto label = node->getTag();
	m_allNodes.erase(node->getId());

	if (!label.empty()) {
		m_labeledNodes.at(label).erase(node);
	}

}

Node* Engine::getNode(int id) {
    auto it = m_allNodes.find(id);
    if (it != m_allNodes.end()) {
       return it->second;
    }
    return nullptr;
}

std::unordered_set<Node *> Engine::getNodes(const std::string & label) {
	auto it = m_labeledNodes.find(label);
	if (it != m_labeledNodes.end()) {
		return m_labeledNodes.at(label);
	}
	return std::unordered_set<Node*>();

}

void Engine::scheduleForRemoval(Node * node) {
    m_scheduledForRemoval.push_back(node);
}

std::shared_ptr<Room> Engine::getRoom() {
	return _room;
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
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include "png.h"
#include "engine.h"
#include "node.h"
#include <iostream>
#include "monkeyfu.h"
#include "shape.h"
#include "shapes/convexpoly.h"
#include "shapes/circle.h"
#include "model.h"
#include "models/tiled.h"
#include "runners/collision_engine.h"
#include "components/sprite_collider.h"
#include "components/controller.h"
#include "components/controller2d.h"
#include "components/dynamics.h"
#include "components/statemachine.h"
#include "states/walk2d.h"
#include "states/playerwalk2d.h"
#include "components/follow.h"


namespace py = pybind11;


double add(double x, double y) {
    return x + y;
}


int read_png(const std::string& file) {
    FILE* infile;
    infile = fopen(file.c_str(), "rb");
    unsigned char sig[8];

    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */
    fread(sig, 1, 8, infile);
	
	int is_png = png_sig_cmp(sig, 0, 8);
	std::cout << sig[0] << sig[1] << ", " << is_png<< "\n";
	
    if (is_png != 0)
        return 1;
        //exit(1);   /* bad signature */

    return 0;
}

PYBIND11_MODULE(monkey, m) {
    m.doc() = "prova prova2"; // optional module docstring
    m.def("add", &add);
    m.def("read", &read_png);
	m.def("get_sprite", &getSprite);
    m.def("engine", &getEngine, py::return_value_policy::reference, "Gets the engine");
    m.attr("SHADER_COLOR") = static_cast<int>(ShaderType::SHADER_COLOR);
    m.attr("SHADER_TEXTURE") = static_cast<int>(ShaderType::SHADER_TEXTURE);
    m.attr("SHADER_TEXTURE_PALETTE") = static_cast<int>(ShaderType::SHADER_TEXTURE_PALETTE);



    py::class_<Engine>(m, "Engine")
        //.def(py::init<>())
        .def("start", &Engine::start)
        .def("shutdown", &Engine::shutdown);

    py::class_<Room, std::shared_ptr<Room>>(m, "Room")
        .def(py::init<const std::string&>())
		.def("add_runner", &Room::addRunner)
        .def("root", &Room::getRoot, py::return_value_policy::reference);

    py::class_<Node, std::shared_ptr<Node>>(m, "Node")
        .def(py::init<>())
        .def("get_camera", &Node::getCamera)
        .def("set_camera", &Node::setCamera)
        .def_property("user_data", &Node::getUserData, &Node::setUserData)
        .def_property("active", &Node::active, &Node::setActive)
        .def("get_parent",&Node::getParent, py::return_value_policy::reference)
        .def("get_children", &Node::getChildren)
        .def_property_readonly("id", &Node::getId)
        .def("add", &Node::add)
        .def("move_to", &Node::moveTo)
        .def("set_position", &Node::setPosition)
        .def("set_model", &Node::setModel)
        .def("add_component", &Node::addComponent)
        .def("remove", &Node::remove);

    py::class_<Camera, std::shared_ptr<Camera>>(m, "camera")
        .def("set_bounds", &Camera::setBounds)
        .def("set_position", &Camera::setPosition)
        .def(py::init<const py::kwargs&>());

    py::class_<OrthoCamera, Camera, std::shared_ptr<OrthoCamera>>(m, "camera_ortho")
        .def(py::init<float, float, const py::kwargs&>());

    py::class_<PerspectiveCamera, Camera, std::shared_ptr<PerspectiveCamera>>(m, "camera_perspective")
        .def(py::init<const py::kwargs&>());

    py::class_<Shape, std::shared_ptr<Shape>>(m, "shape")
        .def(py::init<>());

    py::class_<ConvexPoly, Shape, std::shared_ptr<ConvexPoly>>(m, "convex_poly")
        .def(py::init<const py::array_t<float>&>());

    py::class_<Rect, ConvexPoly, std::shared_ptr<Rect>>(m, "rect")
        .def(py::init<float, float, const py::kwargs&>());

    py::class_<Circle, Shape, std::shared_ptr<Circle>>(m, "circle")
        .def(py::init<float, const py::kwargs&>());

    py::class_<Segment, Shape, std::shared_ptr<Segment>>(m, "segment")
        .def(py::init<float, float, float, float>());

    /// --- models ---
    py::module_ mm = m.def_submodule("models");
    py::class_<Model, std::shared_ptr<Model>>(mm, "Model")
        .def(py::init<int>());
    py::class_<TiledModel, Model, std::shared_ptr<TiledModel>>(mm, "tiled")
        .def(py::init<const pybind11::kwargs&>());
	py::class_<AnimatedTiledModel, Model, std::shared_ptr<AnimatedTiledModel>>(mm, "tiled_animated")
		.def(py::init<const pybind11::kwargs&>());
	py::class_<Sprite, Model, std::shared_ptr<Sprite>>(mm, "sprite");

	/// --- runners ---
	py::class_<Runner, std::shared_ptr<Runner>>(m, "runner");
	py::class_<CollisionEngine, Runner, std::shared_ptr<CollisionEngine>>(m, "collision_engine")
		.def("add_response", &CollisionEngine::addResponse)
		.def(py::init<float, float, float>());

	/// --- components ---
	py::class_<Component, std::shared_ptr<Component>>(m, "component");

	py::class_<Collider, Component, std::shared_ptr<Collider>>(m, "icollider")
		.def_property_readonly("bounds", &Collider::bounds)
		.def("set_collision_flag", &Collider::setCollisionFlag);

	py::class_<SimpleCollider, Collider, std::shared_ptr<SimpleCollider>>(m, "collider")
		.def(py::init<std::shared_ptr<Shape>, int, int, int>());

	py::class_<SpriteCollider, Collider, std::shared_ptr<SpriteCollider>>(m, "sprite_collider")
		.def(py::init<int, int, int, const pybind11::kwargs&>());

	py::class_<Controller, Component, std::shared_ptr<Controller>>(m, "controller")
		.def_property_readonly("grounded", &Controller::grounded)
		.def_property_readonly("size", &Controller::getSize)
		.def("set_size", &Controller::setSize);

	py::class_<Controller2D, Controller, std::shared_ptr<Controller2D>>(m, "controller_2d")
		.def(py::init<py::kwargs&>());

	py::class_<Dynamics, Component, std::shared_ptr<Dynamics>>(m, "dynamics")
		.def_readwrite("velocity", &Dynamics::m_velocity)
		.def(py::init<const pybind11::kwargs&>());

	py::class_<Follow, Component, std::shared_ptr<Follow>>(m, "follow")
		.def(py::init<std::shared_ptr<Camera>, pybind11::tuple&, pybind11::tuple&>());

	py::class_<Platform, Component, std::shared_ptr<Platform>>(m, "platform")
		.def(py::init<>());

	py::class_<StateMachine, Component, std::shared_ptr<StateMachine>>(m, "state_machine")
		.def("add", &StateMachine::addState)
		.def("set_initial_state", &StateMachine::setInitialState)
		.def(py::init<>());

	/// --- states ---
	py::class_<State, std::shared_ptr<State>>(m, "state");
	py::class_<Walk2D, State, std::shared_ptr<Walk2D>>(m, "walk_2d");
	py::class_<PlayerWalk2D, State, std::shared_ptr<PlayerWalk2D>>(m, "walk_2d_player")
		.def(py::init<const std::string&, py::kwargs&>());


}

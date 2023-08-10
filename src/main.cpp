#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include "png.h"
#include "engine.h"
#include "node.h"
#include "models/modelmake.h"
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
#include "components/move_translate.h"
#include "shapes/aabb.h"
#include "states/climb.h"
#include "components/keyboard.h"
#include "states/foewalk2d.h"
#include "states/idle.h"
#include "states/bounce.h"
#include "runners/scheduler.h"
#include "actions/delay.h"
#include "actions/nodeaction.h"
#include "actions/blink.h"
#include "actions/callfunc.h"
#include "actions/moveacc.h"
#include "actions/move.h"
#include "actions/animate.h"
#include "actions/remove.h"
#include "components/scriptplayer.h"
#include "actions/move_dynamics.h"
#include "runners/lighting.h"
#include "models/quad.h"
//#include "batch/linebatch.h"
#include "models/lines.h"

#include "states/attack.h"
#include "models/text.h"
#include "multinode.h"
#include "nodes/itemlist.h"


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
	m.def("get_multi", &getMulti);
	m.def("get_node", &getNode);
	//m.def("get_batch", &getBatch, py::return_value_policy::reference);
    m.def("get_camera", &getCamera, py::return_value_policy::reference);
	m.def("close_room", &closeRoom);
	m.def("play", &playScript);
	m.def("engine", &getEngine, py::return_value_policy::reference, "Gets the engine");
	m.def("engine", &getEngine, py::return_value_policy::reference, "Gets the engine");
    m.attr("SHADER_COLOR") = static_cast<int>(ShaderType::SHADER_COLOR);
    m.attr("SHADER_TEXTURE") = static_cast<int>(ShaderType::SHADER_TEXTURE);
    m.attr("SHADER_TEXTURE_PALETTE") = static_cast<int>(ShaderType::SHADER_TEXTURE_PALETTE);
	m.attr("SHADER_TEXTURE_LIGHT") = static_cast<int>(ShaderType::SHADER_TEXTURE_LIGHT);
    m.attr("SHADER_BATCH") = static_cast<int>(ShaderType::QUAD_SHADER);
    m.attr("SHADER_LINEBATCH") = static_cast<int>(ShaderType::LINE_SHADER);
    m.attr("ALIGN_LEFT") = static_cast<int>(HAlign::LEFT);
	m.attr("ALIGN_CENTER") = static_cast<int>(HAlign::CENTER);
	m.attr("ALIGN_RIGHT") = static_cast<int>(HAlign::RIGHT);
	m.attr("VALIGN_TOP") = static_cast<int>(VAlign::TOP);
	m.attr("VALIGN_CENTER") = static_cast<int>(VAlign::CENTER);
	m.attr("VALIGN_BOTTOM") = static_cast<int>(VAlign::BOTTOM);


    py::class_<Engine>(m, "Engine")
        //.def(py::init<>())
        .def("start", &Engine::start)
        .def("run", &Engine::run)
        .def("shutdown", &Engine::shutdown);

    py::class_<Room, std::shared_ptr<Room>>(m, "Room")
        .def(py::init<>())
        .def("add_spritesheet", &Room::addSpritesheet)
		.def("add_runner", &Room::addRunner)
		.def("add_camera", &Room::addCamera)
		.def("add_sprite_batch", &Room::addSpriteBatch)
        .def("add_line_batch", &Room::addLinesBatch)
		.def("set_clear_color", &Room::setClearColor)
		.def("set_main_cam", &Room::setMainCam)
        .def("root", &Room::getRoot, py::return_value_policy::reference);

    py::class_<Node, std::shared_ptr<Node>>(m, "Node")
        .def(py::init<const std::string&>(), py::arg("lebel")="")
        .def("get_camera", &Node::getCamera)
        .def("set_camera", &Node::setCamera)
		.def_property("scale", &Node::getScale, &Node::setScale)
        .def_property("user_data", &Node::getUserData, &Node::setUserData)
        .def_property("active", &Node::active, &Node::setActive)
        .def("get_parent",&Node::getParent, py::return_value_policy::reference)
        .def("get_children", &Node::getChildren)
        .def_property_readonly("id", &Node::getId)
        .def_property_readonly("label", &Node::getLabel)
        .def("add", &Node::add)
        .def("move_to", &Node::moveTo)
        .def("set_position", &Node::setPosition)
        .def("set_model", &Node::setModel)
        .def("set_palette", &Node::setPalette)
        .def("set_animation", &Node::setAnimation)
        .def("add_component", &Node::addComponent)
		.def("set_state", &Node::setState)
		.def_property_readonly("state", &Node::getState)
		.def_property_readonly("x", &Node::getX)
		.def_property_readonly("y", &Node::getY)
		.def_property_readonly("z", &Node::getZ)
		.def_property("flip_x",&Node::getFlipX, &Node::setFlipX)
		.def("get_state_machine", &Node::getComponent<StateMachine>, py::return_value_policy::reference)
		.def("get_sprite_collider", &Node::getComponent<SpriteCollider>, py::return_value_policy::reference)
		.def("get_dynamics", &Node::getComponent<Dynamics>, py::return_value_policy::reference)
        .def("remove", &Node::remove);

    py::class_<MultiNode, Node, std::shared_ptr<MultiNode>>(m, "MultiNode")
    	.def("set_node_model", &MultiNode::setNodeModel);

    py::class_<ItemList, Node, std::shared_ptr<ItemList>>(m, "ItemList")
    	.def(py::init<const pybind11::kwargs&>())
		.def("add_item", &ItemList::addItem)
		.def("clear", &ItemList::clear);
		//.def(py::init<const std::string&>())


    py::class_<IBatch, std::shared_ptr<IBatch>>(m, "batch");
        //.def("add", &IBatch::add);
	py::class_<Batch<QuadBatchVertexData>, IBatch, std::shared_ptr<Batch<QuadBatchVertexData>>>(m, "qbatch");
    py::class_<Batch<LineBatchVertexData>, IBatch, std::shared_ptr<Batch<LineBatchVertexData>>>(m, "lbatch");

//    py::class_<QuadBatch, Batch<QuadBatchVertexData>, std::shared_ptr<QuadBatch>>(m, "sprite_batch")
//        .def(py::init<int, std::shared_ptr<Camera>, const std::string&>());
//    py::class_<LineBatch, Batch<LineBatchVertexData>, std::shared_ptr<LineBatch>>(m, "line_batch")
//        .def(py::init<int, std::shared_ptr<Camera>>());

    py::class_<Camera, std::shared_ptr<Camera>>(m, "camera")
        .def("set_bounds", &Camera::setBounds)
        .def("set_position", &Camera::setPosition);
        //.def(py::init<const py::kwargs&>());

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

	py::class_<AABB, Shape, std::shared_ptr<AABB>>(m, "aabb")
		.def(py::init<float, float, float, float>());


	/// --- models ---
    py::module_ mm = m.def_submodule("models");
	mm.def("make_plane", &ModelMaker::pippo);
	mm.def("make", &ModelMaker::makeModel);
    py::class_<Model, std::shared_ptr<Model>>(mm, "Model");
        //.def(py::init<>());
//    py::class_<Quad, Model, std::shared_ptr<Quad>>(mm, "quad")
//        .def(py::init<const pybind11::kwargs&>());
    //py::class_<Lines, Model, std::shared_ptr<Lines>>(mm)
    py::class_<PolyChain, Model, std::shared_ptr<PolyChain>>(mm, "lines")
        .def(py::init<const pybind11::kwargs&>());

//    py::class_<TiledModel, Model, std::shared_ptr<TiledModel>>(mm, "tiled")
//        .def(py::init<const pybind11::kwargs&>());
//	py::class_<AnimatedTiledModel, Model, std::shared_ptr<AnimatedTiledModel>>(mm, "tiled_animated")
//		.def(py::init<const pybind11::kwargs&>());
	py::class_<Sprite, Model, std::shared_ptr<Sprite>>(mm, "sprite");
		//.def(py::init<const pybind11::kwargs&>());
	py::class_<Text, Model, std::shared_ptr<Text>>(mm, "text")
		.def(py::init<const pybind11::kwargs&>());


//    py::class_<MultiModel, Model, std::shared_ptr<MultiModel>>(mm, "multi_sprite")
//        //.def(py::init<std::shared_ptr<IBatch>>())
//        .def(py::init<const pybind11::kwargs&>())
//        .def("set_model", &MultiModel::setNodeModel);


	/// --- runners ---
	py::class_<Runner, std::shared_ptr<Runner>>(m, "runner");
	py::class_<CollisionEngine, Runner, std::shared_ptr<CollisionEngine>>(m, "collision_engine")
		.def("add_response", &CollisionEngine::addResponse)
		.def(py::init<float, float, float>());
	py::class_<Scheduler, Runner, std::shared_ptr<Scheduler>>(m, "scheduler")
		.def("add", &Scheduler::add)
		.def(py::init<>());
	py::class_<Lighting, Runner, std::shared_ptr<Lighting>>(m, "lighting")
		.def(py::init<>())
		.def("set_ambient", &Lighting::setAmbient)
		.def("add_light", &Lighting::addLight);


	/// --- lights ---
	py::class_<Light, std::shared_ptr<Light>>(m, "light");
	py::class_<DirectionalLight, Light, std::shared_ptr<DirectionalLight>>(m, "light_directional")
		.def(py::init<const pybind11::kwargs&>());

	/// --- scripts & actions
	py::class_<Script, std::shared_ptr<Script>>(m, "script")
		.def("add", &Script::add)
		.def(py::init<const pybind11::kwargs&>());


	/// --- actions ---
	py::module_ ma = m.def_submodule("actions");
	py::class_<Action, std::shared_ptr<Action>>(ma, "action");
	py::class_<NodeAction, Action, std::shared_ptr<NodeAction>>(ma, "node_action");
	py::class_<Delay, Action, std::shared_ptr<Delay>>(ma, "delay")
		.def(py::init<float>());
	py::class_<Animate, NodeAction, std::shared_ptr<Animate>>(ma, "animate")
		.def(py::init<const pybind11::kwargs&>());
	py::class_<Blink, NodeAction, std::shared_ptr<Blink>>(ma, "blink")
		.def(py::init<const pybind11::kwargs&>());
	py::class_<CallFunc, Action, std::shared_ptr<CallFunc>>(ma, "callfunc")
		.def(py::init<pybind11::function>());
	py::class_<MoveAccelerated, NodeAction, std::shared_ptr<MoveAccelerated>>(ma, "move_accelerated")
		.def(py::init<const pybind11::kwargs&>());
	py::class_<MoveDynamics, NodeAction, std::shared_ptr<MoveDynamics>>(ma, "move_dynamics")
		.def(py::init<const pybind11::kwargs&>());
	py::class_<Move, NodeAction, std::shared_ptr<Move>>(ma, "move")
		.def(py::init<const pybind11::kwargs&>());
	py::class_<MoveBy, NodeAction, std::shared_ptr<MoveBy>>(ma, "move_by")
		.def(py::init<const pybind11::kwargs&>());
	py::class_<RemoveNode, NodeAction, std::shared_ptr<RemoveNode>>(ma, "remove")
		.def(py::init<const pybind11::kwargs&>());


	/// --- components ---
	py::class_<Component, std::shared_ptr<Component>>(m, "component");

	py::class_<Collider, Component, std::shared_ptr<Collider>>(m, "icollider")
		.def_property_readonly("bounds", &Collider::bounds)
		.def("set_collision_flag", &Collider::setCollisionFlag);

	py::class_<SimpleCollider, Collider, std::shared_ptr<SimpleCollider>>(m, "collider")
		.def(py::init<std::shared_ptr<Shape>, int, int, int>());

	py::class_<SpriteCollider, Collider, std::shared_ptr<SpriteCollider>>(m, "sprite_collider")
		.def("set_override", &SpriteCollider::setCollisionOverride)
		.def(py::init<int, int, int, const pybind11::kwargs&>());

	py::class_<Controller, Component, std::shared_ptr<Controller>>(m, "controller")
		.def_property_readonly("grounded", &Controller::grounded)
		.def_property_readonly("size", &Controller::getSize)
		.def("set_size", &Controller::setSize);

	py::class_<Controller2D, Controller, std::shared_ptr<Controller2D>>(m, "controller_2d")
		.def(py::init<py::kwargs&>());

	py::class_<Dynamics, Component, std::shared_ptr<Dynamics>>(m, "dynamics")
		.def("set_velocity", &Dynamics::setVelocity)
		.def(py::init<const pybind11::kwargs&>());

	py::class_<Follow, Component, std::shared_ptr<Follow>>(m, "follow")
		.def(py::init<std::shared_ptr<Camera>, pybind11::tuple&, pybind11::tuple&>());

	py::class_<Platform, Component, std::shared_ptr<Platform>>(m, "platform")
		.def(py::init<>());

	py::class_<StateMachine, Component, std::shared_ptr<StateMachine>>(m, "state_machine")
		.def("add", &StateMachine::addState)
		.def("set_initial_state", &StateMachine::setInitialState)
		.def(py::init<const pybind11::kwargs&>());

	py::class_<MoveTranslate, Component, std::shared_ptr<MoveTranslate>>(m, "move_translate")
		.def(py::init<const pybind11::kwargs&>());

	py::class_<Keyboard, Component, std::shared_ptr<Keyboard>>(m, "keyboard")
		.def("add", &Keyboard::addFunction)
		.def(py::init<>());

	py::class_<ScriptPlayer, Component, std::shared_ptr<ScriptPlayer>>(m, "script_player")
		.def("play", &ScriptPlayer::play)
		.def(py::init<>());

	/// --- states ---
	py::class_<State, std::shared_ptr<State>>(m, "state");
	py::class_<Walk2D, State, std::shared_ptr<Walk2D>>(m, "walk_2d");
	py::class_<PlayerWalk2D, State, std::shared_ptr<PlayerWalk2D>>(m, "walk_2d_player")
		.def(py::init<>());
	py::class_<FoeWalk2D, State, std::shared_ptr<FoeWalk2D>>(m, "walk_2d_foe")
		.def(py::init<>());
	py::class_<Climb, State, std::shared_ptr<Climb>>(m, "climb")
		.def(py::init<>());
	py::class_<Idle, State, std::shared_ptr<Idle>>(m, "idle")
		.def(py::init<>());
	py::class_<Bounce, State, std::shared_ptr<Bounce>>(m, "bounce")
		.def(py::init<>());
    py::class_<Attack, State, std::shared_ptr<Attack>>(m, "attack")
            .def(py::init<>());

}

#include <pybind11/pybind11.h>
//#include <pybind11/operators.h>
#include <pybind11/stl.h>
//#include "pyhelper.h"
//#include "png.h"
//#include "engine.h"
//#include "node.h"
//#include "models/modelmake.h"
//#include <iostream>
#include "monkeyfu.h"
#include "nodes/text.h"
//#include "shape.h"
//#include "shapes/convexpoly.h"
//#include "shapes/circle.h"
//#include "model.h"
#include "models/iquad.h"
#include "models/sprite.h"
//#include "runners/collision_engine.h"
//#include "components/sprite_collider.h"
//#include "components/controller.h"
//#include "components/controller2d.h"
//#include "components/dynamics.h"
//#include "components/statemachine.h"
//#include "states/walk2d.h"
//#include "states/playerwalk2d.h"
//#include "components/follow.h"
//#include "components/move_translate.h"
//#include "shapes/aabb.h"
//#include "states/climb.h"
//#include "components/keyboard.h"
//#include "states/foewalk2d.h"
//#include "states/idle.h"
//#include "states/bounce.h"
#include "runners/scheduler.h"
//#include "actions/delay.h"
//#include "actions/nodeaction.h"
//#include "actions/blink.h"
//#include "actions/callfunc.h"
//#include "actions/moveacc.h"
#include "actions/move.h"
#include "actions/animate.h"
//#include "actions/remove.h"
//#include "components/scriptplayer.h"
//#include "actions/move_dynamics.h"
//#include "runners/lighting.h"
//#include "models/quad.h"
////#include "batch/linebatch.h"
//#include "models/lines.h"
//
//#include "states/attack.h"
//#include "models/text.h"
//#include "multinode.h"
//#include "nodes/itemlist.h"
//#include "components/hotspot.h"
//#include "runners/hotspotmanager.h"
//#include "nodes/walkarea.h"
//#include "actions/walk.h"
//#include "components/scummcharacter.h"
//#include "components/texthotspot.h"
//#include "math/funcxy.h"
//#include "actions/turn.h"
//#include "actions/say.h"
//#include "models/paramcurve.h"
//#include "components/cursor.h"
//#include "skeletal/skeletal_model.h"
//#include "cam25.h"
//#include "components/controller3d.h"
//#include "shapes3d/aabb3d.h"
//#include "states/walk3d.h"
//#include "states/playerwalk3d.h"
//#include "states/top2d.h"
#include "batch/trianglebatch.h"
//#include "nodes/road.h"
//#include "models/tilemap.h"
//#include "components/mariocontroller.h"
//#include "shapes/polyline.h"
#include "components/controllers/sierra2d.h"
#include "models/modelmake.h"
#include "shapes/convexpoly.h"
#include "shapes/polyline.h"
#include "shapes/aabb.h"
#include "shapes/polygon.h"
#include "actions/sierra.h"
#include "actions/callfunc.h"
#include "actions/addnode.h"
#include "components/keyboard.h"
#include "actions/waitforkey.h"
#include "actions/delay.h"
#include "nodes/textedit.h"
#include "components/controllers/walk2d.h"
#include "components/follow.h"
#include "components/baseline.h"
#include "actions/remove.h"
#include "components/sprite_collider.h"
#include "components/switch.h"
#include "components/controllers/sierranpc.h"
#include "shapes/circle.h"
#include "runners/clock.h"
#include "nodes/multinode.h"
#include "models/multisprite.h"
#include "runners/walkarea.h"
#include "components/controllers/sierrafollow.h"
#include "runners/mousemanager.h"
#include "actions/walk.h"
#include "actions/turn.h"

#include "components/parallax.h"
#include "components/nodeevent.h"

#include "nodes/tileworld.h"
#include "components/controllers/tilecontroller.h"
#include "components/vehicle2d.h"
#include "components/controller.h"
#include "components/controller2d.h"
#include "components/mover.h"


//#include "nodes/textedit.h"
//#include "components/controllers/walk3d.h"
//#include "skeletal/skeletal_collider.h"

using namespace pybind11::literals; // to bring in the `_a` literal
using namespace shapes;

namespace py = pybind11;


double add(double x, double y) {
    return x + y;
}


//int read_png(const std::string& file) {
//    FILE* infile;
//    infile = fopen(file.c_str(), "rb");
//    unsigned char sig[8];
//
//    /* first do a quick check that the file really is a PNG image; could
//     * have used slightly more general png_sig_cmp() function instead */
//    fread(sig, 1, 8, infile);
//
//	int is_png = png_sig_cmp(sig, 0, 8);
//	std::cout << sig[0] << sig[1] << ", " << is_png<< "\n";
//
//    if (is_png != 0)
//        return 1;
//        //exit(1);   /* bad signature */
//
//    return 0;
//}

PYBIND11_MODULE(monkey, m) {
    m.doc() = "prova prova2"; // optional module docstring

    py::enum_<NodeState>(m, "NodeState")
        .value("ACTIVE", NodeState::ACTIVE)
        .value("PAUSED", NodeState::PAUSED)
        .value("INACTIVE", NodeState::INACTIVE)
        .export_values();

    m.def("add", &add);
    m.def("polyLineToPolygon", &polyLineToPolygon);
//    m.def("read", &read_png);
//    m.def("test",&add2, "i"_a = 1, "j"_a = 3);
    m.def("from_hex", &fromHex, py::arg("color"));
    m.def("read_data_file", &readDataFile);
    m.def("prova", &prova);
	m.def("get_sprite", &getSprite);
    m.def("get_tiled", &getTiled);
//	m.def("get_polymesh", &getPolyMesh);
//	m.def("get_multi", &getMulti);
	m.def("get_node", &getNode, py::return_value_policy::reference);
//	m.def("get_nodes", &getNodes, py::return_value_policy::reference);
//	//m.def("get_batch", &getBatch, py::return_value_policy::reference);
//    m.def("get_camera", &getCamera, py::return_value_policy::reference);
	m.def("close_room", &closeRoom);
	m.def("getClock", &getClock, py::return_value_policy::reference);
    m.def("play", &playScript);
    m.def("kill", &killScript);
//	m.def("engine", &getEngine, py::return_value_policy::reference, "Gets the engine");
	m.def("engine", &getEngine, py::return_value_policy::reference, "Gets the engine");

	m.attr("SHADER_BATCH_QUAD_PALETTE") = static_cast<int>(ShaderType::BATCH_QUAD_PALETTE);
//    m.attr("SHADER_BATCH_QUAD_NO_PALETTE") = static_cast<int>(ShaderType::BATCH_QUAD_NO_PALETTE);
    m.attr("SHADER_BATCH_LINES") = static_cast<int>(ShaderType::BATCH_LINES);
//
//	//m.attr("SHADER_BATCH_COLOR_TRI") = static_cast<int>(ShaderType::BATCH_COLOR_TRI);
//	m.attr("SHADER_SKELETAL") = static_cast<int>(ShaderType::SHADER_SKELETAL);
////	m.attr("SHADER_TEXTURE_LIGHT") = static_cast<int>(ShaderType::SHADER_TEXTURE_LIGHT);
////    m.attr("SHADER_BATCH") = static_cast<int>(ShaderType::QUAD_SHADER);
////    m.attr("SHADER_LINEBATCH") = static_cast<int>(ShaderType::LINE_SHADER);
    m.attr("HALIGN_LEFT") = static_cast<int>(HAlign::LEFT);
	m.attr("HALIGN_CENTER") = static_cast<int>(HAlign::CENTER);
	m.attr("HALIGN_RIGHT") = static_cast<int>(HAlign::RIGHT);

	m.attr("ANCHOR_TOPLEFT") = static_cast<int>(Anchor::TOPLEFT);
    m.attr("ANCHOR_TOPRIGHT") = static_cast<int>(Anchor::TOPRIGHT);
    m.attr("ANCHOR_BOTTOMLEFT") = static_cast<int>(Anchor::BOTTOMLEFT);
    m.attr("ANCHOR_BOTTOMRIGHT") = static_cast<int>(Anchor::BOTTOMRIGHT);
    m.attr("ANCHOR_CENTER") = static_cast<int>(Anchor::CENTER);
//	m.attr("VALIGN_TOP") = static_cast<int>(VAlign::TOP);
//	m.attr("VALIGN_CENTER") = static_cast<int>(VAlign::CENTER);
//	m.attr("VALIGN_BOTTOM") = static_cast<int>(VAlign::BOTTOM);
//
	py::enum_<FillType>(m, "FillType")
		.value("Outline", FillType::OUTLINE)
		.value("Solid", FillType::SOLID)
		.export_values();
//
    py::class_<Engine>(m, "Engine")
        //.def(py::init<>())
        .def("start", &Engine::start)
        .def("run", &Engine::run)
        .def("getRoom", &Engine::getRoom)
        .def("shutdown", &Engine::shutdown);

	py::class_<Room, std::shared_ptr<Room>>(m, "Room")
        .def(py::init<>())
//        .def("add_spritesheet", &Room::addSpritesheet)
		.def("add_runner", &Room::addRunner)
		.def("add_camera", &Room::addCamera)
		.def("add_batch", &Room::addBatch)
		.def("hasBatch", &Room::hasBatch)
		.def("addOnStart", &Room::addOnStart)
		.def("setOnEnd", &Room::setOnEnd)
//        //.def("add_line_batch", &Room::addLinesBatch)
		.def("set_clear_color", &Room::setClearColor)
//		.def("set_main_cam", &Room::setMainCam)
        .def("root", &Room::getRoot, py::return_value_policy::reference);
//
    py::class_<Node, std::shared_ptr<Node>>(m, "Node")
        .def(py::init<>())
        .def("set_position", py::overload_cast<float, float, float>(&Node::setPosition))
        .def("set_position", py::overload_cast<float, float>(&Node::setPosition))
        .def("set_angle", &Node::setAngle)
        .def("add_component", &Node::addComponent)
        .def("set_model", &Node::setModel)
        .def("get_model", &Node::getModel)
        .def("add", &Node::add)
        .def("get_children", &Node::getChildren)
        .def("remove", &Node::remove)
        .def("clear", &Node::clearChildren)
//        .def("get_switch", &Node::getComponent<Switch>, py::return_value_policy::reference)
		.def("getMouseArea", &Node::getComponent<MouseArea>, py::return_value_policy::reference)
		.def("getController", &Node::getComponent<WalkableCharacter>, py::return_value_policy::reference)
		.def("getRenderer", &Node::getComponent<Renderer>, py::return_value_policy::reference)
		.def("getNodes", &Node::getNodes, py::return_value_policy::reference)
		.def("setPalette", &Node::setPalette)
        .def("sendMessage", &Node::sendMessage)
        .def("addMessage", &Node::addMessage)
        //.def("setBehavior", &Node::setBehavior)
        //.def("addBehavior", &Node::addBehavior)
        .def("getLabelledComponent", &Node::getTaggedComponent, py::return_value_policy::reference)
        .def_property_readonly("id", &Node::getId)
        .def_property_readonly("x", &Node::getX)
		.def_property_readonly("y", &Node::getY)
		.def_property_readonly("z", &Node::getZ)
		.def_property_readonly("collisionTag", &Node::getCollisionTag)
        .def("setAnimation", &Node::setAnimation)
        .def_property_readonly("flip_x", &Node::getFlipX)
        .def_property("tag", &Node::getTag, &Node::setTag)
        .def_property("state", &Node::getState, &Node::setState)
		.def_property("user_data", &Node::getUserData, &Node::setUserData)
		.def_property("scale", &Node::getScale, &Node::setScale);


	py::class_<Text, Node, std::shared_ptr<Text>>(m, "Text")
    	.def(py::init<const std::string&, const std::string&, const std::string&, const pybind11::kwargs&>(),
    	        "batch"_a, "font"_a, "text"_a)
        .def("updateText", &Text::updateText)
        .def("getText", &Text::getText)
		.def_property_readonly("size", &Text::getSize);
	py::class_<TextEdit, Text, std::shared_ptr<TextEdit>>(m, "TextEdit")
		.def(py::init<const std::string&, const std::string&, const std::string&, const std::string&, const pybind11::kwargs&>(),
			 "batch"_a, "font"_a, "prompt"_a, "cursor"_a)
		.def("setText", &TextEdit::setText);
	py::class_<TileWorld, Node, std::shared_ptr<TileWorld>>(m, "TileWorld")
		.def(py::init<int, int, int, const std::string&, const std::string&, const pybind11::kwargs&>(), "width"_a, "height"_a, "size"_a, "file"_a, "batch"_a);
//        .def("get_camera", &Node::getCamera)
//        .def("set_camera", &Node::setCamera)


//        .def("get_parent",&Node::getParent, py::return_value_policy::reference)



//        .def_property("text", &Node::getText, &Node::setText)

//        .def("move", &Node::movea)
//        .def("move_to", &Node::moveTo)

//        .def("rotate", &Node::rotate)
//        .def("set_palette", &Node::setPalette)
//        .def_property_readonly("anim", &Node::getAnimation)

//		.def("set_state", &Node::setState)
//		.def_property_readonly("state", &Node::getState)
//		.def_property_readonly("x", &Node::getX)
//		.def_property_readonly("y", &Node::getY)
//		.def_property_readonly("z", &Node::getZ)
//		.def_property("flip_x",&Node::getFlipX, &Node::setFlipX)
//		.def("get_state_machine", &Node::getComponent<StateMachine>, py::return_value_policy::reference)
//		.def("get_sprite_collider", &Node::getComponent<SpriteCollider>, py::return_value_policy::reference)
//		.def("get_dynamics", &Node::getComponent<Dynamics>, py::return_value_policy::reference)

//
//    py::class_<MultiNode, Node, std::shared_ptr<MultiNode>>(m, "MultiNode")
//    	.def("set_node_model", &MultiNode::setNodeModel);
//
//    py::class_<ItemList, Node, std::shared_ptr<ItemList>>(m, "ItemList")
//    	.def(py::init<const pybind11::kwargs&>())
//		.def("add_item", &ItemList::addItem)
//		.def("clear", &ItemList::clear);
//		//.def(py::init<const std::string&>())
//

//
//
//	py::class_<WalkArea, Node, std::shared_ptr<WalkArea>>(m, "_walkarea")
//		.def("set_z_function", &WalkArea::setZFunction)
//		.def("set_wall", &WalkArea::setWall)
//		.def("set_scale_function", &WalkArea::setScaleFunction);
//
//	py::class_<WalkAreaPolygon, WalkArea, std::shared_ptr<WalkAreaPolygon>>(m, "WalkArea")
//		.def(py::init<const pybind11::kwargs&>());
//	py::class_<WalkAreaPolyline, WalkArea, std::shared_ptr<WalkAreaPolyline>>(m, "WalkArea_Line")
//		.def(py::init<const pybind11::kwargs&>());
//
//
//
//
    py::class_<IBatch, std::shared_ptr<IBatch>>(m, "batch");
	py::class_<Batch<QuadBatchVertexData>, IBatch, std::shared_ptr<Batch<QuadBatchVertexData>>>(m, "qbatch");
    py::class_<Batch<LineBatchVertexData>, IBatch, std::shared_ptr<Batch<LineBatchVertexData>>>(m, "lbatch");
	py::class_<Batch<TriBatchVertexData>, IBatch, std::shared_ptr<Batch<TriBatchVertexData>>>(m, "tbatch");
//
//
    py::class_<QuadBatch, Batch<QuadBatchVertexData>, std::shared_ptr<QuadBatch>>(m, "SpriteBatch")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<LineBatch, Batch<LineBatchVertexData>, std::shared_ptr<LineBatch>>(m, "LineBatch")
        .def(py::init<const pybind11::kwargs&>());
	py::class_<TriangleBatch, Batch<TriBatchVertexData>, std::shared_ptr<TriangleBatch>>(m, "TriangleBatch")
		.def(py::init<const pybind11::kwargs&>());
//	py::class_<ProvaBatch, IBatch, std::shared_ptr<ProvaBatch>>(m, "prova_batch")
//		.def(py::init<const pybind11::kwargs&>());
//
    py::class_<Camera, std::shared_ptr<Camera>>(m, "camera")
        .def("set_bounds", &Camera::setBounds)
        .def("set_position", &Camera::setPosition);

    py::class_<OrthoCamera, Camera, std::shared_ptr<OrthoCamera>>(m, "CamOrtho")
        .def(py::init<float, float, const py::kwargs&>());

//    py::class_<PerspectiveCamera, Camera, std::shared_ptr<PerspectiveCamera>>(m, "CamPerspective")
//        .def(py::init<const py::kwargs&>());
//
//	py::class_<Camera25, OrthoCamera, std::shared_ptr<Camera25>>(m, "Cam25")
//		.def(py::init<float, float, const py::kwargs&>());
//

    // -- shapes
    py::module_ ms = m.def_submodule("shapes");

    ms.def("check_los", &checkLOS);
    py::class_<Shape, std::shared_ptr<Shape>>(ms, "Shape");
        //.def(py::init<>());
    py::class_<Segment, Shape, std::shared_ptr<Segment>>(ms, "Segment")
        .def(py::init<float, float, float, float>());

//    py::class_<Point, Shape, std::shared_ptr<Point>>(ms, "Point")
//        .def(py::init<>());

    py::class_<ConvexPoly, Shape, std::shared_ptr<ConvexPoly>>(ms, "ConvexPoly")
        .def(py::init<const std::vector<float>&>());

	py::class_<PolyLine, Shape, std::shared_ptr<PolyLine>>(ms, "PolyLine")
		.def(py::init<const std::vector<float>&>());
    py::class_<Polygon, Shape, std::shared_ptr<Polygon>>(ms, "Polygon")
        .def(py::init<const std::vector<float>&>());

    py::class_<Rect, ConvexPoly, std::shared_ptr<Rect>>(ms, "Rect")
        .def(py::init<float, float, glm::vec2>(), py::arg("width"), py::arg("height"), py::arg("anchor") = glm::vec2(0.f, 0.f));
//
    py::class_<Circle, Shape, std::shared_ptr<Circle>>(ms, "Circle")
        .def(py::init<float, glm::vec2>(), py::arg("radius"), py::arg("center") = glm::vec2(0.f, 0.f));
//
//
    py::class_<AABB, Shape, std::shared_ptr<AABB>>(ms, "AABB")
        .def(py::init<float, float, float, float>());
//
//	py::class_<AABB3D, Shape, std::shared_ptr<AABB3D>>(m, "AABB3D")
//		.def(py::init<float, float, float, float, float, float>());
//
//
//	/// --- models ---
    py::module_ mm = m.def_submodule("models");
//
//	mm.def("make_plane", &ModelMaker::pippo);
	mm.def("from_shape", &ModelMaker::makeModel);
    mm.def("getSprite", &getSpriteModel);
    py::class_<Model, std::shared_ptr<Model>>(mm, "Model")
        .def("set_palette", &Model::setPalette)
        .def("set_color", &Model::setColor);
//	py::class_<TiledModel, Model, std::shared_ptr<TiledModel>>(mm, "itiled");
//        //.def(py::init<>());
    py::class_<IQuads, Model, std::shared_ptr<IQuads>>(mm, "Quad")
        .def(py::init<const std::string&>(), "batch"_a)
        .def(py::init<const std::string&, const std::string&>(), "batch"_a, "desc"_a)
        .def("prova", &IQuads::prova)
        .def("add", &IQuads::addQuad);
    py::class_<Sprite, Model, std::shared_ptr<Sprite>>(mm, "Sprite");
    py::class_<MultiSprite, Model, std::shared_ptr<MultiSprite>>(mm, "MultiSprite")
        .def(py::init<>())
        .def("addSprite", &MultiSprite::addSprite);

////        .def(py::init<const pybind11::kwargs&>());
//    //py::class_<Lines, Model, std::shared_ptr<Lines>>(mm)
//    py::class_<PolyChain, Model, std::shared_ptr<PolyChain>>(mm, "Lines")
//        .def(py::init<const pybind11::kwargs&>());
//	py::class_<ParametricCurve, Model, std::shared_ptr<ParametricCurve>>(mm, "Curve")
//		.def(py::init<const pybind11::kwargs&>());
//    py::class_<StaticTiledModel, TiledModel, std::shared_ptr<StaticTiledModel>>(mm, "Tiled")
//        .def(py::init<const pybind11::kwargs&>());
//	py::class_<AnimatedTiledModel, TiledModel, std::shared_ptr<AnimatedTiledModel>>(mm, "TiledAnim")
//		.def(py::init<const pybind11::kwargs&>());
//	py::class_<RoadModel, Model, std::shared_ptr<RoadModel>>(mm, "Road")
//		.def(py::init<const pybind11::kwargs&>())
//		.def("add_section", &RoadModel::addSection);
//    py::class_<TileMap, Model, std::shared_ptr<TileMap>>(mm, "TileMap")
//        .def(py::init<const pybind11::kwargs&>())
//        .def("add", &TileMap::setTile);
////	py::class_<AnimatedTiledModel, Model, std::shared_ptr<AnimatedTiledModel>>(mm, "tiled_animated")
////		.def(py::init<const pybind11::kwargs&>());
//	py::class_<Sprite, Model, std::shared_ptr<Sprite>>(mm, "Sprite");
//	py::class_<PolyMesh, Model, std::shared_ptr<PolyMesh>>(mm, "PolyMesh");
//		//.def(py::init<const pybind11::kwargs&>());
//	py::class_<Text, Model, std::shared_ptr<Text>>(mm, "Text")
//		.def(py::init<const pybind11::kwargs&>());
//	py::class_<StaticQuad, Model, std::shared_ptr<StaticQuad>>(mm, "Quad")
//		.def(py::init<const pybind11::kwargs&>());
//	py::class_<monkey::skeletal::SkeletalModel, Model, std::shared_ptr<monkey::skeletal::SkeletalModel>>(mm, "SkeletalModel")
//		.def(py::init<const pybind11::kwargs&>());
//
////    py::class_<MultiModel, Model, std::shared_ptr<MultiModel>>(mm, "multi_sprite")
////        //.def(py::init<std::shared_ptr<IBatch>>())
////        .def(py::init<const pybind11::kwargs&>())
////        .def("set_model", &MultiModel::setNodeModel);
//
//
//	/// --- runners ---
	py::class_<Runner, std::shared_ptr<Runner>>(m, "Runner");
	py::class_<ICollisionEngine, Runner, std::shared_ptr<ICollisionEngine>>(m, "icollision");
	py::class_<SpatialHashingCollisionEngine, ICollisionEngine, std::shared_ptr<SpatialHashingCollisionEngine>>(m, "CollisionEngine2D")
		.def(py::init<float, float>(), "width"_a, "height"_a);
                //.def("add_response", &CollisionEngine2D::addResponse);
//    py::class_<CollisionEngine3D, ICollisionEngine, std::shared_ptr<CollisionEngine3D>>(m, "CollisionEngine3D")
//        .def(py::init<float, float, float>(), "width"_a, "height"_a, "depth"_a);
	py::class_<Scheduler, Runner, std::shared_ptr<Scheduler>>(m, "Scheduler")
		.def("add", &Scheduler::add)
		.def(py::init<>());
    py::class_<WalkArea, std::shared_ptr<WalkArea>>(m, "WalkArea")
        .def("addPolyWall", &WalkArea::addPolyWall)
        .def("addLinearWall", &WalkArea::addLineWall)
        //.def("addDynamic", &WalkArea::addDynamic)
        .def("recompute", &WalkArea::recompute)
        .def(py::init<std::vector<float>&, int>());
    py::class_<WalkManager, Runner, std::shared_ptr<WalkManager>>(m, "WalkManager")
        .def(py::init<glm::vec2>())
		.def("recompute", &WalkManager::recomputeWalkareas)
        .def("recomputeBaselines", &WalkManager::recomputeBaselines)
        .def("addWalkArea", &WalkManager::addWalkArea);

    py::class_<Clock, Runner, std::shared_ptr<Clock>>(m, "Clock")
        .def(py::init<>())
        .def("addEvent", &Clock::addEvent)
        .def("removeEvent", &Clock::removeEvent);
    py::class_<MouseManager, Runner, std::shared_ptr<MouseManager>>(m, "MouseManager")
        .def("setFunc", &MouseManager::setDefaultCallback)
        .def("addCamera", &MouseManager::addCamera)
        .def(py::init<>());

//	py::class_<Lighting, Runner, std::shared_ptr<Lighting>>(m, "Lighting")
//		.def(py::init<>())
//		.def("set_ambient", &Lighting::setAmbient)
//		.def("add_light", &Lighting::addLight);
//	py::class_<HotSpotManager, Runner, std::shared_ptr<HotSpotManager>>(m, "HotSpotManager")
//		.def(py::init<>());
//
//	/// --- lights ---
//	py::class_<Light, std::shared_ptr<Light>>(m, "Light");
//	py::class_<DirectionalLight, Light, std::shared_ptr<DirectionalLight>>(m, "light_directional")
//		.def(py::init<const pybind11::kwargs&>());
//
//	/// --- scripts & actions
    py::class_<Script, std::shared_ptr<Script>>(m, "Script")
        .def("add", &Script::add)
        .def("getLastId", &Script::getLastId)
        .def(py::init<const pybind11::kwargs&>());
//
//
//	/// --- actions ---
    py::module_ ma = m.def_submodule("actions");
    py::class_<Action, std::shared_ptr<Action>>(ma, "Action")
        .def("set_on_end", &Action::setOnEnd, "function"_a);

    py::class_<NodeAction, Action, std::shared_ptr<NodeAction>>(ma, "NodeAction");
	py::class_<Move, NodeAction, std::shared_ptr<Move>>(ma, "Move")
		.def(py::init<int, glm::vec2, float>(), "id"_a, "position"_a, "speed"_a);
	py::class_<MoveBy, NodeAction, std::shared_ptr<MoveBy>>(ma, "MoveBy")
		.def(py::init<int, glm::vec2, float, float>(), "id"_a, "delta"_a, "time"_a = 0.f, "speed"_a = 0.f);
    py::class_<MoveAccelerated, NodeAction, std::shared_ptr<MoveAccelerated>>(ma, "MoveAccelerated")
        .def(py::init<int, glm::vec3, glm::vec3, const pybind11::kwargs&>(), "id"_a, "initial_velocity"_a, "acceleration"_a);
	py::class_<Delay, Action, std::shared_ptr<Delay>>(ma, "Delay")
		.def(py::init<float>(), "time"_a);
    py::class_<Animate, NodeAction, std::shared_ptr<Animate>>(ma, "Animate")
        .def(py::init<int, const std::string&, bool, bool>(), "id"_a, "anim"_a, "sync"_a=false, "backwards"_a=false);
//	py::class_<Blink, NodeAction, std::shared_ptr<Blink>>(ma, "Blink")
//		.def(py::init<int, float, float>(), "id"_a, "duration"_a, "period"_a);
	py::class_<CallFunc, Action, std::shared_ptr<CallFunc>>(ma, "CallFunc")
		.def(py::init<pybind11::function>(), "function"_a);
	py::class_<AddNode, NodeAction, std::shared_ptr<AddNode>>(ma, "Add")
		.def(py::init<int, std::shared_ptr<Node>>(), "id"_a, "node"_a);
	py::class_<RemoveNode, NodeAction, std::shared_ptr<RemoveNode>>(ma, "Remove")
		.def(py::init<int>(), "id"_a);
    py::class_<Walk, NodeAction, std::shared_ptr<Walk>>(ma, "Walk")
        .def(py::init<int, glm::vec2>(), "id"_a, "target"_a);
    py::class_<Turn, NodeAction, std::shared_ptr<Turn>>(ma, "Turn")
        .def(py::init<int, const std::string&>(), "id"_a, "dir"_a);
    py::class_<WalkDynamic, NodeAction, std::shared_ptr<WalkDynamic>>(ma, "WalkDynamic")
        .def(py::init<int, pybind11::function>(), "id"_a, "func"_a);
	py::class_<WaitForKey, Action, std::shared_ptr<WaitForKey>>(ma, "WaitForKey")
		.def(py::init<>())
		.def("add", &WaitForKey::add);

//	py::class_<MoveAccelerated, NodeAction, std::shared_ptr<MoveAccelerated>>(ma, "MoveAccelerated")
//		.def(py::init<int, glm::vec3, glm::vec3, float>(), "id"_a, "velocity"_a, "acceleration"_a, "timeout"_a);
//	py::class_<MoveDynamics, NodeAction, std::shared_ptr<MoveDynamics>>(ma, "MoveDynamics")
//		.def(py::init<int, glm::vec3, glm::vec3>(), "id"_a, "velocity"_a, "acceleration"_a);

//	py::class_<actions::Walk, NodeAction, std::shared_ptr<actions::Walk>>(ma, "Walk")
//		.def(py::init<int, glm::vec3>(), "id"_a, "pos"_a);
//	py::class_<Turn, NodeAction, std::shared_ptr<Turn>>(ma, "Turn")
//		.def(py::init<int, const std::string&>(), "id"_a, "direction"_a);
//	py::class_<ShowMessageBase, Action, std::shared_ptr<ShowMessageBase>>(ma, "ShowMsg");
//	py::class_<ShowMessage, ShowMessageBase, std::shared_ptr<ShowMessage>>(ma, "Msg")
//		.def(py::init<const std::string&, const std::string&, const std::string&, glm::vec3, int,
//	   		float, glm::vec2, int, float, int, int, const py::kwargs&>(), "font"_a, "text"_a, "batch"_a, "pos"_a,
//	   		"palette"_a,"timeout"_a=0, "margin"_a=glm::vec2(0.f), "parent"_a=0, "max_width"_a=0,
//	   		"halign"_a=0, "valign"_a=0, py::kw_only());
//	py::class_<Say, ShowMessageBase, std::shared_ptr<Say>>(ma, "say")
//		.def(py::init<const std::string&, const std::string&, const std::string&, glm::vec3, int,
//			 float, glm::vec2, int, float, int, int, const py::kwargs&>(), "font"_a, "text"_a, "batch"_a, "pos"_a,
//			 "palette"_a,"timeout"_a, "margin"_a, "parent"_a, "max_width"_a,
//			 "halign"_a, "valign"_a, py::kw_only());
	py::class_<EnableSierraController, NodeAction, std::shared_ptr<EnableSierraController>>(ma, "SierraEnable")
		.def(py::init<int, bool>(), "id"_a, "value"_a);

	//	py::class_<ChangeSierraAnim, NodeAction, std::shared_ptr<ChangeSierraAnim>>(ma, "SierraChangeAnim")
//		.def(py::init<int, const std::string&, const std::string&>(), "id"_a, "idle"_a, "walk"_a);
//
//
//
//	/// --- components ---
    py::module_ mc = m.def_submodule("components");

	py::class_<Component, std::shared_ptr<Component>>(m, "component")
	    .def_property_readonly("node", &Component::getNode, py::return_value_policy::reference)
	    .def("setState", &Component::setState);

	py::class_<Renderer, Component, std::shared_ptr<Renderer>>(m, "renderer");
	py::class_<BatchRenderer<QuadBatch>, Renderer, std::shared_ptr<BatchRenderer<QuadBatch>>>(m, "batchRendererQuad")
	    .def("setQuadPalette", py::overload_cast<int, const std::string&>(&BatchRenderer<QuadBatch>::setPrimitivePalette))
        .def("setQuadPalette", py::overload_cast<int, int>(&BatchRenderer<QuadBatch>::setPrimitivePalette))
        .def("getQuadPalette", &BatchRenderer<QuadBatch>::getPrimitivePalette);
	py::class_<IQuadsRenderer, BatchRenderer<QuadBatch>, std::shared_ptr<IQuadsRenderer>>(m, "iquadsrenderer");



//
//	py::class_<HotSpot, Component, std::shared_ptr<HotSpot>>(m, "_hotspot");
//
//	py::class_<PyHotSpot, HotSpot, std::shared_ptr<PyHotSpot>>(m, "Hotspot")
//		.def(py::init<std::shared_ptr<Shape>, const pybind11::kwargs&>())
//		.def("set_on_enter", &PyHotSpot::setOnEnter)
//		.def("set_on_leave", &PyHotSpot::setOnLeave)
//		.def("set_on_click", &PyHotSpot::setOnClick);
//
//	py::class_<TextHotSpot, PyHotSpot, std::shared_ptr<TextHotSpot>>(m, "TextHotspot")
//		.def(py::init<pybind11::kwargs&>());
//
//
	py::class_<Mover, Component, std::shared_ptr<Mover>>(mc, "Mover")
		.def(py::init<const pybind11::kwargs&>())
		.def("add", &Mover::addMove);



	py::class_<Collider, Component, std::shared_ptr<Collider>>(mc, "icollider")
		.def_property_readonly("bounds", &Collider::bounds)
		.def("setMask", &Collider::setCollisionMask)
		.def_property("flag", &Collider::getCollisionFlag, &Collider::setCollisionFlag)
		.def("setResponse", &Collider::setResponse)
		.def("set_collision_flag", &Collider::setCollisionFlag);


	py::class_<SimpleCollider, Collider, std::shared_ptr<SimpleCollider>>(mc, "Collider")
		.def(py::init<int, int, int, std::shared_ptr<Shape>, const pybind11::kwargs&>(), py::arg("flag"),
        py::arg("mask"), py::arg("tag"),  py::arg("shape"),py::kw_only());

	py::class_<SpriteCollider, Collider, std::shared_ptr<SpriteCollider>>(mc, "SpriteCollider")
            //.def("set_override", &SpriteCollider::setCollisionOverride)
		.def(py::init<int, int, int, const pybind11::kwargs&>(), py::arg("flag"), py::arg("mask"), py::arg("tag"), py::kw_only());

//	py::class_<Switch, Component, std::shared_ptr<Switch>>(mc, "Switch")
//	    .def(py::init<>())
//	    .def("add", &Switch::add)
//	    .def("enable", &Switch::enable);
	//
//    py::class_<monkey::skeletal::SkeletalCollider, Collider, std::shared_ptr<monkey::skeletal::SkeletalCollider>>(m, "SkeletalCollider")
//        .def(py::init<const pybind11::kwargs&>());
//
	py::class_<Controller, Component, std::shared_ptr<Controller>>(mc, "Controller")
		.def_property_readonly("grounded", &Controller::grounded)
		.def_property_readonly("size", &Controller::getSize)
		.def("set_size", &Controller::setSize);

	py::class_<MouseArea, Component, std::shared_ptr<MouseArea>>(mc, "MouseArea")
		.def(py::init<std::shared_ptr<Shape>, int, int, const pybind11::kwargs&>())
		.def("setShape", &MouseArea::setShape);

    py::class_<VehicleController2D, Component, std::shared_ptr<VehicleController2D>>(mc, "VehicleController")
        .def(py::init<float, float, float, float, glm::vec2, float, float, float, float, py::kwargs&>());

	py::class_<Controller2D, Controller, std::shared_ptr<Controller2D>>(mc, "Controller2D")
		.def(py::init<py::kwargs&>());

	py::class_<TileController, Component, std::shared_ptr<TileController>>(mc, "TileController")
		.def(py::init<float, float, float, py::kwargs&>());

	//	py::class_<MarioController, Controller, std::shared_ptr<MarioController>>(m, "MarioController")
//		.def(py::init<py::kwargs&>());
    py::class_<Sierra2DController, Component, std::shared_ptr<Sierra2DController>>(mc, "SierraController")
        .def_property_readonly("direction", &Sierra2DController::getDirection)
        .def(py::init<py::kwargs&>());
	py::class_<PlayerSierra2DController, Sierra2DController, std::shared_ptr<PlayerSierra2DController>>(mc, "PlayerSierraController")
		.def(py::init<py::kwargs&>());

    py::class_<NPCSierraController, Sierra2DController, std::shared_ptr<NPCSierraController>>(mc, "NPCSierraController")
        .def(py::init<int, float, float, float, py::kwargs&>());
    py::class_<WalkableCharacter, Sierra2DController, std::shared_ptr<WalkableCharacter>>(mc, "WalkableCharacter")
        .def(py::init<float, py::kwargs&>())
        .def("setCallback", &WalkableCharacter::setCallback)
        .def("goto", &WalkableCharacter::goTo);
    py::class_<NPCSierraFollow, WalkableCharacter, std::shared_ptr<NPCSierraFollow>>(mc, "NPCSierraFollow")
        .def(py::init<pybind11::function, float, float, py::kwargs&>())
        .def("setFunc", &NPCSierraFollow::setFunction);

//	py::class_<Walk2D, Component, std::shared_ptr<Walk2D>>(mc, "walk2D")
//        .def_property("direction", &Walk2D::getDirection, &Walk2D::setDirection)
//        .def_property_readonly("grounded", &Walk2D::grounded)
//        .def_property("speed", &Walk2D::getSpeed, &Walk2D::setSpeed)
//
//        .def("jump", &Walk2D::jump);
//	py::class_<PlayerWalk2D, Walk2D, std::shared_ptr<PlayerWalk2D>>(mc, "PlayerWalk2D")
//		.def(py::init<float, float, float, float, const pybind11::kwargs&>(), "max_speed"_a,
//        "acceleration"_a, "jump_height"_a, "time_to_jump_apex"_a, py::kw_only());
//    py::class_<CustomWalk2D, Walk2D, std::shared_ptr<CustomWalk2D>>(mc, "CustomWalk2D")
//        .def(py::init<float, float, float, float, const pybind11::object, const pybind11::kwargs&>(), "max_speed"_a,
//             "acceleration"_a, "jump_height"_a, "time_to_jump_apex"_a, "internal"_a,py::kw_only())
//        .def("isPressed", &CustomWalk2D::isPressed);
//
//    py::class_<FoeWalk2D, Walk2D, std::shared_ptr<FoeWalk2D>>(mc, "FoeWalk2D")
//        .def(py::init<float, float, float, float, const pybind11::kwargs&>(), "max_speed"_a,
//        "acceleration"_a, "jump_height"_a, "time_to_jump_apex"_a, py::kw_only());

	py::class_<Baseline, Component, std::shared_ptr<Baseline>>(mc, "Baseline")
		.def(py::init<std::shared_ptr<PolyLine>, const pybind11::kwargs&>());

	//    py::class_<Walk3DController, Component, std::shared_ptr<Walk3DController>>(m, "Walk3DController")
//        .def(py::init<float, float, float, const pybind11::kwargs&>(), "size"_a, "speed"_a, "gravity"_a);
//
////	py::class_<Controller3D, Controller, std::shared_ptr<Controller3D>>(m, "controller_3d")
////		.def(py::init<py::kwargs&>());
//
//	py::class_<Dynamics, Component, std::shared_ptr<Dynamics>>(m, "Dynamics")
//		.def("set_velocity", &Dynamics::setVelocity)
//		.def(py::init<const pybind11::kwargs&>());
//
	py::class_<Follow, Component, std::shared_ptr<Follow>>(mc, "Follow")
		.def(py::init<int, const pybind11::kwargs&>(), py::arg("camId"), py::kw_only());

	py::class_<Parallax, Component, std::shared_ptr<Parallax>>(mc, "Parallax")
		.def(py::init<int, float, glm::vec2, glm::vec2, const pybind11::kwargs&>(),
		        py::arg("camId"), py::arg("z"), py::arg("base"), py::arg("factor"),py::kw_only());
	py::class_<NodeEvent, Component, std::shared_ptr<NodeEvent>>(mc, "NodeEvent")
		.def(py::init<float, float, const pybind11::kwargs&>(), py::arg("width"), py::arg("height"), py::kw_only())
		.def("addEventX", &NodeEvent::addEventX)
		.def("addEventY", &NodeEvent::addEventY);

	//
	py::class_<Platform, Component, std::shared_ptr<Platform>>(mc, "Platform")
		.def(py::init<const pybind11::kwargs&>());
//
//	py::class_<StateMachine, Component, std::shared_ptr<StateMachine>>(m, "StateMachine")
//		.def("add", &StateMachine::addState)
//		.def("set_initial_state", &StateMachine::setInitialState)
//		.def(py::init<const pybind11::kwargs&>());
//
//	py::class_<MoveTranslate, Component, std::shared_ptr<MoveTranslate>>(m, "MoveTranslate")
//		.def(py::init<const pybind11::kwargs&>());
//
	py::class_<Keyboard, Component, std::shared_ptr<Keyboard>>(mc, "Keyboard")
		.def("add", &Keyboard::addFunction)
		.def("add_fallback", &Keyboard::addFallbackFunction)
		.def(py::init<const pybind11::kwargs&>());
//
//	py::class_<ScriptPlayer, Component, std::shared_ptr<ScriptPlayer>>(m, "script_player")
//		.def("play", &ScriptPlayer::play)
//		.def(py::init<>());
//
//	py::class_<ScummCharacter, Component, std::shared_ptr<ScummCharacter>>(m, "scumm_char")
//		.def(py::init<const pybind11::kwargs&>());
//
//	py::class_<Cursor, Component, std::shared_ptr<Cursor>>(m, "Cursor")
//		.def(py::init<const pybind11::kwargs&>());
//
//
//
//	/// --- states ---
//	py::class_<State, std::shared_ptr<State>>(m, "State");
//	py::class_<Top2D, State, std::shared_ptr<Top2D>>(m, "top_2d")
//		.def(py::init<const pybind11::kwargs&>());
//
//	py::class_<Walk2D, State, std::shared_ptr<Walk2D>>(m, "Walk2D");
//	py::class_<Walk3D, State, std::shared_ptr<Walk3D>>(m, "Walk3D");
//	py::class_<PlayerWalk2D, State, std::shared_ptr<PlayerWalk2D>>(m, "walk_2d_player")
//		.def(py::init<const pybind11::kwargs&>());
//	py::class_<PlayerWalk3D, State, std::shared_ptr<PlayerWalk3D>>(m, "walk_3d_player")
//		.def(py::init<const pybind11::kwargs&>());
//	py::class_<FoeWalk2D, State, std::shared_ptr<FoeWalk2D>>(m, "walk_2d_foe")
//		.def(py::init<const pybind11::kwargs&>());
//	py::class_<Climb, State, std::shared_ptr<Climb>>(m, "climb")
//		.def(py::init<>());
//	py::class_<Idle, State, std::shared_ptr<Idle>>(m, "idle")
//		.def(py::init<>());
//	py::class_<Bounce, State, std::shared_ptr<Bounce>>(m, "bounce")
//		.def(py::init<>());
//    py::class_<Attack, State, std::shared_ptr<Attack>>(m, "attack")
//            .def(py::init<>());
//
//	/// --- other ---
//	py::class_<FuncXY, std::shared_ptr<FuncXY>>(m, "funcxy");
//	py::class_<FuncConst, FuncXY, std::shared_ptr<FuncConst>>(m, "func_const")
//		.def(py::init<const pybind11::kwargs&>());
//	py::class_<PiecewiseLinearFunc, FuncXY, std::shared_ptr<PiecewiseLinearFunc>>(m, "func_pl")
//		.def(py::init<const pybind11::kwargs&>());
//	py::class_<GridFunction, FuncXY, std::shared_ptr<GridFunction>>(m, "func_grid")
//		.def(py::init<const pybind11::kwargs&>());

}

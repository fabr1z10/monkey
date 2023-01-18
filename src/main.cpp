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

    py::module_ mm = m.def_submodule("models");
    py::class_<Model, std::shared_ptr<Model>>(mm, "Model")
        .def(py::init<int>());
    py::class_<TiledModel, Model, std::shared_ptr<TiledModel>>(mm, "tiled")
        .def(py::init<const pybind11::kwargs&>());


}

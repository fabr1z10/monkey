#include "road.h"
#include "../pyhelper.h"
#include <iostream>

extern GLFWwindow * window;

RoadModel::RoadModel(const pybind11::kwargs &args) : Model(), _z(0) {
	_dz = py_get_dict<float>(args, "dz", 2.0f);

	_colorScheme[0].roadColor = py_get_dict<glm::vec4>(args, "road_color_0", glm::vec4(156, 156, 156, 255) / 255.0f);
	_colorScheme[0].terrainColor = py_get_dict<glm::vec4>(args, "terrain_color_0", glm::vec4(230, 214, 197, 255) / 255.0f);


	_colorScheme[1].roadColor = py_get_dict<glm::vec4>(args, "road_color_1", glm::vec4(148, 148, 148, 255) / 255.0f);
	_colorScheme[1].terrainColor = py_get_dict<glm::vec4>(args, "terrain_color_1", glm::vec4(239, 222, 208, 255) / 255.0f);


	_nTrianglesPerSegment = 6;

	_dz = py_get_dict<float>(args, "seg_depth", 1.f);

	//float _z;					// current player position
	_n = py_get_dict<int>(args, "segments", 200);
	_ns = py_get_dict<int>(args, "color_switch", 1);

}

void RoadModel::addSection(long length, float width, float curve, float slope) {
	_sections.push_back({_nextStart, _nextStart + length - 1, width, curve, slope});
	_nextStart += length;
}


std::shared_ptr<Renderer> RoadModel::getRenderer(const pybind11::kwargs& args) {
	return std::make_shared<RoadRenderer>(args);

}

int RoadModel::getTotalTrianglCount() {
	return _nTrianglesPerSegment * _n;
}

void RoadRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs &) {
	_roadModel = std::dynamic_pointer_cast<RoadModel>(model).get();
	auto qc = _roadModel->getTotalTrianglCount();
	for (int i = 0; i < qc; ++i) {
		_primitiveIds.push_back(_batch->getPrimitiveId());
	}
}

RoadRenderer::RoadRenderer(const pybind11::kwargs& args) : BatchRenderer<TriangleBatch>(args) {
	//_batch = dynamic_cast<QuadBatch*>(batch);
	assert(_batch != nullptr);
	_s = 0;
	_x = 0;

}

void RoadRenderer::update(double) {
	// first of all, you need to know the current player coordinate
	// then you need
	float ds{0.f};
	if (glfwGetKey(window, GLFW_KEY_UP)==GLFW_PRESS) {
	    ds = 0.05f;

	}
	if (glfwGetKey(window, GLFW_KEY_LEFT)==GLFW_PRESS) {
		_x -= 0.05;
	} else if (glfwGetKey(window, GLFW_KEY_RIGHT)==GLFW_PRESS) {
		_x += 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)==GLFW_PRESS) {
		//_s += +0.05;

	}
    _s += ds;
	//float s=0;
	int index = _s / _roadModel->getSegmentLength();
	// find start section
	int isec=0;
	if (_roadModel->_sections.empty()) {
		return;
	}
	while (_roadModel->_sections[isec].endIndex < index) isec++;
	// so sections[i] now is the first section to draw
	float z =  (_s - index * _roadModel->getSegmentLength());
	int ti =0;
	float y = 0.f;
	const auto* section = &_roadModel->_sections[isec];
	_x = 0.f;
	float dz = _roadModel->getSegmentLength();
//
	float rx= 0.f, ry=0.f;
    float dx= 0.0f, dy = 0.0f;

    std::cout << _s << ", " << index << ", " << z << "," << isec << "\n";

    z=0.f;
    for (size_t i = 0; i < _roadModel->getSegmentsToDraw(); ++i) {
	    if (index > section->endIndex) {
	        // next section
	        isec++;
	        if (isec >= _roadModel->_sections.size()) {
	            break;
	        }
	        section = &_roadModel->_sections[isec];
	    }

	    auto length = (i == 0 ? (index + 1.f) * dz - _s : dz);
		// two triangles for left terrain
		float z1 = z - length;
		int colorSchemeIndex = (index / _roadModel->getSegmentsPerColorSchemeChange()) % 2;
		const auto& cs = _roadModel->getColorScheme(colorSchemeIndex);
        dx += length * section->curve;
        dy += length * section->slope;
        float rleft = rx - section->width;
        float rright = rx + section->width;
		_batch->setTriangle(ti++, glm::vec3(-_x -100.f, y, z), glm::vec3(-_x + rleft, y, z), glm::vec3(-_x + rleft + dx, y, z1), cs.terrainColor);
		_batch->setTriangle(ti++, glm::vec3(-_x -100.f, y, z), glm::vec3(-_x + rleft + dx, y, z1), glm::vec3(-_x-100.f, y, z1), cs.terrainColor);
		// two triangles for road
		_batch->setTriangle(ti++, glm::vec3(-_x+rleft, y, z), glm::vec3(-_x+rright, y, z), glm::vec3(-_x+rright+dx, y, z1), cs.roadColor);
		_batch->setTriangle(ti++, glm::vec3(-_x+rleft, y, z), glm::vec3(-_x+rleft+dx, y, z1), glm::vec3(-_x+rright+dx, y, z1), cs.roadColor);
		// two tri for right terrain
		_batch->setTriangle(ti++, glm::vec3(-_x+rright, y, z), glm::vec3(-_x+100.f, y, z), glm::vec3(-_x+100.f, y, z1), cs.terrainColor);
		_batch->setTriangle(ti++, glm::vec3(-_x+rright, y, z), glm::vec3(-_x+rright+dx, y, z1), glm::vec3(-_x+100.f, y, z1), cs.terrainColor);
        //rx += dx;
        //ry += dy;
        //rmid += dx;

		// lines
		// this stuff should be done by model maybe?
        rx += dx;
		z = z1;
		index++;
	}
}
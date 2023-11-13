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
	if (glfwGetKey(window, GLFW_KEY_UP)==GLFW_PRESS) {
		_s += 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT)==GLFW_PRESS) {
		_x -= 0.05;
	} else if (glfwGetKey(window, GLFW_KEY_RIGHT)==GLFW_PRESS) {
		_x += 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)==GLFW_PRESS) {
		//_s += +0.05;

	}
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
	const auto& section = _roadModel->_sections[isec];
	float dz = _roadModel->getSegmentLength();
	std::cout << _s << ", " << index << ", " << z << "\n";
	for (size_t i = 0; i < _roadModel->getSegmentsToDraw(); ++i) {
		// two triangles for left terrain
		int colorSchemeIndex = (index / _roadModel->getSegmentsPerColorSchemeChange()) % 2;
		const auto& cs = _roadModel->getColorScheme(colorSchemeIndex);

		_batch->setTriangle(ti++, glm::vec3(-_x -100.f, y, z), glm::vec3(-_x -section.width, y, z), glm::vec3(-_x-section.width, y, z-dz), cs.terrainColor);
		_batch->setTriangle(ti++, glm::vec3(-_x -100.f, y, z), glm::vec3(-_x -section.width, y, z-dz), glm::vec3(-_x-100.f, y, z-dz), cs.terrainColor);
		// two triangles for road
		_batch->setTriangle(ti++, glm::vec3(-_x-section.width, y, z), glm::vec3(-_x+section.width, y, z), glm::vec3(-_x+section.width, y, z-dz), cs.roadColor);
		_batch->setTriangle(ti++, glm::vec3(-_x-section.width, y, z), glm::vec3(-_x-section.width, y, z-dz), glm::vec3(-_x+section.width, y, z-dz), cs.roadColor);
		// two tri for right terrain
		_batch->setTriangle(ti++, glm::vec3(-_x+section.width, y, z), glm::vec3(-_x+100.f, y, z), glm::vec3(-_x+100.f, y, z-dz), cs.terrainColor);
		_batch->setTriangle(ti++, glm::vec3(-_x+section.width, y, z), glm::vec3(-_x+section.width, y, z-dz), glm::vec3(-_x+100.f, y, z-dz), cs.terrainColor);
		// lines
		// this stuff should be done by model maybe?
		z -= dz;
		index++;
	}
}
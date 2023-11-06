#include "road.h"
#include "../pyhelper.h"

Road::Road(const pybind11::kwargs &args) : _z(0) {
	_dz = py_get_dict<float>(args, "dz", 2.0f);
	_roadColor0 = py_get_dict<glm::vec4>(args, "road_color_0", glm::vec4(156, 156, 156, 255) / 255.0f);
	_roadColor1 = py_get_dict<glm::vec4>(args, "road_color_1", glm::vec4(148, 148, 148, 255) / 255.0f);

}




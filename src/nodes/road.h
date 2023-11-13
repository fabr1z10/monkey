#pragma once

#include "../node.h"
#include "../batch/trianglebatch.h"
#include <glm/glm.hpp>
#include <pybind11/pybind11.h>
#include "../components/batchrenderer.h"

struct RoadSection {
	long startIndex;
	long endIndex;
	float width;
	float curve;
	float slope;
};

class RoadModel : public Model {


public:
	struct ColorScheme {
		glm::vec4 terrainColor;
		glm::vec4 roadColor;
	};

	explicit RoadModel(const pybind11::kwargs& args);
	int getTotalTrianglCount();
	float getSegmentLength() const;
	int getSegmentsToDraw() const;
	int getSegmentsPerColorSchemeChange() const;
	std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
	void addSection(long length, float width, float curve, float slope);
	std::vector<RoadSection> _sections;
	const ColorScheme& getColorScheme(int) const;
private:
	std::array<ColorScheme, 2> _colorScheme;
	glm::vec4 _roadColor0;
	glm::vec4 _roadColor1;
	glm::vec4 _terrainColor0;
	glm::vec4 _terrainColor1;
	glm::vec4 _stripeColor;
	int _nTrianglesPerSegment;		// number of vertices per road segment
	float _dz;					// z distance of road segment
	float _z;					// current player position
	int _n;						// number of segments to draw
	int _ns;					// number of segments per color switch
	long _nextStart;
};

inline const RoadModel::ColorScheme& RoadModel::getColorScheme(int index) const {
	return _colorScheme[index];
}

inline float RoadModel::getSegmentLength() const {
	return _dz;
}

inline int RoadModel::getSegmentsToDraw() const {
	return _n;
}

inline int RoadModel::getSegmentsPerColorSchemeChange() const {
	return _ns;
}

class RoadRenderer : public BatchRenderer<TriangleBatch> {
public:
	explicit RoadRenderer(const pybind11::kwargs& args);

	void setModel(std::shared_ptr<Model>, const pybind11::kwargs&) override;
	void update(double) override;

private:
	RoadModel* _roadModel;
	float _s;
	float _x;
};



//class RoadRenderer : public BatchRenderer<
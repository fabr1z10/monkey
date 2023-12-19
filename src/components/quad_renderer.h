#pragma once

#include "../models/sprite.h"
#include "batchrenderer.h"
#include "../batch/quadbatch.h"


class StaticQuadRenderer : public BatchRenderer<QuadBatch> {
public:
	explicit StaticQuadRenderer(const pybind11::kwargs& args);
	void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
	std::type_index getType() override;
	void start() override;

	// nothing to do in update! (no animation)
	void update(double) override ;
private:
	unsigned _camId;
	StaticQuad* _model;
	float _fade;
};
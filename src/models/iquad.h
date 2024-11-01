#pragma once

#include "../model.h"
#include <pybind11/pybind11.h>
#include <iostream>
#include "../batch/quadbatch.h"
#include "../components/batchrenderer.h"


// describe a single quad
struct QuadInfo {
	QuadInfo() : location(), size(0.f), repeat(1.f), textureCoordinates(0.f),
				 flipv(false), fliph(false), palette(0), advance(0.f) {}

	// the position w.r.t the node position
	glm::vec3 location;

    //glm::vec2 anchorPoint;
    glm::vec2 size;

    glm::vec2 repeat;

    glm::vec4 textureCoordinates;

    int palette;

    bool flipv;

    bool fliph;

    float advance;
};



//class SpriteSheet;

// model that renders one or more quads using a batch
class IQuads : public Model {
public:
    IQuads(const std::string& batchId);
    IQuads(const std::string& batchId, const std::string& desc);
    virtual ~IQuads() {}
    void addQuad(glm::vec4 texCoords, const pybind11::kwargs& kwargs);
    void prova(glm::vec3 ciao) {
        std::cout << ciao.x << "\n";
    }
    // returns information for rendering a frame (used by the renderer - which is responsibile
    // for keeping track of current animation and frame

    //virtual void initFromPy(const pybind11::kwargs&) = 0;
    //virtual void initFromYAML(const YAML::Node &node, const YAML::Node &globals) = 0;
    std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
    int getQuadCount() const;
	const std::vector<QuadInfo>& getQuads() const;
	std::vector<QuadInfo>& getQuads();
	const QuadInfo& getQuadInfo(int) const;
	void setPalette(int) override;
protected:
    std::string ltrim(const std::string &s);

    //int _quadCount; // the number of quads to book -> this is the # pf quads of the frame with the highest n of quads
    std::string _batchId;
	SpriteSheet* _sheet;
	std::vector<QuadInfo> _quads;
	float _texWidth;
	float _texHeight;
};

inline const QuadInfo & IQuads::getQuadInfo(int i) const {
	return _quads[i];
}

inline const std::vector<QuadInfo> & IQuads::getQuads() const {
	return _quads;
}
inline std::vector<QuadInfo> & IQuads::getQuads() {
	return _quads;
}

inline int IQuads::getQuadCount() const {
    return _quads.size();
}



class IQuadsRenderer : public BatchRenderer<QuadBatch> {
public:
	explicit IQuadsRenderer(const std::string& batchId);
	void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
	std::type_index getType() override;
	//void start() override;

	// nothing to do in update! (no animation)
	void update(double) override ;
private:
	unsigned _camId;
	IQuads* _model;
	float _fade;

};

inline std::type_index IQuadsRenderer::getType() {
	return std::type_index(typeid(Renderer));
}




//class StaticQuad : public Model {
//public:
//	StaticQuad(const pybind11::kwargs&);
//	glm::vec2 getAnchorPoint() const;
//	glm::vec2 getSize() const;
//	glm::vec4 getTexCoords() const;
//	glm::vec2 getRepeat() const;
//	std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
//
//private:
//	glm::vec2 _anchorPoint;
//	glm::vec2 _size;
//	glm::vec4 _textureCoordinates;
//	glm::vec2 _repeat;
//
//};


//inline glm::vec2 StaticQuad::getAnchorPoint() const {
//	return _anchorPoint;
//}
//inline glm::vec2 StaticQuad::getSize() const {
//	return _size;
//}
//
//inline glm::vec4 StaticQuad::getTexCoords() const {
//	return _textureCoordinates;
//}
//
//inline glm::vec2 StaticQuad::getRepeat() const {
//	return _repeat;
//}
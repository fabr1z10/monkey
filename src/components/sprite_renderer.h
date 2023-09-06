#pragma once

#include "../models/sprite.h"
#include "batchrenderer.h"
#include "../batch/quadbatch.h"


//class StaticQuadRenderer : public BatchRenderer<QuadBatch> {
//public:
//	explicit StaticQuadRenderer(const pybind11::kwargs& args);
//	void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
//	std::type_index getType() override;
//	void start() override;
//
//	// nothing to do in update! (no animation)
//	void update(double) override {}
//private:
//	unsigned _camId;
//	std::shared_ptr<StaticQuad> _model;
//};

class SpriteRenderer : public BatchRenderer<QuadBatch> {
public:
	explicit SpriteRenderer(const pybind11::kwargs& args);
	void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
	std::type_index getType() override;
	void start() override;
	[[nodiscard]] const std::string& getAnimation() const;
	void setAnimation(const std::string&) override;
	void setAnimationForce(const std::string&);
    IQuad* getSprite();
	[[nodiscard]] int getFrame() const;
	//void draw(Shader *) override;
	bool isComplete() const;
    void update(double) override;
    bool updateTick(int);
    void updateBatch();
    inline int getTickCount() const;
	const IQuad* getSprite() const;
private:
	//QuadBatch* _spriteBatch;

    unsigned _camId;
	//void innerDraw(Shader*, const glm::mat4&) override;
	const Animation* m_animInfo;
	std::string m_animation;
	int m_frame;
	std::shared_ptr<IQuad> m_sprite;
	int m_ticks;
	bool m_complete;

};

inline const IQuad * SpriteRenderer::getSprite() const {
	return m_sprite.get();
}

inline int SpriteRenderer::getTickCount() const {
    return m_ticks;
}

inline IQuad* SpriteRenderer::getSprite() {
	return m_sprite.get();
}

inline int SpriteRenderer::getFrame() const {
	return m_frame;
}

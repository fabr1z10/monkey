#pragma once

#include "../models/sprite.h"
#include "batchrenderer.h"
#include "../batch/quadbatch.h"

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

inline int SpriteRenderer::getTickCount() const {
    return m_ticks;
}

inline IQuad* SpriteRenderer::getSprite() {
	return m_sprite.get();
}

inline int SpriteRenderer::getFrame() const {
	return m_frame;
}

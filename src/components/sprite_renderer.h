#pragma once

#include "../models/sprite.h"
#include "renderer.h"
#include "../batch/quadbatch.h"

class SpriteRenderer : public Renderer {
public:
	explicit SpriteRenderer(IBatch*);
	virtual ~SpriteRenderer();
	void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
	std::type_index getType() override;
	void start() override;
	[[nodiscard]] const std::string& getAnimation() const;
	void setAnimation(const std::string&) override;
    IQuad* getSprite();
	[[nodiscard]] int getFrame() const;
	//void draw(Shader *) override;
	bool isComplete() const;
    void update(double) override;

private:
	QuadBatch* _spriteBatch;
    std::vector<int> _quadIds;
    int _paletteId;

	//void innerDraw(Shader*, const glm::mat4&) override;
	const Animation* m_animInfo;
	std::string m_animation;
	int m_frame;
	std::shared_ptr<IQuad> m_sprite;
	int m_ticks;
	bool m_complete;

};

inline IQuad* SpriteRenderer::getSprite() {
	return m_sprite.get();
}

inline int SpriteRenderer::getFrame() const {
	return m_frame;
}

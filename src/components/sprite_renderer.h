#pragma once

#include "../models/sprite.h"
#include "renderer.h"
#include "../batch/quadbatch.h"

class SpriteRenderer : public Renderer {
public:
	explicit SpriteRenderer(QuadBatch* , const std::string& anim);
	virtual ~SpriteRenderer();
	void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
	std::type_index getType() override;
	void start() override;
	[[nodiscard]] const std::string& getAnimation() const;
	void setAnimation(const std::string&) ;
	Sprite* getSprite();
	[[nodiscard]] int getFrame() const;
	//void draw(Shader *) override;
	bool isComplete() const;
    void update(double) override;

private:
	QuadBatch* _spriteBatch;
    int _quadId;
    int _paletteId;

	//void innerDraw(Shader*, const glm::mat4&) override;
	const AnimInfo* m_animInfo;
	std::string m_animation;
	int m_frame;
	std::shared_ptr<Sprite> m_sprite;
	int m_ticks;
	bool m_complete;

};

inline Sprite* SpriteRenderer::getSprite() {
	return m_sprite.get();
}

inline int SpriteRenderer::getFrame() const {
	return m_frame;
}

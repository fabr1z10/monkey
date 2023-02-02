#include "anim_tiled_renderer.h"

std::type_index AnimatedTiledModelRenderer::getType() {
	return std::type_index(typeid(Renderer));
}


void AnimatedTiledModelRenderer::start() {
	m_ticks = 0;
	m_frame = 0;
}

void AnimatedTiledModelRenderer::draw(Shader * s) {

	const auto& a = m_tiled->getFrameInfo(m_frame);
	if (m_paletteId != GL_INVALID_VALUE) {
		s->setInt("texture_palette", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_1D, m_paletteId);
		if (m_texId != GL_INVALID_VALUE) {
			s->setInt("texture_pdiffuse1", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texId);
		}
	} else {
		if (m_texId != GL_INVALID_VALUE) {
			s->setInt("texture_diffuse1", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texId);
		}
	}
	m_model->draw(s, a.offset, a.count);
	if (m_ticks >= a.ticks) {
		// increment frame. if this animation is
		m_frame++;
		if (m_frame >= m_tiled->getFrameCount()) {
			m_frame = 0;
		}
		m_ticks = 0;
	} else {
		// if it's not time to update frame, increment current frame length
		m_ticks++;
	}

}


void AnimatedTiledModelRenderer::setModel(std::shared_ptr<Model> model) {
	Renderer::setModel(model);
	m_tiled = std::dynamic_pointer_cast<AnimatedTiledModel>(model);
}
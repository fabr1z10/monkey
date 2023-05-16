#include "sprite_renderer.h"
#include "../error.h"
#include "../node.h"
#include "../pyhelper.h"

SpriteRenderer::SpriteRenderer(QuadBatch* batch, const std::string& anim) : Renderer(),
    _spriteBatch(batch), m_animation(anim), m_frame(0), m_ticks(0) {

    // request a new quad id to the batch
    _quadId = _spriteBatch->getPrimitiveId();
}

SpriteRenderer::~SpriteRenderer() {
	_spriteBatch->releaseQuad(_quadId);
}

void SpriteRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
	Renderer::setModel(model, args);
	_paletteId = py_get_dict<int>(args, "pal", 0);
	m_sprite = std::dynamic_pointer_cast<Sprite>(model);
}

const std::string & SpriteRenderer::getAnimation() const {
	return m_animation;
}

void SpriteRenderer::setAnimation(const std::string& anim) {
	if (anim == m_animation) {
		return;
	}
	m_complete = false;
	m_animInfo = m_sprite->getAnimInfo(anim);
	if (m_animInfo == nullptr) {
		GLIB_FAIL("mmh don't know animation: " + anim);
	}
	m_frame = 0;
	m_animation = anim;
}

void SpriteRenderer::start() {
	m_animInfo = m_sprite->getAnimInfo(m_animation);
}

void SpriteRenderer::update(double dt) {
	const auto& a = m_sprite->getFrameInfo(m_animation, m_frame);
	// get world pos
	auto pos = m_node->getWorldPosition();

	// the bottom left corner depends whether entity is flipped horizontally



	//auto worldTransform = m_node->getWorldMatrix();
	//glm::vec3 pos = worldTransform * glm::vec4(a.anchor_point, 0.f, 1.f);
	auto flipx = m_node->getFlipX() ^ a.flipx;

	glm::vec2 delta = flipx ? (glm::vec2(a.size.x, 0.f) - a.anchor_point) : a.anchor_point;
    auto bottomLeft = pos - glm::vec3(delta, 0.f);

    _spriteBatch->setQuad(_quadId,
						 bottomLeft,
						 a.size,
						 a.texture_coordinates,
						glm::vec2(1, 1),
						 _paletteId,
						 flipx,
						a.flipy);
	//_spriteBatch->setQuad(_quadId, bottomLeft, a.size, a.texture_coordinates, glm::vec2(1, 1), a.paletteIndex, flipx, false);



    if (m_ticks >= a.ticks) {
        // increment frame. if this animation is
        m_frame++;
        if (m_frame >= m_animInfo->frameCount) {
            m_frame = (m_animInfo->loop ? m_animInfo->loopFrame : m_animInfo->frameCount - 1);
            m_complete = true;
        }
        m_ticks = 0;
    } else {
        // if it's not time to update frame, increment current frame length
        m_ticks++;
    }





//	if (m_paletteId != GL_INVALID_VALUE) {
//		s->setInt("texture_palette", 1);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_1D, m_paletteId);
//		if (m_texId != GL_INVALID_VALUE) {
//			s->setInt("texture_pdiffuse1", 0);
//			glActiveTexture(GL_TEXTURE0);
//			glBindTexture(GL_TEXTURE_2D, m_texId);
//		}
//	} else {
//		if (m_texId != GL_INVALID_VALUE) {
//			s->setInt("texture_diffuse1", 0);
//			glActiveTexture(GL_TEXTURE0);
//			glBindTexture(GL_TEXTURE_2D, m_texId);
//		}
//	}

//	m_model->draw(s, a.offset, a.count);
//	m_sprite->innerDraw(s, modelMatrix, ss.str());
//	//m_sprite->draw(s, nullptr);
	// time to update frame?

}

std::type_index SpriteRenderer::getType() {
	return std::type_index(typeid(Renderer));
}

bool SpriteRenderer::isComplete() const {
	return m_complete;
}
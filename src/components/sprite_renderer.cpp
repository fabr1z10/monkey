#include "sprite_renderer.h"
#include "../error.h"
#include "../node.h"
#include "../pyhelper.h"
#include "../engine.h"
#include <iostream>



SpriteRenderer::SpriteRenderer(const std::string& batchId) : BatchRenderer<QuadBatch>(batchId,pybind11::kwargs()), m_frame(0), m_ticks(0), _currentFrameTicks(0),
_direction(1) {
    //_batch = dynamic_cast<QuadBatch*>(batch);
    //assert(_batch != nullptr);
    //_paletteId = py_get_dict<unsigned>(args, "pal", 0);
    //_camId = py_get_dict<unsigned>(args, "cam", 0);

    // request a new quad id to the batch
    //_quadId = _spriteBatch->getPrimitiveId();


}



void SpriteRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
	Renderer::setModel(model, args);
    m_sprite = std::dynamic_pointer_cast<Sprite>(model);
    auto qc = m_sprite->getQuadCount();
    for (int i = 0; i< qc; ++i) {
        _primitiveIds.push_back(_batch->getPrimitiveId());
    }
	m_animation = py_get_dict<std::string>(args, "animation", m_sprite->getDefaultAnimation());
	_paletteId = py_get_dict<int>(args, "pal", 0);
}





const std::string & SpriteRenderer::getAnimation() const {
	return m_animation;
}

void SpriteRenderer::setAnimationForce(const std::string & anim) {
    m_animation.clear();
    setAnimation(anim);
}
void SpriteRenderer::setDirection(int dir ) {
    _direction = dir;
    if (dir == 1) {
        m_frame = 0;
        m_ticks = 0;

    } else if (dir == -1) {
        m_frame = m_animInfo->frames.size()-1;
        m_ticks = 0;
    }
}

void SpriteRenderer::setAnimation(const std::string& anim) {
	if (anim == m_animation) {
		return;
	}
	m_complete = false;
	m_animInfo = m_sprite->getAnimationInfo(anim);
	if (m_animInfo == nullptr) {
		GLIB_FAIL("mmh don't know animation: " + anim);
	}
	m_frame = 0;
	m_ticks = 0;
	m_animation = anim;
}





void SpriteRenderer::start() {
	m_frame = 0;
	m_animInfo = m_sprite->getAnimationInfo(m_animation);
	_currentFrameTicks = m_sprite->getFrameInfo(m_animation, m_frame).getTicks();

}

void SpriteRenderer::updateBatch() {
    const auto& a = m_sprite->getFrameInfo(m_animation, m_frame);
    // get world pos


    //auto pos = m_node->getWorldPosition();
    auto scale = m_node->getScale();
    // the bottom left corner depends whether entity is flipped horizontally
	//glm::vec3 ciao=pos;


    auto worldTransform = m_rendererTransform * m_node->getWorldMatrix();
    //glm::vec3 pos = worldTransform * glm::vec4(a.anchor_point, 0.f, 1.f);

    // draw all quads in the frame
    int qid = 0;

    for (const auto& quad : a.quads) {
        auto flipx = m_node->getFlipX();

        auto bottomLeft = worldTransform * glm::vec4( quad.location.x, quad.location.y, quad.location.z, 1.f) ;
        //glm::vec2 delta = scale * (flipx ? (glm::vec2(quad.size.x, 0.f) - quad.anchorPoint) : quad.anchorPoint);
        //auto bottomLeft = pos + scale * (m_shift + quad.location) - glm::vec3(delta, 0.f);
//		if (m_node->getLabel()== "sword") {
//			std::cout << bottomLeft.x << ", " << bottomLeft.y << " -- " << ciao.x << ", " << ciao.y <<  " ... \n";
//		}
        auto size = quad.size * scale;

        _batch->setQuad(_primitiveIds[qid++],
                              bottomLeft,
                              size,
                              quad.textureCoordinates,
                              quad.repeat,
                              _paletteId == 0 ? quad.palette : _paletteId,
                              flipx,
                              quad.flipv,
                              _zLayer);
    }
}


bool SpriteRenderer::updateTick(int tick) {
    //const auto& a = m_sprite->getFrameInfo(m_animation, m_frame);
    int tck = tick % _currentFrameTicks;
    if (tck >= _currentFrameTicks) {
        // increment frame. if this animation is
        m_frame += _direction;
        if (_direction == 1 && m_frame >= m_animInfo->frames.size()) {
            m_frame = m_animInfo->loop == -1 ? m_animInfo->frames.size() - 1 : m_animInfo->loop;
            m_complete = true;
            return true;
        } else if (_direction == -1 && m_frame < 0) {
            m_frame = m_animInfo->loop == -1 ? 0 : m_animInfo->loop;
        }
        _currentFrameTicks = m_sprite->getFrameInfo(m_animation, m_frame).getTicks();

    }
    return false;

}


void SpriteRenderer::update(double dt) {

	//_spriteBatch->setQuad(_quadId, bottomLeft, a.size, a.texture_coordinates, glm::vec2(1, 1), a.paletteIndex, flipx, false);

    updateBatch();
    //const auto& a = m_sprite->getFrameInfo(m_animation, m_frame);

    if (m_ticks >= _currentFrameTicks) {
        // increment frame. if this animation is
        m_frame += _direction;

        if (_direction == 1 && m_frame >= m_animInfo->frames.size()) {
        	if (m_animInfo->_onEnd) {
        		m_animInfo->_onEnd(m_node);
        	}
        	if (!m_animInfo->next.empty()) {
        		setAnimation(m_animInfo->next);
        	} else {
				m_frame = m_animInfo->loop == -1 ? m_animInfo->frames.size() - 1 : m_animInfo->loop;
				m_complete = true;
			}
        } else if (_direction == -1 && m_frame < 0) {
            if (m_animInfo->_onEnd) {
                m_animInfo->_onEnd(m_node);
            }
            if (!m_animInfo->next.empty()) {
                setAnimation(m_animInfo->next);
            } else {
                m_frame = m_animInfo->loop == -1 ? 0 : m_animInfo->loop;
                m_complete = true;
            }
        }
        m_ticks = 0;
		_currentFrameTicks = m_sprite->getFrameInfo(m_animation, m_frame).getTicks();

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
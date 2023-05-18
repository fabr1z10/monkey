#include "multisprite.h"
#include "../node.h"

MultiSprite::MultiSprite(std::shared_ptr<IBatch> batch) : Model() {
    _batch = dynamic_cast<QuadBatch*>(batch.get());


}

const std::vector<std::unique_ptr<MultiSprite::Node>> & MultiSprite::getNodes() const {
    return _nodes;
}

std::shared_ptr<Renderer> MultiSprite::getRenderer() const {
    return std::make_shared<MultiSpriteRenderer>(_batch);

}



int MultiSprite::getKey(const std::string &key) {
    return _map.at(key);
}

void MultiSprite::addSprite(const std::string &key, std::shared_ptr<Sprite> sprite, const std::string &parent,
                            int joint) {
    size_t id = _nodes.size();
    _map[key] = id;
    int parent_id = -1;
    if (!parent.empty()) {
        parent_id = _map.at(parent);
    }
    _nodes.push_back(std::make_unique<Node>(id, sprite, parent_id, joint));
    m_modelBounds = sprite->getBounds();
    if (!parent.empty()) {
        _nodes.at(_map.at(parent))->_next.push_back(id);
    }
}

MultiSpriteRenderer::MultiSpriteRenderer(QuadBatch* batch) : Renderer(),
    _spriteBatch(batch)
{
}


MultiSpriteRenderer::Node::Node(const MultiSprite::Node* node, int quadId) {
    _quadId = quadId;
    _key = node->id;
    _parent = node->_parent;
    _joint = node->_joint;
    _sprite = node->_sprite;
    _paletteId = 0;
    setAnimation(_sprite->getDefaultAnimation());
    _frame = 0;
    _ticks = 0;
    _next = node->_next;
}

void MultiSpriteRenderer::Node::setAnimation(const std::string & anim) {
    if (_animation != anim) {
        _animInfo = _sprite->getAnimInfo(anim);
        _animation = anim;
        _frame = 0;
        _ticks = 0;

    }
}
void MultiSpriteRenderer::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {
	Renderer::setModel(model, args);
    m_sprite = std::dynamic_pointer_cast<MultiSprite>(model);
    const auto& nodes = m_sprite->getNodes();

    int id = 0;
    for (const auto& node : nodes) {
        // get a quad for current sprite


        auto quadId = _spriteBatch->getPrimitiveId();

        _nodes.push_back(std::make_unique<MultiSpriteRenderer::Node>(node.get(), quadId));
                //quadId, node->id, node->_parent, node->_joint, node->_sprite->getDefaultAnimation(), 0, node->_sprite));

    }

}

void MultiSpriteRenderer::setAnimation(const std::string &key, const std::string &anim) {
    auto node = _nodes.at(m_sprite->getKey(key)).get();
    if (node->_animation != anim) {
        node->_animation = anim;
        node->_frame = 0;
        node->_ticks = 0;
        node->_sprite->getAnimInfo(anim);

    }


    _nodes.at(m_sprite->getKey(key))->_animation = anim;

}

void MultiSpriteRenderer::update(double) {

    // push root node
    std::list<int> nodes{0};
    //std::vector<glm::vec3> positions;
    //std::vector<const AnimInfo*> animInfos;

    // run a DFS on the tree
    while (!nodes.empty()) {
        auto current = _nodes[nodes.front()].get();
        nodes.pop_front();

        // store position

        if (current->_parent == -1) {
            // this is root node
            current->_pos = m_node->getWorldPosition();
        } else {
            // not the root
            auto parentNode = _nodes[current->_parent].get();
            current->_pos = parentNode->_pos +
                    parentNode->_animInfo->frameInfo[parentNode->_frame].joints[current->_joint];
        }

        auto& a = current->_animInfo->frameInfo[current->_frame];

        // update frame
        current->update();

        // update quad
        auto flipx = m_node->getFlipX() ^ a.flipx;
        glm::vec2 delta = flipx ? (glm::vec2(a.size.x, 0.f) - a.anchor_point) : a.anchor_point;
        auto bottomLeft = current->_pos - glm::vec3(delta, 0.f);

        _spriteBatch->setQuad(current->_quadId,
                              bottomLeft,
                              a.size,
                              a.texture_coordinates,
                              glm::vec2(1, 1),
                              current->_paletteId,
                              flipx,
                              a.flipy);


        // add next nodes
        for (const auto& next : current->_next) {
            nodes.push_front(next);
        }


    }


}
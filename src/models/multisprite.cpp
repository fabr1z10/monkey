#include "multisprite.h"
#include "../node.h"
#include "../pyhelper.h"
#include "../engine.h"
#include <iostream>

MultiSprite::MultiSprite(const pybind11::kwargs& args) {
    auto batchId = py_get_dict<int>(args, "batch", 0);
    _batch = dynamic_cast<QuadBatch*>(Engine::instance().getRoom()->getBatch(0, batchId));
    for (const auto& node : args["nodes"]) {
        auto dict = node.cast<pybind11::dict>();
        auto key = node["id"].cast<std::string>();
        auto parent = py_get_dict<std::string>(dict, "parent", "");
        auto joint = py_get_dict<int>(dict, "joint", 0);
        std::cout << "key : " << key << "\n";
        size_t id = _nodes.size();
        _map[key] = id;
        int parent_id = parent.empty() ? -1 : _map.at(parent);
        _nodes.push_back(std::make_unique<Node>(id, nullptr, parent_id, joint));
        if (!parent.empty()) {
            _nodes.at(_map.at(parent))->_next.push_back(id);
        }
    }
    auto anims = args["animations"].cast<pybind11::dict>();
    for (const auto& anim : anims) {
        auto animId = anim.first.cast<std::string>();
        std::unordered_map<int, std::string> dd;
        for (const auto& a : anim.second.cast<pybind11::dict>()) {
            dd[_map.at(a.first.cast<std::string>())] = a.second.cast<std::string>();
        }
        _anims[animId] = dd;
    }
    std::cout << "ok!\n";


}

//MultiSprite::MultiSprite(std::shared_ptr<IBatch> batch) : Model() {
//    _batch = dynamic_cast<QuadBatch*>(batch.get());
//
//
//}

const std::vector<std::unique_ptr<MultiSprite::Node>> & MultiSprite::getNodes() const {
    return _nodes;
}

std::shared_ptr<Renderer> MultiSprite::getRenderer(IBatch* batch) const {
    return std::make_shared<MultiSpriteRenderer>(batch);

}



int MultiSprite::getKey(const std::string &key) {
    return _map.at(key);
}

void MultiSprite::addSprite(const std::string &key, std::shared_ptr<Sprite> sprite, int palette) {
    const auto& node = _nodes.at(_map.at(key));
    node->_sprite = sprite;
    node->_pal = palette;
    //_nodes[_map.at(key)]->_sprite = sprite;

//    size_t id = _nodes.size();
//    _map[key] = id;
//    int parent_id = -1;
//    if (!parent.empty()) {
//        parent_id = _map.at(parent);
//    }
//    _nodes.push_back(std::make_unique<Node>(id, sprite, parent_id, joint));
//    m_modelBounds = sprite->getBounds();
//    if (!parent.empty()) {
//        _nodes.at(_map.at(parent))->_next.push_back(id);
//    }
}

MultiSpriteRenderer::MultiSpriteRenderer(IBatch* batch) : Renderer(),
    _spriteBatch(dynamic_cast<QuadBatch*>(batch))
{
    assert(_spriteBatch);
}


MultiSpriteRenderer::Node::Node(const MultiSprite::Node* node, int quadId) : link(node), _animInfo(nullptr), _complete(false) {
    _quadId = quadId;
    _key = node->id;
    _parent = node->_parent;
    _joint = node->_joint;
    //_sprite = node->_sprite;
    _paletteId = node->_pal;
    if (node->_sprite != nullptr) {
        setAnimation(node->_sprite->getDefaultAnimation());
    }
    _frame = 0;
    _ticks = 0;
    _next = node->_next;
}

void MultiSpriteRenderer::Node::setAnimation(const std::string & anim) {
    if (_animation != anim) {
        if (link->_sprite != nullptr) {
            _animInfo = link->_sprite->getAnimInfo(anim);
        }
        _animation = anim;
        _frame = 0;
        _ticks = 0;
        _complete = false;

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

bool MultiSpriteRenderer::isComplete() {
    bool complete = true;
    for (const auto& node : _nodes) {

        complete &= (node->_animInfo == nullptr || node->_complete);
        if (!complete) {
            return false;
        }
    }
    return complete;
}


void MultiSprite::addAnimation(const std::string& id, const std::string& subsprite, const std::string& anim) {
    _anims[id][_map.at(subsprite)] = anim;
}

const std::unordered_map<int, std::string> & MultiSprite::getAnimationData(const std::string & id) {
    auto anim = _anims.find(id);
    if (anim == _anims.end()) {
        std::cerr << "multisrptie has no animation: " << id;
        exit(1);
    }
    return  anim->second;
}

void MultiSpriteRenderer::setAnimation(const std::string &anim) {

    const auto& aa = m_sprite->getAnimationData(anim);
    for (const auto& m : aa) {
        _nodes[m.first]->setAnimation(m.second);
    }

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

        if (current->_animInfo == nullptr) {
            continue;
        }

        // store position




        // update frame
        current->update();
        auto& a = current->_animInfo->frameInfo[current->_frame];

        auto flipx = m_node->getFlipX() ^ a.flipx;
        auto worldPos =m_node->getWorldPosition();

        if (current->_parent == -1) {
            // this is root node
            current->_pos = worldPos;
        } else {
            // not the root
            auto parentNode = _nodes[current->_parent].get();
            auto joint = parentNode->link->_sprite->getJoint(parentNode->_animation, parentNode->_frame, current->_joint);

            //auto deltax = flipx ? parenta.size.x - joint.x : joint.x;
            current->_pos = flipx ? (parentNode->_bottomRight + glm::vec3(-joint.x, joint.y, 0.f)) :
                            (parentNode->_bottomLeft + glm::vec3(joint.x, joint.y, 0));
            //current->_pos = parentNode->_pos +
            //        parentNode->_animInfo->frameInfo[parentNode->_frame].joints[current->_joint];
        }
        // update quad

        glm::vec2 delta = flipx ? (glm::vec2(a.size.x - a.anchor_point.x,a.anchor_point.y)) : a.anchor_point;
        current->_bottomLeft = current->_pos - glm::vec3(delta, 0.f);
        current->_bottomLeft.z = worldPos.z + a.z;
        current->_bottomRight = current->_bottomLeft + glm::vec3(a.size.x, 0.f, 0.f);

        _spriteBatch->setQuad(current->_quadId,
                              current->_bottomLeft,
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


std::type_index MultiSpriteRenderer::getType() {
    return std::type_index(typeid(Renderer));
}
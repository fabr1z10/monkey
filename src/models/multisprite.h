#pragma once

#include "sprite.h"
#include "../components/renderer.h"
#include "../batch/quadbatch.h"


class MultiSprite : public Model {
public:
    MultiSprite(std::shared_ptr<IBatch> batch, const pybind11::kwargs& args);
	MultiSprite(std::shared_ptr<IBatch> batch);
    int getKey(const std::string& key);


	void addSprite(const std::string& key, std::shared_ptr<Sprite> sprite) ; //, const std::string& parent = std::string(), int joint = -1);
    std::shared_ptr<Renderer> getRenderer() const override;
    struct Node {
        Node(int id, std::shared_ptr<Sprite> sprite, int parent, int joint) : id(id), _sprite(sprite), _parent(parent), _joint(joint) {

        }

        int id;
        std::shared_ptr<Sprite> _sprite;
        int _parent;
        int _joint;
        std::vector<int> _next;
    };
    void addAnimation(const std::string& id, const std::string& subSprite, const std::string& anim);
    const std::unordered_map<int, std::string>& getAnimationData(const std::string&);
private:
    QuadBatch* _batch;
    std::unordered_map<std::string, std::unordered_map<int, std::string>> _anims;


    std::unordered_map<std::string, int> _map;
	std::vector<std::unique_ptr<Node>> _nodes;
public:
    const std::vector<std::unique_ptr<MultiSprite::Node>>& getNodes() const;


};


class MultiSpriteRenderer : public Renderer {
public:
    explicit MultiSpriteRenderer(QuadBatch*);

    void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;

	void update(double) override;

	void setAnimation(const std::string& anim) override;

    std::type_index getType() override;

private:
    QuadBatch* _spriteBatch;
    std::shared_ptr<MultiSprite> m_sprite;


	struct Node {
	    Node (const MultiSprite::Node* node, int quadId);
        const MultiSprite::Node* link;
	    int _quadId;
        int _key;
	    int _parent;
        int _joint;

	    //std::shared_ptr<Sprite> _sprite;
        int _paletteId;
        std::string _animation;
        int _frame;
        int _ticks;
        glm::vec3 _bottomLeft;
        glm::vec3 _bottomRight;
        void setAnimation(const std::string&);
        const AnimInfo* _animInfo;
        glm::vec3 _pos;
        void update() {
            if (_ticks >= _animInfo->frameInfo[_frame].ticks) {
                // increment frame. if this animation is
                _frame++;
                if (_frame >= _animInfo->frameCount) {
                    _frame = (_animInfo->loop ? _animInfo->loopFrame : _animInfo->frameCount - 1);
                }
                _ticks = 0;
            } else {
                // if it's not time to update frame, increment current frame length
                _ticks++;
            }
        }

        std::vector<int> _next;
	};
    std::vector<std::unique_ptr<Node>> _nodes;


};
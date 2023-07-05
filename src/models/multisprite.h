#pragma once

#include "sprite.h"
#include "../components/renderer.h"
#include "../batch/quadbatch.h"
#include "../tree.h"


class MultiSpriteRenderer;

class MultiModel : public Model {
public:

	~MultiModel();

    struct Node {
        Node(const std::string& id, std::unordered_map<std::string, MultiModel::Node*>& m) : _map(m), id(id) {
        	_map[id] = this;
        }
        ~Node() {
        	_map.erase(id);
        	for (auto child : children) {
        		delete child;
        	}
        }
        std::string id;
        std::shared_ptr<Model> model;
		std::shared_ptr<Renderer> renderer;
        std::vector<MultiModel::Node*> children;
		std::unordered_map<std::string, MultiModel::Node*>& _map;
    };


    friend class MultiModelRenderer;

    explicit MultiModel(const pybind11::kwargs& args);
	//MultiSprite();
//    int getKey(const std::string& key);
//
//
    // this should notify renderer to update its tree
	void setModel(const std::string& key, std::shared_ptr<Model> sprite, int palette) ; //, const std::string& parent = std::string(), int joint = -1);

	void erase(const std::string& key);

	MultiModel::Node* getNode(const std::string& id);

    std::shared_ptr<Renderer> getRenderer(IBatch*) override;
//    struct Node {
//        Node(int id, std::shared_ptr<Sprite> sprite, int parent, int joint) : id(id), _sprite(sprite), _parent(parent), _joint(joint) {
//
//        }
//
//        int id;
//        std::shared_ptr<Sprite> _sprite;
//        int _pal;
//        int _parent;
//        int _joint;
//        std::vector<int> _next;
//    };
//    void addAnimation(const std::string& id, const std::string& subSprite, const std::string& anim);
    const std::unordered_map<std::string, std::string>& getAnimationData(const std::string&);
//private:
//    QuadBatch* _batch;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _anims;
//
//
//    std::unordered_map<std::string, int> _map;
//	std::vector<std::unique_ptr<Node>> _nodes;
//public:
//    //const std::vector<std::unique_ptr<MultiSprite::Node>>& getNodes() const;

    //Tree<MultiModel::Node> _ciao;
    MultiModel::Node* _root;
    std::unordered_map<std::string, MultiModel::Node*> _nodes;
	mutable QuadBatch* _spriteBatch;
	MultiSpriteRenderer* _renderer;

};


class MultiSpriteRenderer : public Renderer {
public:
    explicit MultiSpriteRenderer(IBatch*);
//
    void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override;
    void start() override;
	void update(double) override;

	std::shared_ptr<Renderer> getSubModelRenderer(std::shared_ptr<Model>& model);

	void setAnimation(const std::string& anim) override;

    std::type_index getType() override;
//
//    bool isComplete() override;
//private:
//    QuadBatch* _spriteBatch;
    std::shared_ptr<MultiModel> m_sprite;
//
//
//	struct Node {
//	    Node (const MultiSprite::Node* node, int quadId);
//        const MultiSprite::Node* link;
//	    int _quadId;
//        int _key;
//	    int _parent;
//        int _joint;
//
private:
    QuadBatch* _spriteBatch;
	MultiModel::Node* _root;

//
};
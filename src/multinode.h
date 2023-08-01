#pragma once

#include "node.h"
#include "models/sprite.h"
#include "components/sprite_renderer.h"

struct NodeInfo {
	NodeInfo() {}
	NodeInfo(Node* n) : node(n), model(nullptr), renderer(nullptr), parent(""), joint(-1) {}
	NodeInfo(Node* n, const std::string& parent, int joint, float z) : node(n), parent(parent), joint(joint), model(nullptr), renderer(nullptr), z(z) {}
	Node* node;
	Sprite* model;
	SpriteRenderer* renderer;
	std::string parent;
	int joint;
	float z;
};

class MultiNode : public Node {
public:
	MultiNode(const YAML::Node& node);
	MultiNode(const MultiNode&);
	void initialize();
	std::shared_ptr<Node> clone() override;
	void add(std::shared_ptr<Node>);
	void setNodeModel(const std::string& node, const std::string& modelId);
	void addTo(std::shared_ptr<Node>, const std::string& parent, int joint, float);
	void setAnimation(const std::string&) override;
	//void setNodeModel(std::shared_ptr<Model> model, const pybind11::kwargs& args = pybind11::kwargs()) override;
	void update(double dt) override;
	void start() override;
private:
	std::unordered_map<std::string, NodeInfo> _nodes;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _animations;
	std::unordered_map<std::string /* node */,std::unordered_map<std::string, std::pair<std::string, int>>> _availableModels;
	std::unordered_map<std::string, std::string> _defaultModels;
	std::string _defaultAnim;
	std::string _currentAnim;
};
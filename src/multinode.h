#pragma once

#include "node.h"
#include "models/sprite.h"
#include "components/sprite_renderer.h"

struct NodeInfo {
	NodeInfo() : parentIndex(-1) {}
	NodeInfo(Node* n) : node(n), model(nullptr), renderer(nullptr), parent(""), joint(-1), parentIndex(-1) {}
	NodeInfo(Node* n, const std::string& parent, int joint, float z, int parentIndex) : node(n), parent(parent), joint(joint), model(nullptr), renderer(nullptr), z(z), parentIndex(parentIndex) {}
	Node* node;
	std::vector<int> children;
	Sprite* model;
	SpriteRenderer* renderer;
	std::string parent;
	int parentIndex;
	int joint;
	float z;
};

struct ModelInfo {
	std::string model;
	int palette;
	float zLayer;
};

class MultiNode : public Node {
public:
	MultiNode(const YAML::Node& node, const std::string& sheetId);
	MultiNode(const MultiNode&);
	void initialize(const std::string& batch);
	std::shared_ptr<Node> clone() override;
	//void add(std::shared_ptr<Node>);
	void setNodeModel(const std::string& node, const std::string& modelId, const std::string& batch);
	void addTo(std::shared_ptr<Node>, const std::string& parent, int joint, float, int index);
	void setAnimation(const std::string&) override;
	//void setNodeModel(std::shared_ptr<Model> model, const pybind11::kwargs& args = pybind11::kwargs()) override;
	void postProcess() override;
	void start() override;

private:
	std::vector<NodeInfo> _nodes;
	std::unordered_map<std::string, size_t> _nodeMap;
	int _rootIndex;

	//std::unordered_map<std::string, NodeInfo> _nodes;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _animations;
	std::unordered_map<std::string /* node */,std::unordered_map<std::string, ModelInfo>> _availableModels;
	std::unordered_map<std::string, std::string> _defaultModels;
	std::string _defaultAnim;
	std::string _currentAnim;
	std::string _sheetId;
};
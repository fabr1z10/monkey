#include "multinode.h"
#include "pyhelper.h"
#include <iostream>
#include "models/iquad.h"
#include "assetmanager.h"

MultiNode::MultiNode(const YAML::Node& n) : Node() {
	// root must be first node
	for (const auto& node : n["nodes"]) {
		auto nodeId = node.first.as<std::string>();
		if (node.second && node.second["parent"]) {
			auto n = std::make_shared<Node>(nodeId);
			auto parent = node.second["parent"].as<std::string>();
			auto joint = node.second["joint"].as<int>();
			auto z = node.second["z"].as<float>(0.0f);
			this->addTo(n, parent, joint, z);
		} else {
			_label = nodeId;
			_nodes.insert(std::make_pair(_label, NodeInfo(this)));
		}
	}

	for (const auto& anim : n["animations"]) {
		auto animId = anim.first.as<std::string>();
		if (_animations.empty()) {
			_defaultAnim = animId;
		}
		for (const auto& a : anim.second) {
			auto node = a.first.as<std::string>();
			auto nodeAnim = a.second.as<std::string>();
			_animations[animId][node] = nodeAnim;
		}
	}

	for (const auto& model : n["models"]) {
		auto nodeId = model.first.as<std::string>();
		for (const auto& n : model.second) {
			auto key = n.first.as<std::string>();
			auto spriteId = n.second[0].as<std::string>();
			auto palId = n.second[1].as<int>();
			_availableModels[nodeId][key] = {spriteId, palId};
		}
	}

	for (const auto& dm : n["default"]) {
		_defaultModels[dm.first.as<std::string>()] = dm.second.as<std::string>();
	}
}

void MultiNode::initialize() {
	for (const auto& model : _defaultModels) {
		auto node = model.first;
		auto modelId = model.second;
		setNodeModel(node, modelId);
	}
}

void MultiNode::setNodeModel(const std::string &node, const std::string &modelId) {
	const auto& p = _availableModels.at(node).at(modelId);
	auto& n = _nodes.at(node);
	if (p.first.empty()) {
		n.node->setModel(nullptr);
		n.renderer = nullptr;
		n.model = nullptr;

	} else {
		auto model = AssetManager::instance().getSprite(p.first);
		n.model =static_cast<Sprite*>(model.get());
		n.node->setModel(model);
		n.node->setPalette(p.second);
		n.renderer = static_cast<SpriteRenderer*>(n.node->getComponent<Renderer>());
	}
	if (!_currentAnim.empty())
		setAnimation(_currentAnim);
}


MultiNode::MultiNode(const MultiNode& other) : Node(other) {
	_nodes = other._nodes;
	for (const auto& child : getChildren()) {
		auto label = child.second->getLabel();
		auto& n =_nodes.at(label);
		n.node = child.second.get();
		n.renderer = nullptr;
		n.model = nullptr;
	}
	auto& n = _nodes.at(getLabel());
	n.node = this;
	n.renderer = nullptr;
	n.model = nullptr;


	_animations = other._animations;
	_defaultAnim = other._defaultAnim;
	_availableModels = other._availableModels;
	_defaultModels = other._defaultModels;
}

std::shared_ptr<Node> MultiNode::clone() {
	return std::make_shared<MultiNode>(*this);
}

void MultiNode::setAnimation(const std::string & anim) {
	std::cout << "called multinode::setanimation\n";
	auto& a = _animations.at(anim);
	for (const auto& b : a) {
		auto n = _nodes.at(b.first);
		if (n.renderer != nullptr) {
			n.renderer->setAnimationForce(b.second);
		}
	}
	_currentAnim = anim;
}


void MultiNode::start() {
	setAnimation(_defaultAnim);
	Node::start();
}





void MultiNode::add(std::shared_ptr<Node> node) {
	Node::add(node);
	auto label= node->getLabel();
	if (!label.empty()) {
		_nodes[label] = NodeInfo(node.get());
	}
}

void MultiNode::update(double dt) {





	Node::update(dt);
	std::list<Node*> l{this};
	//std::cout << "---\n";
	for (const auto& n : _nodes) {
		if (n.second.renderer == nullptr) {
			continue;
		}
		auto cf = n.second.renderer->getFrame();
		//std::cout << n.first << ", " << cf << "\n";
		if (!n.second.parent.empty()) {

			auto parent = _nodes.at(n.second.parent);
			if (parent.model == nullptr) {
				continue;
			}
			auto anim = parent.renderer->getAnimation();
			auto frame = parent.renderer->getFrame();
			const auto& f = parent.model->getFrameInfo(anim ,frame);
			auto pos = parent.node->getScale() * (-f.quads[0].anchorPoint + f.joints[n.second.joint]);
			n.second.node->setPosition(pos.x, pos.y, n.second.z);

		}
	}
}

void MultiNode::addTo(std::shared_ptr<Node> node, const std::string &parent, int joint, float z) {
	_nodes.at(parent).node->add(node);
	_nodes[node->getLabel()] = NodeInfo(node.get(), parent, joint, z);
}

//void MultiNode::setNodeModel(std::shared_ptr<Model> model, const pybind11::kwargs &args) {
//	auto node = py_get_dict<std::string>(args, "node", "");
//	if (node.empty()) {
//		Node::setNodeModel(model, args);
//	}
//	auto& n = _nodes.at(node);
//	n.node->Node::setNodeModel(model, args);
//	n.renderer = static_cast<SpriteRenderer*>(n.node->getComponent<Renderer>());
//	n.model = static_cast<Sprite*>(model.get());
//}
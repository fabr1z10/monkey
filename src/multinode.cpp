#include "multinode.h"
#include "pyhelper.h"
#include <iostream>
#include "models/iquad.h"
#include "assetmanager.h"

MultiNode::MultiNode(const YAML::Node& n, const std::string& sheetId) : Node(), _sheetId(sheetId) {
	// root must be first node
	_rootIndex = 0;
	for (const auto& node : n["nodes"]) {
		auto nodeId = node.first.as<std::string>();
		_nodeMap[nodeId] = _nodes.size();
		if (node.second && node.second["parent"]) {
			auto n = std::make_shared<Node>();
			auto parent = node.second["parent"].as<std::string>();
			auto joint = node.second["joint"].as<int>();
			auto z = 0.f;// node.second["z"].as<float>(0.0f);
			this->addTo(n, parent, joint, z, _nodes.size());
		} else {
			assert(_nodes.empty());
			//_label = nodeId;
			_nodes.emplace_back(this);

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
			auto zLayer = n.second[2].as<float>();
			_availableModels[nodeId][key] = {spriteId, palId, zLayer};
		}
	}

	for (const auto& dm : n["default"]) {
		_defaultModels[dm.first.as<std::string>()] = dm.second.as<std::string>();
	}
}

void MultiNode::initialize(const std::string& batch) {
	for (const auto& model : _defaultModels) {
		auto node = model.first;
		auto modelId = model.second;
		setNodeModel(node, modelId, batch);
	}

}

void MultiNode::setNodeModel(const std::string &node, const std::string &modelId, const std::string& batch) {
	const auto& p = _availableModels.at(node).at(modelId);
	pybind11::kwargs args;
	auto& n = _nodes[_nodeMap.at(node)];
	if (p.model.empty()) {
		n.node->setModel(nullptr);
		n.renderer = nullptr;
		n.model = nullptr;
	} else {
		auto model = AssetManager::instance().getSprite(_sheetId + "/" + p.model);
		n.model =static_cast<Sprite*>(model.get());
		args["batch"] = batch;
		n.node->setModel(model, args);
		n.node->setPalette(p.palette);
		n.renderer = static_cast<SpriteRenderer*>(n.node->getComponent<Renderer>());
		n.renderer->setZLayer(p.zLayer);
	}

	if (!_currentAnim.empty())
		setAnimation(_currentAnim);

	this->startRecursive();
}


MultiNode::MultiNode(const MultiNode& other) : Node(other), _sheetId(other._sheetId) {
	_nodes = other._nodes;
	_nodeMap = other._nodeMap;
	std::list<Node*> l{this};
	std::list<const Node*> m{&other};
	std::vector<Node*> mms;
	std::vector<const Node*> mms2;
	std::unordered_map<const Node*, Node*> o2n;
	while (!l.empty()) {
		auto* current = l.front();
		l.pop_front();
		mms.push_back(current);
//		auto& n = _nodes[_nodeMap.at(current->getLabel())];
//		n.node = current;
//		n.renderer = nullptr;
//		n.model = nullptr;
		for (const auto& child : current->getChildren()) {
			l.push_back(child.second.get());
		}
	}

	while (!m.empty()) {
		auto* current = m.front();
		m.pop_front();
		mms2.push_back(current);
		for (const auto& c : current->getChildren()) {
			m.push_back(c.second.get());
		}
	}
	for (size_t i = 0; i < mms2.size(); ++i) {
		o2n[mms2[i]] = mms[i];
	}

	for (auto& n : _nodes) {
		n.node = o2n.at(n.node);
		n.renderer = nullptr;
		n.model = nullptr;
	}



//	for (const auto& child : getChildren()) {
//		auto label = child.second->getLabel();
//		auto& n =_nodes.at(label);
//	}
//	auto& n = _nodes.at(getLabel());
//	n.node = this;
//	n.renderer = nullptr;
//	n.model = nullptr;


	_animations = other._animations;
	_defaultAnim = other._defaultAnim;
	_availableModels = other._availableModels;
	_defaultModels = other._defaultModels;
	_currentAnim = _defaultAnim;
}

std::shared_ptr<Node> MultiNode::clone() {
	return std::make_shared<MultiNode>(*this);
}

void MultiNode::setAnimation(const std::string & anim) {
	std::cout << "called multinode::setanimation " << anim << "\n";
	auto& a = _animations.at(anim);
	for (const auto& b : a) {
		auto n = _nodes[_nodeMap.at(b.first)];
		if (n.renderer != nullptr) {
			std::cout << " * " << b.first << ", " << b.second << n.renderer << "\n";
			n.renderer->setAnimationForce(b.second);
		}
	}
	_currentAnim = anim;
}


void MultiNode::start() {
	setAnimation(_currentAnim);
	Node::start();
}





//void MultiNode::add(std::shared_ptr<Node> node) {
//	Node::add(node);
//	auto label= node->getLabel();
//	if (!label.empty()) {
//		_nodes[label] = NodeInfo(node.get());
//	}
//}

void MultiNode::postProcess() {





	//Node::update(dt);

	std::list<int> li{0};
	while (!li.empty()) {
		int current = li.front();
		auto& info = _nodes[current];
		li.pop_front();
		if (info.renderer == nullptr) {
			continue;
		}
		auto cf = info.renderer->getFrame();

		if (!info.parent.empty()) {
			auto parentInfo = _nodes[info.parentIndex];//.at(n.second.parent);
			if (parentInfo.model == nullptr) {
				continue;
			}
			auto anim = parentInfo.renderer->getAnimation();
			auto frame = parentInfo.renderer->getFrame();
			const auto& f = parentInfo.model->getFrameInfo(anim ,frame);
			auto pos = /*parentInfo.node->getScale() **/ (-f.quads[0].anchorPoint + f.joints[info.joint]);
			info.node->setPosition(pos.x, pos.y, 0.f);
		}
		//std::cout << "node: " << current << ", " << info.node->getLocalPosition().x << ", " << info.node->getLocalPosition().y << "\n";
		for (auto i : info.children) {
			li.push_front(i);
		}

	}
	//std::cout << "---\n";

}

void MultiNode::addTo(std::shared_ptr<Node> node, const std::string &parent, int joint, float z, int index) {
	_nodes.emplace_back(node.get(), parent, joint, z, _nodeMap.at(parent));
	auto& parentInfo = _nodes[_nodeMap.at(parent)];
	parentInfo.node->add(node);
	parentInfo.children.push_back(index);

	_nodeMap[node->getTag()] = index;


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
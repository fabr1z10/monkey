#include "node.h"
#include <glm/gtx/transform.hpp>
#include "engine.h"
#include "components/renderer.h"
#include "util.h"
#include "components/collider.h"
#include "pyhelper.h"
//#include "components/statemachine.h"
//#include "models/text.h"

Node::Node() : _id(Engine::instance().getNextId()), m_modelMatrix(1.0f), _state(NodeState::ACTIVE),
    m_parent(nullptr), m_worldMatrix(1.0f), m_started(false), m_userData(pybind11::dict()), m_scaleMatrix(glm::mat4(1.f)),
    m_model(nullptr), _scale(1.0f), _angle(0.f), _renderer(nullptr) {



    Engine::instance().addNode(this);
}

Node::Node(const Node& other) : _id(Engine::instance().getNextId()), m_parent(nullptr) {
	_state = other._state;
	_tag = other._tag;
	_scale = other._scale;
	for (const auto& child : other.m_children) {
		add(child->clone());
	}
	for (const auto& component : other.m_components) {
		// clone component
	}
	m_modelMatrix = other.m_modelMatrix;
	m_worldMatrix = other.m_worldMatrix;
	m_scaleMatrix = other.m_scaleMatrix;

	m_started = other.m_started;
	m_userData = other.m_userData;
	//m_camera = other.m_camera;
	m_model = other.m_model;
}

std::shared_ptr<Node> Node::clone() {
	return std::make_shared<Node>(*this);
}


std::string Node::getTag() const {
	return _tag;
}

void Node::setTag(const std::string& tag) {
	_tag = tag;
}

Node::~Node() {
	std::cout << " -- dtor " << _id << ", " << _tag << std::endl;
    onRemove.fire(this);
    m_components.clear();
    m_children.clear();
    Engine::instance().rmNode(this);
}

void Node::setPalette(const std::string& id) const {
	auto renderer = getComponent<Renderer>();
	if (renderer != nullptr) {
	    renderer->setPalette(id);
	}
}

//std::string Node::getAnimation() const {
//	return getComponent<Renderer>()->getAnimation();
//}
//
void Node::setAnimation(const std::string& animId) {
	_renderer->setAnimation(animId);
}

void Node::setParent(Node * node) {
    m_parent = node;
}


void Node::add(std::shared_ptr<Node> node) {
	auto it = m_children.insert(m_children.end(), node);
	_cache[node->getId()] = it;

	//#m_children.insert(std::make_pair(node->getId(), node));
    node->setParent(this);

    auto& engine = Engine::instance();
    //engine.addNode(node);
    // call start if engine is running (node added on the fly)
    if (engine.isRunning()) {
        node->startRecursive();
    }
	if (_state != NodeState::ACTIVE) {
		node->setState(_state);
	}
	try {
		auto obj = py::cast(node.get());
		engine.storeRef(node->getId(), obj);
	} catch(py::cast_error& err) {

	}


}

// relocate a node in the node-tree
void Node::moveTo(std::shared_ptr<Node> node) {
    m_children.push_back(node);
    node->getParent()->removeChild(node->getId());
    node->setParent(this);
}

void Node::remove() {
	std::cout << " ciccio " << m_parent->_cache.at(_id)->use_count() << "\n";

	setState(NodeState::INACTIVE);
	for (auto& c : m_components) c.second->shutdown();
	std::cout << " ciccio " << m_parent->_cache.at(_id)->use_count() << "\n";

    Engine::instance().scheduleForRemoval(this);
}

void Node::clearChildren() {
    m_children.clear();
}


void Node::removeChild(long id) {
	std::cout << " ciccio " << _cache.at(id)->use_count() << "\n";
	m_children.erase(_cache.at(id));
	Engine::instance().rmRef(id);

    _cache.erase(id);
}

void Node::start() {
	m_started = true;
	// update world matrix
	if (m_parent != nullptr) {
		m_worldMatrix = m_parent->getWorldMatrix() * m_modelMatrix;
	}
	for (auto& c : m_components){
		c.second->start();
	}
	if (_renderer != nullptr)
		_renderer->start();
}

void Node::startRecursive() {
    Node::start();
    for (const auto& c : m_children) {
        c->start();
    }
}

void Node::update(double dt) {
    // don't do any update if node is not active!
    if (_state != NodeState::ACTIVE) {
        return;
    }

    for (auto& iter : m_components) {
        if (iter.second->getState() == NodeState::ACTIVE) {
            iter.second->update(dt);
        }
    }

    // update world matrix
    if (m_parent != nullptr) {
        m_worldMatrix = m_parent->getWorldMatrix() * m_modelMatrix;
    }

	if (_renderer != nullptr && _renderer->getState() == NodeState::ACTIVE) {
		_renderer->update(dt);
	}

}

std::vector<Node *> Node::getNodes(bool recursive) {
    std::vector<Node*> result;
    std::list<Node*> l{this};
    while (!l.empty()) {
        auto current = l.front();
        l.pop_front();
        for (const auto& child : current->m_children) {
            result.push_back(child.get());
            if (recursive) {
                l.push_back(child.get());
            }
        }
    }
    return result;
}

void Node::setState(NodeState state) {
    // state is inherited by all components and all children
    _state = state;
    for (const auto& comp : m_components) {
        comp.second->setState(state);
    }
    for (const auto& c : m_children) {
    	c->setState(state);
    }
}

glm::mat4 Node::getWorldMatrix() const {
    return m_worldMatrix;
}

glm::vec3 Node::getWorldPosition() const {
	return glm::vec3(m_worldMatrix[3]);
}

glm::vec3 Node::getLocalPosition() const {
	return glm::vec3(m_modelMatrix[3]);
}

pybind11::object Node::getUserData() {
    return m_userData;
}

void Node::setUserData(pybind11::object userData) {
    m_userData = userData;
}


void Node::setZ(float z) {
	m_modelMatrix[3][2] = z;
	notifyMove();
}

Component * Node::getTaggedComponent(const std::string &tag) {
    for (const auto& i : m_components) {
        if (i.second->getLabel() == tag) {
            return i.second.get();
        }
    }
    return nullptr;
}

void Node::setPosition(float x, float y) {
    m_modelMatrix[3][0] = x;
    m_modelMatrix[3][1] = y;
    notifyMove();

}

void Node::setAngle(float angle) {
    auto rot = glm::rotate(angle, glm::vec3(0.f, 0.f, 1.f));
    m_modelMatrix[0] = rot[0];
    m_modelMatrix[1] = rot[1];
    m_modelMatrix[2] = rot[2];
    notifyMove();
}

void Node::setPosition(float x, float y, float z) {
    m_modelMatrix[3][0] = x;
    m_modelMatrix[3][1] = y;
    m_modelMatrix[3][2] = z;
    //if (m_parent == nullptr)
	//m_worldMatrix = m_parent->getWorldMatrix() * m_modelMatrix;
	notifyMove();
}

void Node::move(glm::mat4 m) {
	m_modelMatrix *= m;
	m_worldMatrix = m_parent->getWorldMatrix() * m_modelMatrix;
	notifyMove();
}

void Node::move(glm::vec3 delta) {
	m_modelMatrix[3][0] += sign(m_modelMatrix[0][0]) * delta.x;
	m_modelMatrix[3][1] += delta.y;
	m_modelMatrix[3][2] += delta.z;
	notifyMove();
}

void Node::moveLocal(glm::vec3 delta) {
    m_modelMatrix *= glm::translate(delta);
    notifyMove();
}

void Node::movea(glm::vec3 delta) {
	m_modelMatrix[3][0] += delta.x;
	m_modelMatrix[3][1] += delta.y;
	m_modelMatrix[3][2] += delta.z;
	notifyMove();
}

void Node::notifyMove() {
	if (m_parent != nullptr) {
		m_worldMatrix = m_parent->getWorldMatrix() * m_modelMatrix;
	} else {
		m_worldMatrix = m_modelMatrix;
	}

	onMove.fire(this);
	for (const auto& child : m_children) {
		child->notifyMove();
	}
}


void Node::addComponent(std::shared_ptr<Component> c) {
    m_components[c->getType()] = c;
    c->setNode(this);
}



std::shared_ptr<Model> Node::getModel() {
    return m_model;
}

void Node::setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args) {

	if (model == nullptr) {
		if (m_model != nullptr) {
			m_components.erase(std::type_index(typeid(Renderer)));
		}
		m_model = nullptr;

	} else {
		m_model = model;
		auto renderer = model->getRenderer(args);
		//this->addComponent(renderer);
		setRenderer(renderer);
		renderer->setModel(model, args);
		if (Engine::instance().isRunning()) {
			this->start();
		    //renderer->start();

		}
	}

	//auto model = node->getModel();
	//auto renderer = model->getRenderer(this);
	//node->addComponent(renderer);
	//renderer->setNodeModel(model, args);
//	auto *r = getComponent<Renderer>();
//	if (r == nullptr) {
//		auto renderer = model->getRenderer();
//		renderer->setNodeModel(model, args);
//		addComponent(renderer);
//	} else {
//		r->setNodeModel(model);
//		for (auto &c : m_components) {
//			c.second->start();
//		}
//	}
	m_model = model;
}

Bounds Node::getBounds() {
	if (m_model) {
		auto bounds = m_model->getBounds();
		bounds.translate(getWorldPosition());
		return bounds;
	}
	return Bounds::maxBounds();
}


bool Node::getFlipX() const {
	return m_worldMatrix[0][0] < 0.f;
}

void Node::setFlipX(bool value) {
	m_modelMatrix[0][0] = (value ? -1.f : 1.f) * abs(m_modelMatrix[0][0]);
	//if (m_parent != nullptr)
	//	m_worldMatrix = m_parent->getWorldMatrix() * m_modelMatrix;
	notifyMove();
}

//std::string Node::getState() const {
//	auto sm = getComponent<StateMachine>();
//	auto pino = sm->getState()->getId();
//	return pino;
//}
//
//void Node::setState(const std::string &state, const pybind11::kwargs& kwargs) {
//	auto sm = getComponent<StateMachine>();
//	if (sm != nullptr) {
//		sm->setState(state, kwargs);
//	}
//}

float Node::getX() const {
	return m_worldMatrix[3][0];
}

float Node::getY() const {
	return m_worldMatrix[3][1];
}

float Node::getZ() const {
	return m_worldMatrix[3][2];
}

int Node::getCollisionTag() const {
	return getComponent<Collider>()->getCollisionTag();
}


void Node::setScale(float scale) {
    for (int i = 0; i < 3; ++i) {
        m_modelMatrix[i][i] = signf(m_modelMatrix[i][i]) * scale;
    }
    notifyMove();
}

float Node::getScale() const {
	return fabs(m_worldMatrix[0][0]);
}



void Node::rotate(float angle, glm::vec3 axis) {
	m_modelMatrix *= glm::rotate(angle, axis);
	notifyMove();

}

//void Node::sendMessage(const pybind11::kwargs & args) {
//    for (const auto& component : m_components) {
//        component.second->sendMessage(args);
//    }
//}

void Node::sendMessage(const std::string& id, const pybind11::kwargs& args) {
    _messages.at(id)(**args);
}

void Node::addMessage(const std::string& id, pybind11::function f) {
    _messages[id] = f;

}

void Node::setRenderer(std::shared_ptr<Renderer> r) {
	_renderer = r;
	r->setNode(this);
}
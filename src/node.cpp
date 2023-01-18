#include "node.h"
#include "engine.h"
#include "components/renderer.h"

Node::Node() : _id(Engine::instance().getNextId()), m_camera(nullptr), m_modelMatrix(1.0f), m_active(true),
    m_parent(nullptr), m_worldMatrix(1.0f), m_started(false), m_userData(pybind11::dict()), m_scaleMatrix(glm::mat4(1.f)) {
}

Node::~Node() {
    // TODO
    m_components.clear();
    m_children.clear();
}


void Node::setParent(Node * node) {
    m_parent = node;
}


void Node::add(std::shared_ptr<Node> node) {
    m_children.insert(std::make_pair(node->getId(), node));
    node->setParent(this);

    auto& engine = Engine::instance();
    engine.addNode(node);
    // call start if engine is running (node added on the fly)
    if (engine.isRunning()) {
        node->start();
    }
}

// relocate a node in the node-tree
void Node::moveTo(std::shared_ptr<Node> node) {
    m_children.insert(std::make_pair(node->getId(), node));
    node->getParent()->removeChild(node->getId());
    node->setParent(this);
}

void Node::remove() {
    Engine::instance().scheduleForRemoval(this);
}

void Node::removeChild(long id) {
    m_children.erase(id);
}

void Node::start() {
    // TODO
}

void Node::update(double dt) {
    if (!m_active) return;
    for (auto& iter : m_components) {
        iter.second->update(dt);
    }
    // update world matrix
    if (m_parent != nullptr) {
        m_worldMatrix = m_parent->getWorldMatrix() * m_modelMatrix;
    }
}

void Node::setActive(bool active) {
    m_active = active;
    for (const auto& comp : m_components) {
        comp.second->setActive(active);
    }
}

glm::mat4 Node::getWorldMatrix() const {
    return m_worldMatrix;
}

pybind11::object Node::getUserData() {
    return m_userData;
}

void Node::setUserData(pybind11::object userData) {
    m_userData = userData;
}

void Node::setCamera(std::shared_ptr<Camera> cam) {
    m_camera = cam;
}

void Node::setPosition(float x, float y, float z) {
    m_modelMatrix[3][0] = x;
    m_modelMatrix[3][1] = y;
    m_modelMatrix[3][2] = z;
}

void Node::addComponent(std::shared_ptr<Component> c) {
    m_components[c->getType()] = c;
    c->setNode(this);
}

void Node::setModel(std::shared_ptr<Model> model) {

    auto* r = getComponent<Renderer>();
    if (r == nullptr) {
        auto renderer = model->getRenderer();
        renderer->setModel(model);
        addComponent(renderer);
    } else {
        r->setModel(model);
        for (auto& c : m_components) {
            c.second->start();
        }
    }


}
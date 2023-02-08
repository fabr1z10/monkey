#include "scheduler.h"
#include "../pyhelper.h"

long Action::getId() const {
	return _id;
}
void Action::setId(long id) {
	_id = id;
}

//
//void Scheduler::start() {
//    for (auto& b : m_actions) {
//        b.second->init(m_node);
//    }
//}

Script::Script(const pybind11::kwargs& args) : m_done(false), _nextId(0), _lastAddedId(-1), _loopId(-1) {
	m_scriptId = py_get_dict<std::string>(args, "id", "");
}



long Script::add(std::shared_ptr<Action> action, const pybind11::kwargs & args) {
	action->setId(_nextId);
	auto loop = py_get_dict<bool>(args, "loop", false);
	if (loop) {
		_loopId = _nextId;
	}
	m_actions[_nextId] = action;
	m_inDegree[_nextId] = 0;
	m_edges[_nextId] = std::vector<int>();
	auto after = py_get_dict<std::vector<int>>(args, "after", std::vector<int>());
	if (after.empty() && _lastAddedId != -1) {
		m_inDegree[_nextId] = 1;
		m_edges[_lastAddedId].push_back(_nextId);
	} else {
		for (const auto& predecessor : after) {
			m_inDegree[_nextId]++;
			m_edges[predecessor].push_back(_nextId);
		}
	}
	_lastAddedId = _nextId;
	// return newly added id and increment
	return _nextId++;
}

void Script::kill() {
	m_done = true;
	// maybe do something more?
	for (auto& action : m_current) {
		action->end();
	}
}

void Script::update(double dt) {
	// is there anything to process? do it otherwise do nothing


	std::vector<int> complete;
	for (auto it = m_current.begin(); it != m_current.end();) {
		if ((*it)->run(dt) == 0) {
			// action is completed
			(*it)->end();
			for (const auto& edge : m_edges.at((*it)->getId())) {
				m_inDegree[edge]--;
			}
			complete.push_back((*it)->getId());
			it = m_current.erase(it);
		} else {
			it++;
		}
	}

	// check for new actions
	std::unordered_map<long, int>::iterator it = m_inDegree.begin();
	while (it != m_inDegree.end()) {
		// Check if in degree is 0
		if (it->second == 0) {
			// erase() function returns the iterator of the next
			// to last deleted element.
			auto action = m_actions.at(it->first);
			action->start();
			m_current.push_back(action);
			it = m_inDegree.erase(it);
		} else {
			it++;
		}
	}


	// cleanup
	if (_loopId  == -1) {
		for (auto c : complete) {
			m_inDegree.erase(c);
			m_actions.erase(c);
		}
	}
	if (m_current.empty()) {
		if (_loopId == -1) {
			m_done = true;
		} else {
			// reset in degree
			std::list<int> m;
			m.push_back(_loopId);
			std::set<int> explored;
			explored.insert(_loopId);
			m_inDegree[_loopId] = 0;
			while (!m.empty()) {
				auto current = m.front();
				m.pop_front();
				auto it = m_edges.find(current);
				if (it != m_edges.end()) {
					for (const auto& n : it->second) {
						m_inDegree[n]++;
						if (explored.count(n) == 0) {
							m.push_back(n);
							explored.insert(n);
						}
					}
				}
			}

		}
	}

}

void Scheduler::update(double dt) {

	// run all scripts
	for (auto it = m_scripts.begin(); it != m_scripts.end();) {
		if ((*it)->done()) {
			m_scriptMap.erase((*it)->getId());
			it = m_scripts.erase(it);
		} else {
			(*it)->update(dt);
			it++;
		}
	}


}

long Scheduler::add(std::shared_ptr<Script> s) {
	m_ids[_nextId] = s;
	m_scripts.push_back(s);
	auto sid = s->getId();
	if (!sid.empty()) {
		auto f = m_scriptMap.find(sid);
		if (f != m_scriptMap.end()) {
			f->second->get()->kill();
			m_scripts.erase(f->second);
		}
		m_scriptMap[sid] = std::prev(m_scripts.end());
	}
	return _nextId++;

}

void Scheduler::kill(long id) {
	m_ids.at(id)->kill();

}

#pragma once

#include "../runner.h"
#include <list>
#include <memory>
#include <unordered_map>
#include <pybind11/pybind11.h>
#include "../pycast.h"

class Node;

class Action {
public:
	Action() = default;

	Action(const pybind11::kwargs& args);

	virtual void start();

	virtual void reset() {}

	int run(double);
	// stop current action
	virtual void stop();
	//void init(Node*);
	// virtual stuff

	/**
	 * The core action algorithm
	 * @param dt the time interval
	 * @return 0 if completed, 1 if not
	 */
	virtual int process(double) = 0;

	// wrap up code goes here
	virtual void onEnd();

	//void setOnEnd(pybind11::function f);

	long getId() const;
	void setId(long);

protected:
	pybind11::function _onEnd;
	int _status; // 0 not started, 1 running, 2 done
	long _id;
	bool _forcedStop;
	bool _repeat;

};

inline void Action::start() {
	_status = 1;
}

inline void Action::stop() {
	_status = 2;

	if (_onEnd) {
		_onEnd();
	}
}



class Script {
public:
	Script(const pybind11::kwargs&);
	long add(std::shared_ptr<Action> action, const pybind11::kwargs&);
	void update(double);
	bool done() const;
	void kill();
	std::string getId() const;
	int getLastId() const;
	void setOnKill(pybind11::function);
private:
	std::list<std::shared_ptr<Action>> m_current;
	std::unordered_map<int, std::shared_ptr<Action>> m_actions;
	std::unordered_map<int, std::vector<int>> m_edges;
	std::unordered_map<long, int> m_inDegree;
	long _nextId;
	long _lastAddedId;
	std::string m_scriptId;
	bool m_done;
	long _loopId;
	pybind11::function _onKill;

};

inline void Script::setOnKill(pybind11::function f) {
	_onKill = f;
}

inline std::string Script::getId() const {
	return m_scriptId;
}

inline int Script::getLastId() const {
    return _lastAddedId;
}

inline bool Script::done() const {
	return m_done;
}



class Scheduler : public Runner {
public:
	Scheduler() = default;
	void update(double) override;
//    void start() override;
	long add(std::shared_ptr<Script>);
	void kill(const std::string& id);
private:
	std::list<std::shared_ptr<Script>> m_scripts;
	//std::unordered_map<std::string, std::shared_ptr<Script>> m_tags;
	//std::unordered_map<long, std::shared_ptr<Script>> m_ids;
	long _nextId;
	std::vector<long> m_toErase;
	std::unordered_map<std::string, std::list<std::shared_ptr<Script>>::iterator> m_scriptMap;
};
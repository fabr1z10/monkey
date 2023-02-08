#pragma once

#include "../runner.h"
#include <list>
#include <memory>
#include <unordered_map>
#include <pybind11/pybind11.h>

class Node;

class Action {
public:
	Action() : _status(0), _id(-1) {}
	// return 0 when completed
	virtual void start();
	virtual int run(double) = 0;
	virtual void end();
	long getId() const;
	void setId(long);
	void init(Node*);

protected:
	int _status; // 0 not started, 1 running, 2 done
	long _id;

};

inline void Action::start() {
	_status = 1;
}

inline void Action::end() {
	_status = 2;
}

class Script {
public:
	Script(const pybind11::kwargs&);
	long add(std::shared_ptr<Action> action, const pybind11::kwargs&);
	void update(double);
	bool done() const;
	void kill();
	std::string getId() const;
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

};

inline std::string Script::getId() const {
	return m_scriptId;
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
	void kill(long);
private:
	std::list<std::shared_ptr<Script>> m_scripts;
	//std::unordered_map<std::string, std::shared_ptr<Script>> m_tags;
	std::unordered_map<long, std::shared_ptr<Script>> m_ids;
	long _nextId;
	std::vector<long> m_toErase;
	std::unordered_map<std::string, std::list<std::shared_ptr<Script>>::iterator> m_scriptMap;
};
#pragma once

#include <functional>
#include <unordered_map>

template<typename Args>
class Event {
private:
	struct Callback {
		std::function<void(Args)> callback;
	};
public:
	Event() : m_enabled(true) {}
	void setEnabled(bool value) {
		m_enabled = value;
	}

	// register interest in this event with context
	// returns an id that can be used to unregister
	int reg(std::function<void(Args)> f) {
		m_callbacks[m_next] = f;
		return m_next++;
	}

	void unreg(int id) {
		m_callbacks.erase(id);
	}

	void fire(Args args) {
		if (!m_enabled)
			return;
		for (auto& f : m_callbacks) f.second(args);
	}

	bool isEmpty() {
		return m_callbacks.empty();
	}
private:
	bool m_enabled;
	std::unordered_map<int, std::function<void(Args)>> m_callbacks;
	int m_next;
};



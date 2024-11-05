#include <utility>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <iostream>

template <typename T>
class OrderedPair {
public:
	OrderedPair(T* ptr1, T* ptr2) {
		// Enforce ordering in constructor
		if (ptr2 < ptr1) {
			std::swap(ptr1, ptr2);
		}
		first = ptr1;
		second = ptr2;
	}

	T* getFirst() const { return first; }
	T* getSecond() const { return second; }

	std::pair<T*, T*> asPair() const { return {first, second}; }

	// Equality operator for unordered_map key comparison
	bool operator==(const OrderedPair<T>& other) const {
		return first == other.first && second == other.second;
	}

private:
	T* first;
	T* second;
};

// Hash specialization for OrderedPair
namespace std {
	template <typename T>
	struct hash<OrderedPair<T>> {
		size_t operator()(const OrderedPair<T>& pair) const {
			// Combine the hash of the two pointers
			return hash<T*>()(pair.getFirst()) ^ (hash<T*>()(pair.getSecond()) << 1);
		}
	};
}
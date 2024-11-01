#include "trie.h"


TrieNode::TrieNode() : _isToken(false) {}

TrieNode::~TrieNode() {
	for (auto p : _children) {
		delete p.second;
	}
}

Trie::Trie() {
	_root = new TrieNode();
}

Trie::~Trie() {
	delete _root;
}

void Trie::addToken(const std::string & s) {

	auto* node = _root;
	for (char ch : s) {

		if (!node->_children[ch]) {
			node->_children[ch] = new TrieNode();
		}
		node = node->_children[ch];
	}
	node->_isToken = true;
}

std::string Trie::findToken(const std::string & target) {
	auto* node = _root;
	std::string token;
	for (size_t i = 0; i < target.size(); ++i) {
		if (!node->_children[target[i]]) {
			return token;
		}
		node = node->_children[target[i]];
		if (node->_isToken) {
			token = target.substr(0, i+1);
		}
	}
	return token;
}
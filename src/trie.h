#pragma once

#include <unordered_map>
#include <string>

class TrieNode {
public:
	TrieNode();
	~TrieNode();
	std::unordered_map<char, TrieNode*> _children;
	bool _isToken;
};

class Trie {
public:
	Trie();
	~Trie();
	void addToken(const std::string&);
	std::string findToken(const std::string&);
private:
	TrieNode* _root;

};
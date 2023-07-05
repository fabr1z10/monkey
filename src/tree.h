#pragma once


template<typename T>
class TreeNode {
    template<typename U> friend class Tree;
    std::vector<TreeNode<T>*> children;
public:
    T data;
    TreeNode<T>* parent;

    TreeNode(T value) {
        data = value;
        parent = nullptr;
    }

    ~TreeNode() {
        for (auto c : children) {
            delete c;
        }
        children.clear();
    }

    TreeNode<T>* add(T value) {
        children.push_back(new TreeNode<T>(value));
        children.back()->parent = this;
        return children.back();
    }

    typedef void (*Process)(T*);
};


template<typename T>
class Tree {
public:
    Tree() : _root(nullptr) {}

    Tree(T value ) : _root(new TreeNode<T>(value)) {}

    ~Tree() {
        if (_root != nullptr) {
            delete _root;
        }
    }

    TreeNode<T>* setRoot(T value) {
        if (_root != nullptr) {
            delete _root;
        }
        _root = new TreeNode<T>(value);
        return _root;
    }


    void iterate_DFS(std::function<void(TreeNode<T>*)> fun) {
        if (_root == nullptr) {
            return;
        }
        std::list<TreeNode<T>*> l {_root};
        while (!l.empty()) {
            auto current = l.front();
            l.pop_front();
            fun(current);
            // process current
            for (auto& child : current->children) {
                l.push_back(child);
            }
        }

    }

    TreeNode<T>* find(decltype(T::id) value) {
        std::list<TreeNode<T>*> l {_root};
        while (!l.empty()) {
            auto current = l.front();
            l.pop_front();
            if (current->data.id == value) {
                return current;
            }
            for (auto& child : current->children) {
                l.push_back(child);
            }
        }
    }

private:
    TreeNode<T>* _root;

};
#ifndef KEYS_H_
#define KEYS_H_

#include<vector>
#include<iterator>
#include<algorithm>
#include<assert.h>


template<class Node>
struct Branch
{
    int value;
    Node* left;
    Node* right;

    Branch(int v): value(v), left(nullptr), right(nullptr) {}
    Branch(int v, Node* left, Node* right): value(v), left(left), right(right) {}

    bool has_children() const noexcept
    {
        return (left != nullptr && right != nullptr);
    }

    operator int() const noexcept
    {
        return value;
    }
};

template <class Node>
class Keys
{
private:
    size_t degree;
    Node* owner;
    std::vector<int> keys;
    std::vector<Node*> children;

    Keys(size_t degree, Node* owner, std::vector<int> _keys): Keys(degree, owner)
    {
        keys = _keys;
    }

    Keys(size_t degree, Node* owner, std::vector<int> keys, std::vector<Node*> new_children): Keys(degree, owner, keys)
    {
        assert(keys.size() + 1 == new_children.size());

        children = new_children;
    }

public:

    Keys() {}
    Keys(size_t degree, Node* owner = nullptr): degree(degree), owner(owner) {}

    Branch<Node> get_branch(const size_t i) const
    {
        if (is_leaf())
        {
            return Branch<Node>(keys[i]);
        }

        return Branch<Node>(keys[i], get_child(i), get_child(i+1));
    }

    void add(const Branch<Node> k)
    {
        auto pos = get_pos_of_key(k);
        keys.insert(keys.begin() + pos, k);

        if (is_last_position(pos))
        {
            push_children_of_branch(k);
        }
        else
        {
            insert_children_of_branch_to_pos(k, pos);
        }
    }

    Node* select_node_for_key(const int k) const
    {
        return children[get_pos_of_key(k)];
    }

    bool is_present(const int k) const noexcept
    {
        return std::binary_search(keys.begin(), keys.end(), k);
    }

    int get_median_with_new_key(const int k) const
    {
        std::vector<int> tmp(keys);
        tmp.insert(tmp.begin() + get_pos_of_key(k), k);
        size_t middle = keys.size() / 2;

        return tmp[middle];
    }

    Branch<Node> get_first_branch() const noexcept
    {
        if (is_leaf())
        {
            return Branch<Node>(keys[0]);
        }

        return Branch<Node>(keys[0], get_child(0), get_child(1));
    }

    Branch<Node> get_last_branch() const noexcept
    {
        auto last_index = keys.size()-1;

        if (is_leaf())
        {
            return Branch<Node>(keys[last_index]);
        }

        return Branch<Node>(keys[last_index], get_child(last_index), get_child(last_index + 1));
    }

    std::vector<int>::iterator begin() noexcept
    {
        return keys.begin();
    }

    std::vector<int>::iterator end() noexcept
    {
        return keys.end();
    }

    size_t size() const noexcept
    {
        return keys.size();
    }

    std::vector<int> dump() const
    {
        return keys;
    }

    void clear() noexcept
    {
        keys.clear();
        children.clear();
    }

    bool is_leaf() const noexcept
    {
        return children.size() == 0;
    }

    typename std::vector<Node*>::iterator children_begin() noexcept
    {
        return children.begin();
    }

    typename std::vector<Node*>::iterator children_end() noexcept
    {
        return children.end();
    }

    Node* get_rightmost_child() const
    {
        return children[children.size()-1];
    }

    void set_owner(Node* new_owner) noexcept
    {
        owner = new_owner;

        for (Node* child : children)
        {
            child->parent = owner;
        }
    }

    Keys<Node> get_left_half_of_keys() const
    {
        size_t half = keys.size() / 2;
        auto left_keys = std::vector<int>(keys.begin(), keys.begin() + half);

        if (is_leaf())
        {
            return Keys<Node>(degree, owner, left_keys);
        }

        auto left_children = std::vector<Node*>(children.begin(), children.begin() + half + 1);

        return Keys<Node>(degree, owner, left_keys, left_children);

    }

    Keys<Node> get_right_half_of_keys() const
    {
        size_t half = keys.size() / 2;
        auto right_keys = std::vector<int>(keys.begin() + half, keys.end());

        if (is_leaf())
        {
            return Keys<Node>(degree, owner, right_keys);
        }

        auto right_children = std::vector<Node*>(children.begin() + half, children.end());

        return Keys<Node>(degree, owner, right_keys, right_children);
    }

    void change_first_to(const Branch<Node> new_first)
    {
        remove_first();
        add(new_first);
    }

    void change_last_to(const Branch<Node> new_last)
    {
        remove_last();
        add(new_last);
    }


private:
    bool is_last_position(const size_t i) const noexcept
    {
        return i + 1 >= keys.size();
    }

    size_t get_pos_of_key(const int k) const
    {
        return std::upper_bound(keys.begin(), keys.end(), k) - keys.begin();
    }

    Node* get_child(const size_t i) const noexcept
    {
        if (children.size() < i+1)
        {
            return nullptr;
        }

        return children[i];
    }

    void push_children_of_branch(Branch<Node> k)
    {
        if (!k.has_children())
        {
            return;
        }

        own_branch(k);

        if (children.size() == 0)
        {
            children.push_back(k.left);
        }
        else
        {
            children[children.size()-1] = k.left;
        }

        children.push_back(k.right);
    }

    void own_branch(Branch<Node> k)
    {
        k.left->parent = owner;
        k.right->parent = owner;
    }

    void insert_children_of_branch_to_pos(Branch<Node> k, const size_t pos)
    {
        if (!k.has_children())
        {
            return;
        }

        own_branch(k);

        children.insert(children.begin() + pos, k.left);
        children[pos+1] = k.right;
    }

    void remove_first()
    {
        keys.erase(keys.begin());

        if (is_leaf())
        {
            return;
        }

        children.erase(children.begin());
    }

    void remove_last()
    {
        keys.erase(keys.end() - 1);

        if (is_leaf())
        {
            return;
        }

        children.erase(children.end() - 1);
    }
};

#endif

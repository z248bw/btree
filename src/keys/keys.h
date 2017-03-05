#ifndef KEYS_H_
#define KEYS_H_

#include<vector>
#include<iterator>
#include<algorithm>
#include<assert.h>

template<typename KEY, typename VALUE>
struct KeyValue
{
    KEY k;
    VALUE v;

    KeyValue(KEY k, VALUE v): k(k), v(v) {}

    bool operator<(const KeyValue & other) const
    {
        return k < other.k;
    }

    operator KEY() const
    {
        return k;
    }
};


template<class Node>
struct Branch
{
    using KV = KeyValue<typename Node::key_t, typename Node::value_t>;

    KV value;
    Node* left;
    Node* right;

    Branch(KV v): value(v), left(nullptr), right(nullptr) {}
    Branch(KV v, Node* left, Node* right): value(v), left(left), right(right) {}

    bool has_children() const noexcept
    {
        return (left != nullptr && right != nullptr);
    }
};

template <class Node>
class Keys
{
private:
    using KV = KeyValue<typename Node::key_t, typename Node::value_t>;

    size_t degree;
    Node* owner;
    std::vector<KV> keyvalues;
    std::vector<Node*> children;

    Keys(size_t degree, Node* owner, std::vector<KV> _keyvalues): Keys(degree, owner)
    {
        keyvalues = _keyvalues;
        children.reserve(degree + 1);
    }

    Keys(size_t degree, Node* owner, std::vector<KV> keyvalues, std::vector<Node*> new_children)
        : Keys(degree, owner, keyvalues)
    {
        assert(keyvalues.size() + 1 == new_children.size());

        children = new_children;
    }

public:
    Keys() = default;

    Keys(size_t degree, Node* owner = nullptr): degree(degree), owner(owner)
    {
        keyvalues.reserve(degree);
        children.reserve(degree + 1);
    }

    Branch<Node> get_branch(const size_t i) const
    {
        if (is_leaf())
        {
            return Branch<Node>(keyvalues[i]);
        }

        return Branch<Node>(keyvalues[i], get_child(i), get_child(i+1));
    }

    void add(const Branch<Node> k)
    {
        auto pos = get_pos_of_key(k.value);
        keyvalues.insert(keyvalues.begin() + pos, k.value);

        if (is_last_position(pos))
        {
            push_children_of_branch(k);
        }
        else
        {
            insert_children_of_branch_to_pos(k, pos);
        }
    }

    Node* select_node_for_key(const typename Node::key_t k) const
    {
        return children[get_pos_of_key(k)];
    }

    bool is_present(const typename Node::key_t k) const noexcept
    {
        return std::binary_search(keyvalues.begin(), keyvalues.end(), k);
    }

    KV get_median_with_new_key(const KV k) const
    {
        std::vector<KV> tmp(keyvalues.begin(), keyvalues.end());
        tmp.insert(tmp.begin() + get_pos_of_key(k), k);
        size_t middle = keyvalues.size() / 2;

        return tmp[middle];
    }

    Branch<Node> get_first_branch() const noexcept
    {
        if (is_leaf())
        {
            return Branch<Node>(keyvalues[0]);
        }

        return Branch<Node>(keyvalues[0], get_child(0), get_child(1));
    }

    Branch<Node> get_last_branch() const noexcept
    {
        auto last_index = keyvalues.size()-1;

        if (is_leaf())
        {
            return Branch<Node>(keyvalues[last_index]);
        }

        return Branch<Node>(keyvalues[last_index], get_child(last_index), get_child(last_index + 1));
    }

    size_t size() const noexcept
    {
        return keyvalues.size();
    }

    std::vector<KV> dump() const
    {
        return keyvalues;
    }

    void clear() noexcept
    {
        keyvalues.clear();
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
        size_t half = keyvalues.size() / 2;
        auto left_keyvalues = std::vector<KV>(keyvalues.begin(), keyvalues.begin() + half);

        if (is_leaf())
        {
            return Keys<Node>(degree, owner, left_keyvalues);
        }

        auto left_children = std::vector<Node*>(children.begin(), children.begin() + half + 1);

        return Keys<Node>(degree, owner, left_keyvalues, left_children);
    }

    Keys<Node> get_right_half_of_keys() const
    {
        size_t half = keyvalues.size() / 2;
        auto right_keyvalues = std::vector<KV>(keyvalues.begin() + half, keyvalues.end());

        if (is_leaf())
        {
            return Keys<Node>(degree, owner, right_keyvalues);
        }

        auto right_children = std::vector<Node*>(children.begin() + half, children.end());

        return Keys<Node>(degree, owner, right_keyvalues, right_children);
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
        return i + 1 >= keyvalues.size();
    }

    size_t get_pos_of_key(const typename Node::key_t  k) const
    {
        return std::upper_bound(keyvalues.begin(), keyvalues.end(), k) - keyvalues.begin();
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
        keyvalues.erase(keyvalues.begin());

        if (is_leaf())
        {
            return;
        }

        children.erase(children.begin());
    }

    void remove_last()
    {
        keyvalues.erase(keyvalues.end() - 1);

        if (is_leaf())
        {
            return;
        }

        children.erase(children.end() - 1);
    }
};

#endif

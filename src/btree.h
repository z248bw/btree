#ifndef BTREE_H_
#define BTREE_H_

#include<vector>
#include<algorithm>

#define MAX_KEY_SIZE 2
#define MAX_INT 99999

// I want to be able to simply verify whether the tree is balanced.
// A way to do is to find the deepest and the shallowest leaf and then check
// if they are equal. I provide this functionality as a mixin because I need to
// prove that the walk algorithm works correctly and for this I need to be able
// to walk unbalanced trees as well.
// Using a mixin like this is not cheating. The btree promises that the shallowest
// and the deepest leaf's depth is equal, and thus it should provide a way to check that.
class Traversable
{
protected:
    virtual Traversable* get_child(const size_t i) = 0;
    virtual size_t max_branches_num() = 0;

public:
    static size_t deepest;
    static size_t shallowest;
    virtual ~Traversable(){}

    void walk(Traversable* t = nullptr, size_t depth = 0)
    {
        if (t == nullptr)
        {
            init_walk();
            t = this;
        }

        size_t children_num = 0;
        Traversable* next = t->get_child(children_num);
        while (next != nullptr)
        {
            walk(next, depth+1);
            next = t->get_child(++children_num);
        }

        if (depth > deepest)
        {
            deepest = depth;
        }

        // A node is a leaf in a btree if it does not have the maximum number
        // of children. Altough the class's name is Traversable, I use this
        // rule because I want to use it for btrees. The other implementations of this class
        // sole purposes are to test that the walk algorithm works in the defined way
        if (children_num < t->max_branches_num() && depth < shallowest)
        {
            shallowest = depth;
        }
    }

private:
    void init_walk()
    {
        deepest = 0;
        shallowest = MAX_INT;
    }

};

struct TraversableTree : public Traversable
{
    TraversableTree* l;
    TraversableTree* r;

    TraversableTree(): l(nullptr), r(nullptr) {}

    Traversable* get_child(const size_t i)
    {
        if (i == 0)
        {
            return l;
        }
        else if (i == 1)
        {
            return r;
        }

        return nullptr;
    }

    size_t max_branches_num()
    {
        return 2;
    }

    ~TraversableTree()
    {
        delete l;
        delete r;
    }
};

class Btree : public Traversable
{
private:
    using Nodes = std::vector<Btree*>;
    using Keys = std::vector<int>;

    Btree* parent;
    Keys keys;
    Nodes children;

public:
    Btree(Btree* parent = nullptr): parent(parent) {}

    Btree(int k)
    {
        keys.push_back(k);
    }

    Btree(Keys keys, Nodes children):
        keys(keys), children(children) {}

    virtual void add(int k)
    {
        auto n = find_node_for_key(this, k);
        // element fits into the right node
        if (n->keys.size() < MAX_KEY_SIZE)
        {
            n->keys.push_back(k);
            std::sort(n->keys.begin(), n->keys.end());
        }
        // n is root
        else if (n->parent == nullptr)
        {
            auto left = new Btree(n->keys[0]);
            auto median = n->keys[MAX_KEY_SIZE-1];
            auto right = new Btree(k);
            n->grow_tree(left, median, right);
        }
        // n is node
        else
        {
            auto left = new Btree(n->keys[0]);
            auto median = n->keys[1];
            auto right = new Btree(k);
            n->push_to_parent(left, median, right);
            n->remove_node();
        }
    }

    Keys dump(Btree* n = nullptr, Keys result = Keys())
    {
        if (n == nullptr)
        {
            n = this;
        }

        if (n->children.size() > 0)
        {
            for (size_t i = 0; i < n->keys.size(); i++)
            {
                result = dump(n->children[i], result);
                result.push_back(n->keys[i]);
            }
        }
        else
        {
            result.insert(result.end(), n->keys.begin(), n->keys.end());
        }

        if (n->children.size() > 0)
        {
            result = dump(n->children[n->children.size()-1], result);
        }

        return result;
    }

    Btree* find(int k, Btree* n = nullptr)
    {
        if (n == nullptr)
        {
            n = this;
        }

        if (std::binary_search(n->keys.begin(), n->keys.end(), k))
        {
            return n;
        }

        for (auto child : n->children)
        {
            auto result = find(k, child);
            if (result != nullptr)
            {
                return result;
            }
        }

        return nullptr;
    }

    Keys get_keys()
    {
        return keys;
    }

    Traversable* get_child(const size_t i)
    {
        if (i < children.size())
        {
            return children[i];
        }

        return nullptr;
    }

    size_t max_branches_num()
    {
        return keys.size() + 1;
    }

    ~Btree()
    {
        for (auto child : children)
        {
            delete child;
        }
    }

private:

    void grow_tree(Btree* left, int median, Btree* right)
    {
        keys.clear();
        children.clear();
        keys.push_back(median);
        add_child(left);
        add_child(right);
    }

    Btree* find_node_for_key(Btree* n, int k)
    {
        // start inorder walk of the tree
        for (size_t i = 0; i < n->keys.size(); i++)
        {
            if (k < n->keys[i])
            {
                if (n->children.size() > 0)
                {
                    return find_node_for_key(n->children[i], k);
                }

                // In case the current node n is a leaf node because it does not have any children
                return n;
            }
        }

        if (n->children.size() > 0)
        {
            return find_node_for_key(n->children[n->children.size()-1], k);
        }

        // We are at the node containing the biggest element in the tree
        // and k is even bigger than that
        return n;
    }

    void remove_node()
    {
        children.clear();
        delete this;
    }

    void push_to_parent(Btree* left, int k, Btree* right)
    {
        auto pos = parent->find_pos_of_key_in_keys(k);
        parent->insert_key(pos, k);
        parent->add_child(pos, left);
        parent->add_child(pos+1, right);
    }

    void add_child(size_t i, Btree* child)
    {
        if (i < children.size())
        {
            children[i] = child;
        }
        else
        {
            children.push_back(child);
        }

        child->parent = this;
    }

    void add_child(Btree* child)
    {
        children.push_back(child);
        child->parent = this;
    }

    void insert_key(size_t i, int k)
    {
        if (i < keys.size())
        {
            keys.insert(keys.begin() + i, k);
        }
        else
        {
            keys.push_back(k);
        }
    }

    size_t find_pos_of_key_in_keys(int k)
    {
        for (size_t i = 0; i < keys.size(); i++)
        {
            if (k < keys[i])
            {
                return i;
            }
        }

        if (keys.size() < MAX_KEY_SIZE)
        {
            return keys.size();
        }

        return -1;
    }
};
#endif

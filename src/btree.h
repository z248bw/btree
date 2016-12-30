#ifndef BTREE_H_
#define BTREE_H_

#include<vector>
#include<iterator>
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
    virtual Traversable* first_child() = 0;
    virtual bool is_leaf() = 0;

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

        auto child = t->first_child();
        while (child = t->next_child())
        {
            walk(child, depth+1);
        }

        if (depth > deepest)
        {
            deepest = depth;
        }

        // A node is a leaf in a btree if it does not have the maximum number
        // of children. Altough the class's name is Traversable, I use this
        // rule because I want to use it for btrees. The other implementations of this class
        // sole purposes are to test that the walk algorithm works in the defined way
        if (t->is_leaf() && depth < shallowest)
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

    virtual TraversableIterator children_begin() override
    {
        auto v = std::vector<Traversable*>{l, r};

        return v.begin();
    }

    virtual TraversableIterator children_end() override
    {
        auto v = std::vector<Traversable*>{l, r};

        return v.end();
    }

    virtual bool is_leaf() override
    {
        return l == nullptr || r == nullptr;
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
    class Keys
    {
    private:
        std::vector<int> keys;
        std::vector<Btree*> children;

    public:
        int get(size_t i)
        {
            return keys[i];
        }

        void add(int k)
        {
            keys.push_back(k);
            std::sort(keys.begin(), keys.end());
        }

        void set(size_t i, int k)
        {
            if (i < keys.size())
            {
                keys[i] = k;
            }
            else
            {
                add(k);
            }
        }

        void insert(size_t i, int k)
        {
            if (i < keys.size())
            {
                keys.insert(keys.begin() + i, k);
            }
            else
            {
                add(k);
            }
        }

        bool is_present(int k)
        {
            if (std::binary_search(keys.begin(), keys.end(), k))
            {
                return true;
            }

            return false;
        }

        int get_first()
        {
            return keys[0];
        }

        int get_last()
        {
            return keys[keys.size()-1];
        }

        std::vector<int>::iterator begin()
        {
            return keys.begin();
        }

        std::vector<int>::iterator end()
        {
            return keys.end();
        }

        size_t size()
        {
            return keys.size();
        }

        std::vector<int> dump()
        {
            return keys;
        }

        void clear()
        {
            keys.clear();
            children.clear();
        }

        bool is_leaf()
        {
            return children.size() == 0;
        }

        std::vector<Btree*>::iterator children_begin()
        {
            return children.begin();
        }

        std::vector<Btree*>::iterator children_end()
        {
            return children.end();
        }

        void set_left_child_for_key(int k, Btree* child)
        {
            children[get_pos_of_key(k)] = child;
        }

        void set_right_child_for_key(int k, Btree* child)
        {
            children[get_pos_of_key(k)+1] = child;
        }

        Btree* get_left_child_of_key(int k)
        {
            return children[get_pos_of_key(k)];
        }

        Btree* get_right_child_of_key(int k)
        {
            return children[get_pos_of_key(k)+1];
        }

        Btree* get_rightmost_child()
        {
            return children[children.size()-1];
        }

    private:
        size_t get_pos_of_key(int k)
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

    Btree* parent;
    Keys keys;

public:
    Btree(Btree* parent = nullptr): parent(parent) {}

    Btree(int k)
    {
        keys.add(k);
    }

    void add(int k)
    {
        auto n = find_node_for_key(this, k);
        // element fits into the right node
        if (n->keys.size() < MAX_KEY_SIZE)
        {
            n->keys.add(k);
        }
        else
        {
            auto left = new Btree(n->keys.get(0));
            auto median = n->keys.get(1);
            auto right = new Btree(k);
            // n is root
            if (n->parent == nullptr)
            {
                n->grow_tree(left, median, right);
            }
            // n is node
            else
            {
                n->push_to_parent(left, median, right);
                n->remove_node();
            }
        }
    }

    std::vector<int> dump(Btree* n = nullptr, std::vector<int> result = std::vector<int>())
    {
        if (n == nullptr)
        {
            n = this;
        }

        // n is a node
        if (!n->keys.is_leaf())
        {
            for (size_t i = 0; i < n->keys.size(); i++)
            {
                result = dump(n->keys.get_left_child_of_key(i), result);
                result.push_back(n->keys.get(i));
            }
        }
        // n is a leaf
        else
        {
            result.insert(result.end(), n->keys.begin(), n->keys.end());
        }

        // walk the last child of the node
        if (!n->keys.is_leaf())
        {
            result = dump(n->keys.get_rightmost_child(), result);
        }

        return result;
    }

    Btree* find(int k, Btree* n = nullptr)
    {
        if (n == nullptr)
        {
            n = this;
        }

        if (n->keys.is_present(k))
        {
            return n;
        }

        for (auto it = n->keys.children_begin(); it != n->keys.children_end(); it++)
        {
            auto result = find(k, *it);
            if (result != nullptr)
            {
                return result;
            }
        }

        return nullptr;
    }

    std::vector<int> get_keys()
    {
        return keys.dump();
    }

    virtual bool is_leaf() override
    {
        return keys.is_leaf();
    }

    ~Btree()
    {
        for (auto child = keys.children_begin(); child != keys.children_end(); child++)
        {
            delete child;
        }
    }

private:

    void grow_tree(Btree* left, int median, Btree* right)
    {
        keys.clear();
        keys.add(median);
        keys.add_left_child_for_key(median, left);
        keys.add_right_child_for_key(median, right);
    }

    Btree* find_node_for_key(Btree* n, int k)
    {
        if (n == nullptr)
        {
            return nullptr;
        }

        if (n->keys.get_first() < k && k < n->keys.get_last())
        {
            return n;
        }

        auto child = n->keys.children_begin();
        while (n->keys.children_end())
        {
            auto result = find_node_for_key(child++, k);
            if (result != nullptr)
            {
                return result;
            }
        }

        // k is bigger than all other keys in the tree
        return child;
    }

    void remove_node()
    {
        delete this;
    }

    void push_to_parent(Btree* left, int k, Btree* right)
    {
        parent->keys.insert(k);
        parent->keys.add_left_child_for(k, left);
        parent->keys.add_right_child_for(k, right);
    }
};
#endif

#ifndef BTREE_H_
#define BTREE_H_

#include<vector>
#include<iterator>
#include<algorithm>
#include<functional>

#define MAX_KEY_SIZE 2
#define MAX_INT 99999


// I want to be able to simply verify whether the tree is balanced.
// A way to do this is, to find the deepest and the shallowest leaf and then check
// if they are equal. I provide this functionality as a mixin because I need to
// prove that the walk algorithm works correctly and for this I need to be able
// to walk unbalanced trees as well.
class Traversable
{
protected:
    virtual void on_children(std::function<void(Traversable*)> on_child) = 0;
    virtual bool is_leaf() = 0;
    virtual std::vector<Traversable*> get_children() = 0;

public:
    static size_t deepest;
    static size_t shallowest;
    virtual ~Traversable(){}

    void walk(size_t depth = 0)
    {
        if (depth == 0)
        {
            init_walk();
        }

        auto children = get_children();
        for (auto child : children)
        {
             child->walk(depth+1);
        }

        //on_children([t, depth] (Traversable* child) {
             //t->walk(child, depth+1);
        //});

        if (depth > deepest)
        {
            deepest = depth;
        }

        // A node is a leaf in a btree if it does not have the maximum number
        // of children. Altough the class's name is Traversable, I use this
        // rule because I want to use it for btrees. The other implementations of this class
        // sole purposes are to test that the walk algorithm works in the defined way
        if (is_leaf() && depth < shallowest)
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

class TraversableTree : public Traversable
{
protected:
    virtual std::vector<Traversable*> get_children() override
    {
        auto vector = std::vector<Traversable*>();
        if (l != nullptr)
        {
            vector.push_back(l);
        }
        if (r != nullptr)
        {
            vector.push_back(r);
        }

        return vector;
    }

    virtual void on_children(std::function<void(Traversable*)> on_child) override
    {
        if (l != nullptr)
        {
            on_child(l);
        }

        if (r != nullptr)
        {
            on_child(r);
        }
    }

    virtual bool is_leaf() override
    {
        return l == nullptr || r == nullptr;
    }

public:
    TraversableTree* l;
    TraversableTree* r;

    Traversable* current;

    TraversableTree(): l(nullptr), r(nullptr) {}

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
        Btree* owner;
        std::vector<int> keys;
        std::vector<Btree*> children;

    public:
        Keys(Btree* owner): owner(owner) {}
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

        void insert(int k)
        {
            keys.insert(keys.begin() + get_pos_of_key(k), k);
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
            if (child == nullptr)
            {
                return;
            }

            child->parent = owner;

            if (children.size() == 0)
            {
                children.push_back(child);

                return;
            }

            children[get_pos_of_key(k)] = child;
        }

        void set_right_child_for_key(int k, Btree* child)
        {
            if (child == nullptr)
            {
                return;
            }

            child->parent = owner;

            auto i = get_pos_of_key(k)+1;
            if (children.size() < i+1)
            {
                children.push_back(child);

                return;
            }

            children[i] = child;
        }

        Btree* get_left_child_of_key(int k)
        {
            auto i = get_pos_of_key(k);
            if (children.size() == 0 || children.size() < i)
            {
                return nullptr;
            }

            return children[i];
        }

        Btree* get_right_child_of_key(int k)
        {
            auto i = get_pos_of_key(k)+1;
            if (children.size() < i)
            {
                return nullptr;
            }

            return children[i];
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
                if (k <= keys[i])
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

    Btree* parent = nullptr;
    Keys keys = nullptr;

protected:
    virtual void on_children(std::function<void(Traversable*)> on_child) override
    {
        for (auto it = keys.children_begin(); it != keys.children_end(); it++)
        {
            on_child(*it);
        }
    }

    virtual bool is_leaf() override
    {
        return keys.is_leaf();
    }

    virtual std::vector<Traversable*> get_children() override
    {
        return std::vector<Traversable*>(keys.children_begin(), keys.children_end());
    }

public:
    Btree(Btree* parent = nullptr): parent(parent)
    {
        keys = Keys(this);
    }

    Btree(int k)
    {
        keys.add(k);
    }

    Btree(Btree* left, int median, Btree* right)
    {
        keys.insert(median);
        keys.set_left_child_for_key(median, left);
        keys.set_right_child_for_key(median, right);
    }

    void add(int k)
    {
        auto n = get_node_for_key(k);
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
            auto new_branch = new Btree(left, median, right);

            if (n->parent == nullptr)
            {
                n->keys.clear();
                n->insert(new_branch);
            }
            else
            {
                n->upwards_add(new_branch);
            }
        }
    }

    void upwards_add(Btree* k)
    {
        if (keys.size() < MAX_KEY_SIZE)
        {
            insert(k);
        }
        else if (parent == nullptr)
        {
            int left_key = keys.get(0);
            auto left = new Btree(
                keys.get_left_child_of_key(left_key),
                left_key,
                keys.get_right_child_of_key(left_key)
            );
            auto median = keys.get(1);
            auto right = k;
            keys.clear();
            insert(new Btree(left, median, right));
        }
        else
        {
            push_to_parent(k);
        }
    }

    std::vector<int> dump(std::vector<int> result = std::vector<int>())
    {
        if (!keys.is_leaf())
        {
            for (size_t i = 0; i < keys.size(); i++)
            {
                result = keys.get_left_child_of_key(keys.get(i))->dump(result);
                result.push_back(keys.get(i));
            }

            result = keys.get_rightmost_child()->dump(result);
        }
        else
        {
            result.insert(result.end(), keys.begin(), keys.end());
        }

        return result;
    }

    Btree* find(int k)
    {
        if (keys.is_present(k))
        {
            return this;
        }

        for (auto it = keys.children_begin(); it != keys.children_end(); it++)
        {
            auto result = (*it)->find(k);
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

    ~Btree()
    {
        for (auto it = keys.children_begin(); it != keys.children_end(); it++)
        {
            delete *it;
        }
    }

private:

    Btree* get_node_for_key(int k)
    {
        auto n = find_node_for_key(k);
        if (n == nullptr)
        {
            return get_largest_child();
        }

        return n;
    }

    // TODO: could be generalized look for find method
    Btree* find_node_for_key(int k)
    {
        if (keys.size() > 0 && keys.get_first() < k && k < keys.get_last())
        {
            return this;
        }

        for (auto child = keys.children_begin(); child != keys.children_end(); child++)
        {
            auto result = (*child)->find_node_for_key(k);
            if (result != nullptr)
            {
                return result;
            }
        }

        // k is bigger than all other keys in the tree
        return nullptr;
    }

    Btree* get_largest_child()
    {
        // if current is the root without any keys
        if (keys.size() == 0)
        {
            return this;
        }

        auto largest_child = keys.get_right_child_of_key(keys.get_last());
        if (largest_child == nullptr)
        {
            return this;
        }

        return largest_child->get_largest_child();
    }

    void remove_node()
    {
        delete this;
    }

    void insert(Btree* k)
    {
        auto median = k->keys.get_first();
        keys.insert(median);
        keys.set_left_child_for_key(median, k->keys.get_left_child_of_key(median));
        keys.set_right_child_for_key(median, k->keys.get_right_child_of_key(median));
        k->keys.clear();
        k->remove_node();
    }

    void push_to_parent(Btree* k)
    {
        if (parent->keys.size() < MAX_KEY_SIZE)
        {
            parent->insert(k);
        }
        else
        {
            parent->upwards_add(k);
        }
    }
};
#endif

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
    struct Key
    {
        int value;
        Btree* left;
        Btree* right;

        Key(int v): value(v), left(nullptr), right(nullptr) {}
        Key(int v, Btree* left, Btree* right): value(v), left(left), right(right) {}

        operator int() const
        {
            return value;
        }
    };

    class Keys
    {
    private:
        Btree* owner;
        std::vector<int> keys;
        std::vector<Btree*> children;

    public:
        Keys(Btree* owner): owner(owner) {}

        Key get_key(size_t i)
        {
            return Key(keys[i], get_child(i), get_child(i+1));
        }

        void add(Key k)
        {
            auto pos = get_pos_of_key(k);
            keys.insert(keys.begin() + pos, k);
            set_children_of_key(k);
        }

        bool is_present(int k)
        {
            if (std::binary_search(keys.begin(), keys.end(), k))
            {
                return true;
            }

            return false;
        }

        int get_median_with_new_key(int k)
        {
            std::vector<int> tmp(keys);
            tmp.push_back(k);
            std::sort(tmp.begin(), tmp.end());

            return tmp[1];
        }

        Key get_first_key()
        {
            return Key(keys[0], get_child(0), get_child(1));
        }

        Key get_last_key()
        {
            auto last_index = keys.size()-1;

            return Key(keys[last_index], get_child(last_index), get_child(last_index + 1));
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

            //TODO
            // should never happern
            return -1;
        }

        Btree* get_child(size_t i)
        {
            if (children.size() < i+1)
            {
                return nullptr;
            }

            return children[i];
        }

        void set_children_of_key(Key k)
        {
            if (k.left == nullptr || k.right == nullptr)
            {
                return;
            }

            k.left->parent = owner;
            k.right->parent = owner;

            size_t pos = get_pos_of_key(k.value);
            size_t children_size = children.size();

            if (children_size == 0)
            {
                children.push_back(k.left);
            }
            else
            {
                children[pos] = k.left;
            }

            if (children_size < pos + 2)
            {
                children.push_back(k.right);
            }
            else
            {
                children[pos + 1] = k.right;
            }
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

    Btree(int k): Btree()
    {
        keys.add(Key(k));
    }

    Btree(Key k): Btree()
    {
        keys.add(k);
    }

    void add(int k)
    {
        auto n = get_leaf_for_key(k);
        if (n->keys.size() < MAX_KEY_SIZE)
        {
            n->keys.add(Key(k));
        }
        else
        {
            n->upwards_add(new Btree(k));
        }
    }

    std::vector<int> dump(std::vector<int> result = std::vector<int>())
    {
        if (!keys.is_leaf())
        {
            for (size_t i = 0; i < keys.size(); i++)
            {
                Key k = keys.get_key(i);
                result = k.left->dump(result);
                result.push_back(k.value);
            }

            result = keys.get_rightmost_child()->dump(result);
        }
        else
        {
            result.insert(result.end(), keys.begin(), keys.end());
        }

        return result;
    }

    Btree* find_node_with_key(int k)
    {
        if (keys.is_present(k))
        {
            return this;
        }

        for (auto it = keys.children_begin(); it != keys.children_end(); it++)
        {
            auto result = (*it)->find_node_with_key(k);
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
    void grow(Btree* new_root)
    {
        keys.clear();
        insert(new_root);
    }

    void upwards_add(Btree* branch)
    {
        if (keys.size() < MAX_KEY_SIZE)
        {
            insert(branch);

            return;
        }

        if (parent == nullptr)
        {
            grow(seperate_current_for_unfitting(branch));
        }
        else
        {
            parent->upwards_add(seperate_current_for_unfitting(branch));

            remove_node();
        }
    }

    Btree* seperate_current_for_unfitting(Btree* unfitting)
    {
        auto unfitting_key = unfitting->keys.get_first_key();
        auto median = keys.get_median_with_new_key(unfitting_key.value);
        Key seperated(median);

        if (median == unfitting_key.value)
        {
            seperated.left = new Btree(keys.get_key(0));
            seperated.right = new Btree(keys.get_key(1));

            seperated.left->insert(unfitting_key.left);
            seperated.right->insert(unfitting_key.right);
        }
        else
        {

            if (median < unfitting_key.value)
            {
                seperated.left = new Btree(keys.get_key(0));
                seperated.right = unfitting;
            }
            else
            {
                seperated.left = unfitting;
                seperated.right = new Btree(keys.get_key(1));
            }
        }

        return new Btree(seperated);
    }

    Btree* get_leaf_for_key(int k)
    {
        if (keys.is_leaf())
        {
            return this;
        }

        return select_branch_for_key(k)->get_leaf_for_key(k);
    }

    Btree* select_branch_for_key(int new_key)
    {
        Btree* branch_for_key = keys.get_first_key().left;

        for (size_t i = 0; i < keys.size(); i++)
        {
            auto key = keys.get_key(i);
            if (key.value < new_key)
            {
                branch_for_key = key.right;
            }
        }

        return branch_for_key;
    }

    void remove_node()
    {
        keys.clear();
        delete this;
    }

    void insert(Btree* k)
    {
        keys.add(k->keys.get_first_key());
        k->remove_node();
    }
};
#endif

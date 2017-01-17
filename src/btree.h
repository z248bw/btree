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

        void set_children_of_key(int k, Btree* left, Btree* right)
        {
            if (left == nullptr || right == nullptr)
            {
                return;
            }

            left->parent = owner;
            right->parent = owner;

            size_t pos = get_pos_of_key(k);
            size_t children_size = children.size();

            if (children_size == 0)
            {
                children.push_back(left);
            }
            else
            {
                children[pos] = left;
            }

            if (children_size < pos + 2)
            {
                children.push_back(right);
            }
            else
            {
                children[pos + 1] = right;
            }
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

            //TODO
            // should never happern
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

    Btree(int k): Btree()
    {
        keys.add(k);
    }

    Btree(Btree* left, int median, Btree* right): Btree()
    {
        keys.add(median);
        keys.set_children_of_key(median, left, right);
    }

    void add(int k)
    {
        auto n = get_leaf_for_key(k);
        if (n->keys.size() < MAX_KEY_SIZE)
        {
            n->keys.add(k);
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
        auto unfitting_key = unfitting->keys.get_first();
        auto median = keys.get_median_with_new_key(unfitting_key);
        Btree* left;
        Btree* right;

        if (median == unfitting_key)
        {
            left = key_to_branch(0);
            right = key_to_branch(1);

            // because key_to_branch always transforms a new branch wih 1 key
            // the insertion to left and right will not be recursive
            left->insert(unfitting->keys.get_left_child_of_key(unfitting_key));
            right->insert(unfitting->keys.get_right_child_of_key(unfitting_key));
        }
        else
        {

            if (median < unfitting_key)
            {
                left = key_to_branch(0);
                right = unfitting;
            }
            else
            {
                left = unfitting;
                right = key_to_branch(1);
            }
        }

        return new Btree(left, median, right);
    }

    Btree* key_to_branch(int i)
    {
        int k = keys.get(i);
        return new Btree(
            keys.get_left_child_of_key(k),
            k,
            keys.get_right_child_of_key(k)
        );
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
        auto ks = keys.dump();
        Btree* branch_for_key = keys.get_left_child_of_key(keys.get_first());

        for (auto key : ks)
        {
            if (key < new_key)
            {
                branch_for_key = keys.get_right_child_of_key(key);
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
        auto median = k->keys.get_first();
        keys.add(median);
        auto left = k->keys.get_left_child_of_key(median);
        auto right = k->keys.get_right_child_of_key(median);
        keys.set_children_of_key(median, left, right);
        k->remove_node();
    }
};
#endif

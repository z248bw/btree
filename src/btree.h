#ifndef BTREE_H_
#define BTREE_H_

#include<vector>
#include<iterator>
#include<algorithm>
#include<functional>
#include<assert.h>

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

class invalid_key_exception: public std::exception
{
public:
    const char* message;

    invalid_key_exception(const char* message):
        std::exception(), message(message){}

    virtual const char* what() const noexcept override
    {
        return message;
    }
};

template <size_t degree>
class Btree : public Traversable
{
private:
    struct Branch
    {
        int value;
        Btree* left;
        Btree* right;

        Branch(int v): value(v), left(nullptr), right(nullptr) {}
        Branch(int v, Btree* left, Btree* right): value(v), left(left), right(right) {}
        Branch(Btree* b)
        {
            assert(b->keys.size() == 1);

            auto branch = b->keys.get_branch(0);
            value = branch.value;
            left = branch.left;
            right = branch.right;
        }

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

        Keys(Btree* owner, std::vector<int> keys): owner(owner), keys(keys) {}

        Keys(Btree* owner, std::vector<int> keys, std::vector<Btree*> new_children): Keys(owner, keys)
        {
            assert(keys.size() + 1 == new_children.size());

            children = new_children;
        }

    public:

        Keys(Btree* owner): owner(owner) {}

        Branch get_branch(size_t i)
        {
            if (is_leaf())
            {
                return Branch(keys[i]);
            }

            return Branch(keys[i], get_child(i), get_child(i+1));
        }

        void add(Branch k)
        {
            auto pos = get_pos_of_key(k);
            keys.insert(keys.begin() + pos, k);

            if (pos+1 < keys.size())
            {
                insert_children(pos, k);
            }
            else
            {
                set_children(pos, k);
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

        int get_median_with_new_key(int k)
        {
            std::vector<int> tmp(keys);
            tmp.push_back(k);
            std::sort(tmp.begin(), tmp.end());

            return tmp[1];
        }

        Branch get_first_branch()
        {
            return Branch(keys[0], get_child(0), get_child(1));
        }

        Branch get_last_branch()
        {
            auto last_index = keys.size()-1;

            return Branch(keys[last_index], get_child(last_index), get_child(last_index + 1));
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

        typename std::vector<Btree*>::iterator children_begin()
        {
            return children.begin();
        }

        typename std::vector<Btree*>::iterator children_end()
        {
            return children.end();
        }

        Btree* get_rightmost_child()
        {
            return children[children.size()-1];
        }

        void set_owner(Btree* new_owner)
        {
            owner = new_owner;

            for (Btree* child : children)
            {
                child->parent = owner;
            }
        }

        Keys get_left_half_of_keys()
        {
            size_t half = keys.size() / 2;
            auto left_keys = std::vector<int>(keys.begin(), keys.begin() + half);

            if (is_leaf())
            {
                return Keys(owner, left_keys);
            }

            auto left_children = std::vector<Btree*>(children.begin(), children.begin() + half + 1);

            return Keys(owner, left_keys, left_children);

        }

        Keys get_right_half_of_keys()
        {
            size_t half = keys.size() / 2;
            auto right_keys = std::vector<int>(keys.begin() + half, keys.end());

            if (is_leaf())
            {
                return Keys(owner, right_keys);
            }

            auto right_children = std::vector<Btree*>(children.begin() + half, children.end());

            return Keys(owner, right_keys, right_children);
        }

        void remove(Branch b)
        {
            auto pos = get_pos_of_key(b.value);

            keys.erase(keys.begin() + pos);

            if (is_leaf())
            {
                return;
            }

            if ( pos == 0)
            {
                children.erase(children.begin());
            }

            if ( pos + 2 > children.size())
            {
                children.erase(children.end() - 1);
            }
        }

        void drop_keys_less_than(int k)
        {
            auto pos = get_pos_of_key(k);
            auto keys_to_be_removed = std::vector<int>(keys.begin(), keys.begin() + pos + 1);

            remove(keys_to_be_removed);
        }

        void drop_keys_bigger_than(int k)
        {
            auto pos = get_pos_of_key(k);
            auto keys_to_be_removed = std::vector<int>(keys.begin() + pos, keys.end());

            remove(keys_to_be_removed);
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

            assert(keys.size() < degree);

            return keys.size();
        }

        Btree* get_child(size_t i)
        {
            if (children.size() < i+1)
            {
                return nullptr;
            }

            return children[i];
        }

        void set_children(size_t pos, Branch k)
        {
            if (k.left == nullptr || k.right == nullptr)
            {
                return;
            }

            k.left->parent = owner;
            k.right->parent = owner;

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

        void insert_children(size_t pos, Branch k)
        {
            if (k.left == nullptr || k.right == nullptr)
            {
                return;
            }

            k.left->parent = owner;
            k.right->parent = owner;

            children.insert(children.begin() + pos, k.left);
            children[pos+1] = k.right;
        }

        void remove(std::vector<int> keys_to_be_removed)
        {
            for (auto k : keys_to_be_removed)
            {
                remove(k);
            }
        }
    };

    Btree* parent = nullptr;
    Keys keys = nullptr;

protected:
    virtual bool is_leaf() override
    {
        return keys.is_leaf();
    }

    virtual std::vector<Traversable*> get_children() override
    {
        return std::vector<Traversable*>(keys.children_begin(), keys.children_end());
    }

public:
    Btree()
    {
        keys = Keys(this);
    }

    Btree(int k): Btree()
    {
        keys.add(Branch(k));
    }

    Btree(Branch k): Btree()
    {
        keys.add(k);
    }

    Btree(Keys ks)
    {
        keys = ks;
        // TODO: this is not really a nice solution
        keys.set_owner(this);
    }

    void add(int k)
    {
        auto n = get_leaf_for_key(k);
        if (n->keys.size() < degree)
        {
            n->keys.add(Branch(k));
        }
        else
        {
            n->upwards_add(Branch(k));
        }
    }

    void inorder_walk(std::function<void(int)> on_visit)
    {
        if (!keys.is_leaf())
        {
            for (size_t i = 0; i < keys.size(); i++)
            {
                Branch k = keys.get_branch(i);
                k.left->inorder_walk(on_visit);
                on_visit(k.value);
            }

            keys.get_rightmost_child()->inorder_walk(on_visit);
        }
        else
        {
            for (size_t i = 0; i < keys.size(); i++)
            {
                on_visit(keys.get_branch(i).value);
            }
        }
    }

    void preorder_walk(std::function<void(int)> on_visit)
    {
        for (size_t i = 0; i < keys.size(); i++)
        {
            on_visit(keys.get_branch(i).value);
        }

        for (auto it = keys.children_begin(); it != keys.children_end(); it++)
        {
            (*it)->preorder_walk(on_visit);
        }
    }

    void postorder_walk(std::function<void(int)> on_visit)
    {
        for (auto it = keys.children_begin(); it != keys.children_end(); it++)
        {
            (*it)->postorder_walk(on_visit);
        }

        for (size_t i = 0; i < keys.size(); i++)
        {
            on_visit(keys.get_branch(i).value);
        }
    }

    std::vector<int> dump()
    {
        std::vector<int> result;
        inorder_walk([&result] (int k) {
            result.push_back(k);
        });

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
    Btree* get_leaf_for_key(int k)
    {
        if (keys.is_present(k))
        {
            throw invalid_key_exception("key already exists");
        }

        if (keys.is_leaf())
        {
            return this;
        }

        return select_branch_for_key(k)->get_leaf_for_key(k);
    }

    Btree* select_branch_for_key(int new_key)
    {
        Btree* branch_for_key = keys.get_first_branch().left;

        for (size_t i = 0; i < keys.size(); i++)
        {
            auto key = keys.get_branch(i);

            if (key.value < new_key)
            {
                branch_for_key = key.right;
            }
        }

        return branch_for_key;
    }

    void upwards_add(Branch branch)
    {
        if (keys.size() < degree)
        {
            insert(branch);

            return;
        }

        auto seperated = seperate_current_for_unfitting(branch);

        if (parent == nullptr)
        {
            grow(seperated);
        }
        else
        {
            parent->upwards_add(seperated);

            remove_self();
        }
    }

    void insert(Branch k)
    {
        keys.add(k);
    }

    Branch seperate_current_for_unfitting(Branch unfitting)
    {
        auto median = keys.get_median_with_new_key(unfitting.value);
        Branch seperated(median);

        if (median == unfitting.value)
        {
            auto left_branch_keys = keys.get_left_half_of_keys();
            auto right_branch_keys = keys.get_right_half_of_keys();

            if (unfitting.left != nullptr && unfitting.right != nullptr)
            {
                left_branch_keys.drop_keys_bigger_than(median);
                right_branch_keys.drop_keys_less_than(median);
                //left_branch_keys.right = unfitting.left;
                //right_branch_keys.left = unfitting.right;
            }

            seperated.left = new Btree(left_branch_keys);
            seperated.right = new Btree(right_branch_keys);
        }
        else if (median < unfitting.value)
        {
            auto left_branch_keys = keys.get_left_half_of_keys();
            auto right_branch_keys = keys.get_right_half_of_keys();

            right_branch_keys.remove(median);
            right_branch_keys.add(unfitting);

            seperated.left = new Btree(left_branch_keys);
            seperated.right = new Btree(right_branch_keys);
        }
        else
        {
            auto left_branch_keys = keys.get_left_half_of_keys();
            auto right_branch_keys = keys.get_right_half_of_keys();

            left_branch_keys.remove(median);
            left_branch_keys.add(unfitting);

            seperated.left = new Btree(left_branch_keys);
            seperated.right = new Btree(right_branch_keys);
        }

        return seperated;
    }

    void grow(Branch new_root)
    {
        keys.clear();
        insert(new_root);
    }

    void remove_self()
    {
        keys.clear();
        delete this;
    }

};
#endif

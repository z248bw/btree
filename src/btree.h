#ifndef BTREE_H_
#define BTREE_H_

#include<keys.h>


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
    Keys<Btree> keys;

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
    Btree* parent = nullptr;
    Btree()
    {
        keys = Keys<Btree>(degree, this);
    }

    Btree(int k): Btree()
    {
        keys.add(Branch<Btree>(k));
    }

    Btree(Branch<Btree> k): Btree()
    {
        keys.add(k);
    }

    Btree(Keys<Btree> ks)
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
            n->keys.add(Branch<Btree>(k));
        }
        else
        {
            n->upwards_add(Branch<Btree>(k));
        }
    }

    void inorder_walk(std::function<void(int)> on_visit)
    {
        if (!keys.is_leaf())
        {
            for (size_t i = 0; i < keys.size(); i++)
            {
                auto k = keys.get_branch(i);
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

    void upwards_add(Branch<Btree> branch)
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

    void insert(Branch<Btree> k)
    {
        keys.add(k);
    }

    Branch<Btree> seperate_current_for_unfitting(Branch<Btree> unfitting)
    {
        auto median = keys.get_median_with_new_key(unfitting.value);
        Branch<Btree> seperated(median);
        auto left_branch_keys = keys.get_left_half_of_keys();
        auto right_branch_keys = keys.get_right_half_of_keys();

        if (median == unfitting.value &&
            unfitting.left != nullptr &&
            unfitting.right != nullptr
        )
        {
            assert(left_branch_keys.get_last_branch().right == right_branch_keys.get_first_branch().left);

            auto left_last_branch = left_branch_keys.get_last_branch();
            left_last_branch.right = unfitting.left;
            left_branch_keys.change_last_to(left_last_branch);

            auto right_first_branch = right_branch_keys.get_first_branch();
            right_first_branch.left = unfitting.right;
            right_branch_keys.change_first_to(right_first_branch);
        }
        else if (median < unfitting.value)
        {
            right_branch_keys.change_first_to(unfitting);
        }
        else if (median > unfitting.value)
        {
            left_branch_keys.change_last_to(unfitting);
        }

        seperated.left = new Btree(left_branch_keys);
        seperated.right = new Btree(right_branch_keys);

        return seperated;
    }

    void grow(Branch<Btree> new_root)
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

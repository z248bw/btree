#ifndef BTREE_H_
#define BTREE_H_

#include<functional>

#include "keys/keys.h"


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
class Btree
{
    friend class Keys<Btree<degree>>;

private:
    Btree* parent = nullptr;

protected:
    Keys<Btree> keys;

    Btree(const Keys<Btree> ks)
    {
        keys = ks;
        keys.set_owner(this);
    }

    virtual Btree* new_node(const Keys<Btree> ks)
    {
        return new Btree(ks);
    }

public:

    Btree()
    {
        keys = Keys<Btree>(degree, this);
    }

    Btree(const Btree & other)
    {
        keys = other.keys;
        for (auto it = keys.children_begin(); it != keys.children_end(); it++)
        {
            (*it) = (*it)->copy(this);
        }
    }

    Btree(Btree && other) = default;

    Btree& operator=(Btree copy_of_other)
    {
        std::swap(keys, copy_of_other.keys);
        std::swap(parent, copy_of_other.parent);

        return *this;
    }

    void add(const int k)
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

    std::vector<int> dump()
    {
        std::vector<int> result;
        inorder_walk([&result] (int k) {
            result.push_back(k);
        });

        return result;
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

    virtual ~Btree()
    {
        for (auto it = keys.children_begin(); it != keys.children_end(); it++)
        {
            delete *it;
        }
    }

private:
    Btree* get_leaf_for_key(const int k)
    {
        if (keys.is_present(k))
        {
            throw invalid_key_exception("key already exists");
        }

        if (keys.is_leaf())
        {
            return this;
        }

        return keys.select_node_for_key(k)->get_leaf_for_key(k);
    }

    void upwards_add(Branch<Btree> branch)
    {
        if (keys.size() < degree)
        {
            keys.add(branch);

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

    Branch<Btree> seperate_current_for_unfitting(Branch<Btree> unfitting)
    {
        auto median = keys.get_median_with_new_key(unfitting.value);
        Branch<Btree> seperated(median);
        auto left_branch_keys = keys.get_left_half_of_keys();
        auto right_branch_keys = keys.get_right_half_of_keys();

        if (median == unfitting.value && unfitting.has_children())
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

        seperated.left = new_node(left_branch_keys);
        seperated.right = new_node(right_branch_keys);

        return seperated;
    }

    void grow(Branch<Btree> new_root)
    {
        keys.clear();
        keys.add(new_root);
    }

    void remove_self() noexcept
    {
        keys.clear();
        delete this;
    }

    Btree* copy(Btree* parent)
    {
        auto copy = new_node(keys);
        copy->parent = parent;

        for (auto it = keys.children_begin(); it != keys.children_end(); it++)
        {
            (*it) = (*it)->copy(copy);
        }

        return copy;
    }

};
#endif

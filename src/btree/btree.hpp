#ifndef BTREE_H_
#define BTREE_H_

#include<functional>

#include "keys/keys.hpp"


namespace btree
{
    class duplicated_key_exception: public std::exception
    {
    public:
        virtual const char* what() const noexcept override
        {
            return "duplicated key";
        }
    };


    template <typename KEY, typename VALUE, size_t DEGREE>
    class Btree
    {
    public:
        static const size_t degree = DEGREE;
        using key_t = KEY;
        using value_t = VALUE;

    private:
        friend class Keys<Btree>;

        using KV_pair = std::pair<key_t, value_t>;

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

        Btree & operator=(Btree copy_of_other)
        {
            std::swap(keys, copy_of_other.keys);
            std::swap(parent, copy_of_other.parent);

            return *this;
        }

        void add(const key_t k, const value_t v)
        {
            auto n = get_leaf_for_key(k);
            auto kv = KeyValue<key_t, value_t>(k, v);

            if (n->keys.size() < degree)
            {
                n->keys.add(Branch<Btree>(kv));
            }
            else
            {
                n->upwards_add(Branch<Btree>(kv));
            }
        }

        value_t & get(const key_t k)
        {
            auto value = keys.find_and_get_value(k);
            if (value.is_present)
            {
                return value;
            }

            if (keys.is_leaf())
            {
                throw key_does_not_exist_exception();
            }

            return keys.select_node_for_key(k)->get(k);
        }

        std::vector<KV_pair> dump()
        {
            std::vector<KV_pair> result;
            inorder_walk([&result] (KV_pair kv_pair) {
                result.push_back(kv_pair);
            });

            return result;
        }

        void inorder_walk(std::function<void(KV_pair)> on_visit)
        {
            if (!keys.is_leaf())
            {
                for (size_t i = 0; i < keys.size(); i++)
                {
                    auto b = keys.get_branch(i);
                    b.left->inorder_walk(on_visit);
                    on_visit(b.kv);
                }

                keys.get_rightmost_child()->inorder_walk(on_visit);
            }
            else
            {
                for (size_t i = 0; i < keys.size(); i++)
                {
                    on_visit(keys.get_branch(i).kv);
                }
            }
        }

        void preorder_walk(std::function<void(KV_pair)> on_visit)
        {
            for (size_t i = 0; i < keys.size(); i++)
            {
                on_visit(keys.get_branch(i).kv);
            }

            for (auto it = keys.children_begin(); it != keys.children_end(); it++)
            {
                (*it)->preorder_walk(on_visit);
            }
        }

        void postorder_walk(std::function<void(KV_pair)> on_visit)
        {
            for (auto it = keys.children_begin(); it != keys.children_end(); it++)
            {
                (*it)->postorder_walk(on_visit);
            }

            for (size_t i = 0; i < keys.size(); i++)
            {
                on_visit(keys.get_branch(i).kv);
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
        Btree* get_leaf_for_key(const key_t k)
        {
            if (keys.is_present(k))
            {
                throw duplicated_key_exception();
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
            auto median = keys.get_median_KV_with_new_key(unfitting.kv);
            Branch<Btree> seperated(median);
            auto left_branch_keys = keys.get_left_half_of_keys();
            auto right_branch_keys = keys.get_right_half_of_keys();

            if (median == unfitting.kv && unfitting.has_children())
            {
                assert(left_branch_keys.get_last_branch().right == right_branch_keys.get_first_branch().left);

                auto left_last_branch = left_branch_keys.get_last_branch();
                left_last_branch.right = unfitting.left;
                left_branch_keys.change_last_to(left_last_branch);

                auto right_first_branch = right_branch_keys.get_first_branch();
                right_first_branch.left = unfitting.right;
                right_branch_keys.change_first_to(right_first_branch);
            }
            else if (median < unfitting.kv)
            {
                right_branch_keys.change_first_to(unfitting);
            }
            else if (median > unfitting.kv)
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

}
#endif

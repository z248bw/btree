#ifndef KEYS_H_
#define KEYS_H_

#include<vector>
#include<iterator>
#include<algorithm>
#include<assert.h>


namespace btree
{
    template<typename KEY, typename VALUE>
    struct KeyValue
    {
        KEY key;
        VALUE value;

        KeyValue() = default;

        KeyValue(const KEY key, const VALUE value): key(key), value(value) {}

        struct Compare
        {
            bool operator () (const KEY & k, const KeyValue<KEY, VALUE> & kv) const
            {
                return k < kv.key;
            }

            bool operator () (const KeyValue<KEY, VALUE> & kv, const KEY & k) const
            {
                return kv.key < k;
            }
        };

        operator std::pair<KEY, VALUE> () const
        {
            return std::pair<KEY, VALUE>(key, value);
        }

        bool operator<(const KeyValue & other) const
        {
            return key < other.key;
        }

        bool operator>(const KeyValue & other) const
        {
            return key > other.key;
        }

        bool operator==(const KeyValue & other) const
        {
            return key == other.key;
        }

        operator KEY () const
        {
            return key;
        }
    };


    template<class Node>
    struct Branch
    {
        using KV = KeyValue<typename Node::key_t, typename Node::value_t>;

        KV kv;
        Node* left;
        Node* right;

        Branch(KV kv): kv(kv), left(nullptr), right(nullptr) {}
        Branch(KV kv, Node* left, Node* right): kv(kv), left(left), right(right) {}

        bool has_children() const noexcept
        {
            return (left != nullptr && right != nullptr);
        }
    };


    class key_does_not_exist_exception: public std::exception
    {
    public:
        virtual const char* what() const noexcept override
        {
            return "key does not exist";
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

        template<typename VALUE>
        class SearchedValue
        {
        private:
            VALUE * v = nullptr;

        public:
            bool is_present = false;

            SearchedValue() = default;
            SearchedValue(bool _is_present, VALUE * _v): v(_v), is_present(_is_present) {}

            operator VALUE & ()
            {
                if (!is_present)
                {
                    throw key_does_not_exist_exception();
                }

                return *v;
            }
        };

        SearchedValue<typename Node::value_t> find_and_get_value(typename Node::key_t k)
        {
            size_t pos =
                std::lower_bound(keyvalues.begin(), keyvalues.end(), k, typename KV::Compare()) - keyvalues.begin();
            if (pos >= keyvalues.size() || keyvalues[pos].key != k)
            {
                return SearchedValue<typename Node::value_t>();
            }

            return SearchedValue<typename Node::value_t>(true, &keyvalues[pos].value);
        }

        Branch<Node> get_branch(const size_t i) const
        {
            if (is_leaf())
            {
                return Branch<Node>(keyvalues[i]);
            }

            return Branch<Node>(keyvalues[i], get_child(i), get_child(i + 1));
        }

        void add(const Branch<Node> b)
        {
            auto pos = get_pos_of_key(b.kv);
            keyvalues.insert(keyvalues.begin() + pos, b.kv);

            if (is_last_position(pos))
            {
                push_children_of_branch(b);
            }
            else
            {
                insert_children_of_branch_to_pos(b, pos);
            }
        }

        Node* select_child_for_key(const typename Node::key_t k) const
        {
            return children[get_pos_of_key(k)];
        }

        bool is_present(const typename Node::key_t k) const noexcept
        {
            return std::binary_search(keyvalues.begin(), keyvalues.end(), k, typename KV::Compare());
        }

        KV get_median_KV_with_new_key(const KV k) const
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
            auto last_index = keyvalues.size() - 1;

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
            return children[children.size() - 1];
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
            return std::upper_bound(keyvalues.begin(), keyvalues.end(), k, typename KV::Compare()) - keyvalues.begin();
        }

        Node* get_child(const size_t i) const noexcept
        {
            if (children.size() < i + 1)
            {
                return nullptr;
            }

            return children[i];
        }

        void push_children_of_branch(Branch<Node> b)
        {
            if (!b.has_children())
            {
                return;
            }

            own_branch(b);

            if (children.size() == 0)
            {
                children.push_back(b.left);
            }
            else
            {
                children[children.size() - 1] = b.left;
            }

            children.push_back(b.right);
        }

        void own_branch(Branch<Node> b)
        {
            b.left->parent = owner;
            b.right->parent = owner;
        }

        void insert_children_of_branch_to_pos(Branch<Node> b, const size_t pos)
        {
            if (!b.has_children())
            {
                return;
            }

            own_branch(b);

            children.insert(children.begin() + pos, b.left);
            children[pos+1] = b.right;
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
}

#endif

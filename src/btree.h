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
    virtual Traversable* get_child(const int i) = 0;
    virtual int max_branches_num() = 0;

public:
    static int deepest;
    static int shallowest;
    virtual ~Traversable(){}

    void walk(Traversable* t = nullptr, int depth = 0)
    {
        if (t == nullptr)
        {
            init_walk();
            t = this;
        }

        int children_num = 0;
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

    Traversable* get_child(const int i)
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

    int max_branches_num()
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
    std::vector<int> keys;
    std::vector<Btree*> children;

public:
    Btree() {}

    Btree(int k)
    {
        keys.push_back(k);
    }

    Btree(std::vector<int> keys, std::vector<Btree*> children):
        keys(keys), children(children) {}

    void add(int k)
    {
        auto n = find_node_for_key(this, k);
        if (n->keys.size() < MAX_KEY_SIZE)
        {
            n->keys.push_back(k);
            std::sort(n->keys.begin(), n->keys.end());
        }
        else
        {
            auto left = new Btree(n->keys[0]);
            auto right = new Btree(k);
            auto median = n->keys[MAX_KEY_SIZE-1];
            n->keys.clear();
            n->children.clear();
            n->keys.push_back(median);
            n->children.push_back(left);
            n->children.push_back(right);
        }
    }

    Btree* find_node_for_key(Btree* n, int k)
    {
        // start inorder walk of the tree
        for (int i = 0; i < n->keys.size(); i++)
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

    std::vector<int> get_keys()
    {
        return keys;
    }

    ~Btree()
    {
        for (auto child : children)
        {
            delete child;
        }
    }

    Traversable* get_child(const int i)
    {
        if (i < children.size())
        {
            return children[i];
        }

        return nullptr;
    }

    int max_branches_num()
    {
        return keys.size() + 1;
    }
};

#endif

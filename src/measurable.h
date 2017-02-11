#ifndef MEASURABLE_H_
#define MEASURABLE_H_

#include<btree.h>

#define MAX_INT 99999

// I want to be able to simply verify whether the tree is balanced.
// A way to do this is, to find the level of the deepest and the shallowest leaf and then check
// if they are equal. I provide this functionality as a mixin because I need to
// prove that the walk algorithm works correctly and for this I need to be able
// to walk unbalanced trees as well.
class Measurable
{
protected:
    virtual bool is_leaf() = 0;
    virtual std::vector<Measurable*> get_children() = 0;

public:
    static size_t deepest;
    static size_t shallowest;
    virtual ~Measurable(){}

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

class MeasurableTree : public Measurable
{
protected:
    virtual std::vector<Measurable*> get_children() override
    {
        auto vector = std::vector<Measurable*>();
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
    MeasurableTree* l;
    MeasurableTree* r;

    Measurable* current;

    MeasurableTree(): l(nullptr), r(nullptr) {}

    ~MeasurableTree()
    {
        delete l;
        delete r;
    }
};

template <size_t degree>
class MeasurableBtree : public Btree<degree>, public Measurable
{
private:
    MeasurableBtree(const Keys<Btree<degree>> ks): Btree<degree>(ks) {}

protected:
    virtual Btree<degree>* new_node(const Keys<Btree<degree>> ks) override
    {
        return new MeasurableBtree(ks);
    }

    virtual bool is_leaf() override
    {
        return this->keys.is_leaf();
    }

    virtual std::vector<Measurable*> get_children() override
    {
        std::vector<Measurable*> result;
        for (auto it = this->keys.children_begin(); it != this->keys.children_end(); ++it)
        {
            result.push_back(static_cast<MeasurableBtree*>(*(it)));
        }

        return result;
    }

public:
    MeasurableBtree(): Btree<degree>() {}

    MeasurableBtree* find_node_with_key(const int k)
    {
        if (this->keys.is_present(k))
        {
            return this;
        }

        for (auto it = this->keys.children_begin(); it != this->keys.children_end(); it++)
        {
            auto result = static_cast<MeasurableBtree<degree>*>(*it)->find_node_with_key(k);
            if (result != nullptr)
            {
                return result;
            }
        }

        return nullptr;
    }

    std::vector<int> get_keys() const
    {
        return this->keys.dump();
    }

};

#endif

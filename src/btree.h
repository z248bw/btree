#ifndef BTREE_H_
#define BTREE_H_

#define MAX_KEY_SIZE 3
#define MAX_INT 99999

class Traversable
{
public:
    static int deepest;
    static int shallowest;
    virtual Traversable* get_child(const int i) = 0;
    virtual int max_branches_num() = 0;
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

        // it does not have all the children
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
    int keys[MAX_KEY_SIZE];
    int keys_len = 0;
    Btree* children[MAX_KEY_SIZE-1];
    int children_len = 0;

public:
    ~Btree()
    {
        for (int i = 0; i < children_len; i++)
        {
            delete children[i];
        }
    }

    Traversable* get_child(const int i)
    {
        if (i < children_len)
        {
            return children[i];
        }

        return nullptr;
    }

    int max_branches_num()
    {
        return MAX_KEY_SIZE-1;
    }

};

#endif

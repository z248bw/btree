#ifndef KEYSTESTUTILS_H_
#define KEYSTESTUTILS_H_

#include<memory>

#include "gtest/gtest.h"

#include "keys/keys.h"

struct TestNode
{
    size_t id;
    TestNode* parent;

    TestNode(size_t id): id(id) {}
    void set_parent(TestNode* new_parent)
    {
        parent = new_parent;
    }
};

class TestNodeFactoryRAII
{
private:
    std::vector<std::shared_ptr<TestNode>> nodes;

public:
    std::vector<TestNode*> create_nodes(size_t n);
    TestNode* create(size_t id);
};

class KeysFactoryRAII
{
private:
    TestNodeFactoryRAII node_factory;

public:
    Keys<TestNode> create_keys(size_t n);
};


#endif

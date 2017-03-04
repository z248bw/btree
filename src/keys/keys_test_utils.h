#ifndef KEYSTESTUTILS_H_
#define KEYSTESTUTILS_H_

#include<memory>

#include "gtest/gtest.h"

#include "keys/keys.h"


template<typename T>
struct IdentityKeyValue: public KeyValue<T, T>
{
    IdentityKeyValue(T id): KeyValue<T, T>(id, id) {}
};

struct TestNode
{
    using key_t = int;
    using value_t = int;
    KeyValue<key_t, value_t> id;
    TestNode* parent;

    TestNode(size_t id): id(id, id){}
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

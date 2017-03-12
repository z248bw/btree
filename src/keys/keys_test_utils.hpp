#ifndef KEYSTESTUTILS_H_
#define KEYSTESTUTILS_H_

#include<memory>
#include<string>

#include "gtest/gtest.h"

#include "keys/keys.hpp"


namespace btree
{
    KeyValue<int, const char*> get_kv(size_t n);


    template<typename K = int, typename V = const char*>
    struct TestNode
    {
        using key_t = K;
        using value_t = V;

        KeyValue<key_t, value_t> kv;
        TestNode* parent;

        TestNode(KeyValue<K, V> n_kv): kv(n_kv) { }
    };


    class TestNodeFactoryRAII
    {
    private:
        std::vector<std::shared_ptr<TestNode<>>> nodes;

    public:
        std::vector<TestNode<>*> create_nodes(size_t n);
        TestNode<>* create(size_t id);
    };


    class KeysFactoryRAII
    {
    private:
        TestNodeFactoryRAII node_factory;

    public:
        Keys<TestNode<>> create_keys(size_t n);
    };
}


#endif

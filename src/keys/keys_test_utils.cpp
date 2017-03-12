#include "keys_test_utils.hpp"


namespace btree
{
    KeyValue<int, const char*> get_kv(size_t n)
    {
        // * lower case ascii letters start from 97
        // * there is 25 lower case ascii letters
        const char* v = std::to_string((n % 25) + 97).c_str();
        return KeyValue<int, const char*>(n, v);
    }

    std::vector<TestNode<>*> TestNodeFactoryRAII::create_nodes(size_t n)
    {
        std::vector<TestNode<>*> result;

        for (size_t i = 0; i < n; i++)
        {
            result.push_back(create(i+1));
        }

        return result;
    }

    TestNode<>* TestNodeFactoryRAII::create(size_t id)
    {
        auto new_node = std::make_shared<TestNode<>>(get_kv(id));
        nodes.push_back(new_node);

        return new_node.get();
    }

    Keys<TestNode<>> KeysFactoryRAII::create_keys(size_t n)
    {
        Keys<TestNode<>> ks(n+1);
        auto nodes = node_factory.create_nodes(n + 1);

        for (size_t i = 0; i < n; i++)
        {
            ks.add(Branch<TestNode<>>(get_kv(i+1), nodes[i], nodes[i+1]));
        }

        return ks;
    }
}

#include "keys_test_utils.h"

std::vector<TestNode*> TestNodeFactoryRAII::create_nodes(size_t n)
{
    std::vector<TestNode*> result;

    for (size_t i = 0; i < n; i++)
    {
        result.push_back(create(i+1));
    }

    return result;
}

TestNode* TestNodeFactoryRAII::create(size_t id)
{
    auto new_node = std::make_shared<TestNode>(id);
    nodes.push_back(new_node);

    return new_node.get();
}

Keys<TestNode> KeysFactoryRAII::create_keys(size_t n)
{
    Keys<TestNode> ks(n+1);
    auto nodes = node_factory.create_nodes(n + 1);

    for (size_t i = 0; i < n; i++)
    {
        ks.add(Branch<TestNode>(IdentityKeyValue<int>(i+1), nodes[i], nodes[i+1]));
    }

    return ks;
}

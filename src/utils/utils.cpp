#include "utils.h"

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
        ks.add(Branch<TestNode>(i+1, nodes[i], nodes[i+1]));
    }

    return ks;
}

std::vector<int> input_from_file(std::string filename)
{
    std::vector<int> result;
    std::ifstream file;
    file.open(filename);
    std::string line;
    while(getline(file, line))
    {
        result.push_back(std::stoi(line));
    }

    file.close();

    return result;
}

void test_from_file(std::string filename)
{
    std::vector<int> inputs = input_from_file(filename);

    MeasurableBtree<2> t;
    for (int i : inputs)
    {
        t.add(i);
    }

    std::sort(inputs.begin(), inputs.end());
    auto result = t.dump();
    for (size_t i = 0; i < inputs.size(); i++)
    {
        ASSERT_EQ(inputs[i], result[i]);
    }

    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}

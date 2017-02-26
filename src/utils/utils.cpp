#include "utils.h"

std::vector<TestNode*> create_test_nodes(size_t n)
{
    std::vector<TestNode*> result;

    for (size_t i = 0; i < n; i++)
    {
        result.push_back(new TestNode(i + 1));
    }

    return result;
}

Keys<TestNode> create_keys(size_t n)
{
    Keys<TestNode> ks(n+1);
    auto nodes = create_test_nodes(n + 1);

    for (size_t i = 0; i < n; i++)
    {
        ks.add(Branch<TestNode>(i+1, nodes[i], nodes[i+1]));
    }

    return ks;
}

void destroy_keys(Keys<TestNode> ks)
{
    for (auto it = ks.children_begin(); it != ks.children_end(); it++)
    {
        delete *it;
    }
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

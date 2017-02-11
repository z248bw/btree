#include "test_utils.h"

std::vector<TestNode*> create_test_nodes(size_t n)
{
    std::vector<TestNode*> result;

    for (size_t i = 0; i < n; i++)
    {
        result.push_back(new TestNode(i + 1));
    }

    return result;
}

void destroy_test_nodes(std::vector<TestNode*> nodes)
{
    for (TestNode* n : nodes)
    {
        delete n;
    }
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

MeasurableBtree<2> tree_with_incremental_elements(size_t num_of_elems)
{
    MeasurableBtree<2> t;
    for (size_t i = 0; i < num_of_elems; i++)
    {
        t.add(i);
    }

    return t;
}

void incremental_test(size_t num_of_elems)
{
    MeasurableBtree<2> t = tree_with_incremental_elements(num_of_elems);

    auto result = t.dump();
    for (size_t i = 0; i < result.size(); i++)
    {
        ASSERT_EQ(i, result[i]);
    }

    t.walk();
    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}


void test_mixed(size_t n)
{
    auto top = 2 * n + 10;

    MeasurableBtree<2> t;
    for (size_t i = 0; i < n; i++)
    {
        t.add(i);
        t.add(top - i);
    }

    auto result = t.dump();
    for (size_t i = 0; i < n; i++)
    {
        ASSERT_EQ(i, result[i]);
    }

    for (size_t i = 0; i < n; i++)
    {
        ASSERT_EQ(i+n+10+1, result[i+n]);
    }

    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}


void test_random(size_t n)
{
    std::unordered_set<int> s;
    std::srand(std::time(0));
    for (size_t i = 0; i < n; i++)
    {
        s.insert(std::rand() % 100);
    }


    MeasurableBtree<2> t;
    for (int i : s)
    {
        t.add(i);
    }

    auto result = t.dump();
    std::vector<int> inputs(s.begin(), s.end());
    std::sort(inputs.begin(), inputs.end());
    for (size_t i = 0; i < inputs.size(); i++)
    {
        ASSERT_EQ(inputs[i], result[i]);
    }

    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
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

MeasurableBtree<3> incremental_even_tree_with_num_of_elems(size_t n)
{
    MeasurableBtree<3> t;
    for (size_t i = 0; i < n; i++)
    {
       t.add(i);
    }

    return t;
}

void test_incremental_with_odd_degree_and_num_of_elems(size_t n)
{
    MeasurableBtree<3> t = incremental_even_tree_with_num_of_elems(n);
    auto result = t.dump();
    for (size_t i = 0; i < n; i++)
    {
        ASSERT_EQ(i, result[i]);
    }

    t.walk();
    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}

void odd_decremental_test(size_t num_of_elems)
{
    MeasurableBtree<3> t;
    for (size_t i = num_of_elems; i > 0; i--)
    {
        t.add(i);
    }

    auto result = t.dump();
    for (size_t i = 0; i < result.size(); i++)
    {
        ASSERT_EQ(i+1, result[i]);
    }

    t.walk();
    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}

void test_odd_mixed(size_t n)
{
    auto top = 2 * n + 10;

    MeasurableBtree<3> t;
    for (size_t i = 0; i < n; i++)
    {
        t.add(i);
        t.add(top - i);
    }

    auto result = t.dump();
    for (size_t i = 0; i < n; i++)
    {
        ASSERT_EQ(i, result[i]);
    }

    for (size_t i = 0; i < n; i++)
    {
        ASSERT_EQ(i+n+10+1, result[i+n]);
    }

    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}

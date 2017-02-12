#ifndef TESTUTILS_H_
#define TESTUTILS_H_

#include<unordered_set>
#include<iostream>
#include<fstream>

#include "gtest/gtest.h"

#include "measurable.h"

struct TestNode
{
    size_t id;
    TestNode* parent;

    TestNode(size_t id): id(id) {}
};

std::vector<TestNode*> create_test_nodes(size_t n);

void destroy_test_nodes(std::vector<TestNode*> nodes);

Keys<TestNode> create_keys(size_t n);

void destroy_keys(Keys<TestNode> ks);

template<size_t degree>
MeasurableBtree<degree> tree_with_incremental_elements(size_t num_of_elems)
{
    MeasurableBtree<degree> t;
    for (size_t i = 0; i < num_of_elems; i++)
    {
        t.add(i);
    }

    return t;
}

template<size_t degree>
void test_incremental(size_t num_of_elems)
{
    MeasurableBtree<degree> t = tree_with_incremental_elements<degree>(num_of_elems);

    auto result = t.dump();
    for (size_t i = 0; i < result.size(); i++)
    {
        ASSERT_EQ(i, result[i]);
    }

    t.measure();
    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}

template<size_t degree>
void test_decremental(size_t num_of_elems)
{
    MeasurableBtree<degree> t;
    for (size_t i = num_of_elems; i > 0; i--)
    {
        t.add(i);
    }

    auto result = t.dump();
    for (size_t i = 0; i < result.size(); i++)
    {
        ASSERT_EQ(i+1, result[i]);
    }

    t.measure();
    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}

template<size_t degree>
void test_mixed(size_t n)
{
    auto top = 2 * n + 10;

    MeasurableBtree<degree> t;
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

    t.measure();
    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}

template<size_t degree>
void test_random(size_t n)
{
    std::unordered_set<int> s;
    std::srand(std::time(0));
    for (size_t i = 0; i < n; i++)
    {
        s.insert(std::rand() % 100);
    }


    MeasurableBtree<degree> t;
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

    t.measure();
    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}


std::vector<int> input_from_file(std::string filename);

void test_from_file(std::string filename);


#endif

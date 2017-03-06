#ifndef MEASURABLETESTUTILS_H_
#define MEASURABLETESTUTILS_H_

#include<unordered_set>
#include<iostream>
#include<fstream>
#include<memory>

#include "gtest/gtest.h"

#include "measurable/measurable.h"
#include "keys/keys.h"
#include "keys/keys_test_utils.h"


template<class T>
void check_balance(T measurable_btree)
{
    measurable_btree.measure();

    ASSERT_EQ(Measurable::deepest, Measurable::shallowest);
}

template<size_t degree>
MeasurableBtree<degree> tree_with_incremental_elements(size_t num_of_elems)
{
    MeasurableBtree<degree> t;
    for (size_t i = 0; i < num_of_elems; i++)
    {
        t.add(get_kv(i));
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

    check_balance(t);
}

template<size_t degree>
void test_decremental(size_t num_of_elems)
{
    MeasurableBtree<degree> t;
    for (size_t i = num_of_elems; i > 0; i--)
    {
        t.add(get_kv(i));
    }

    auto result = t.dump();
    for (size_t i = 0; i < result.size(); i++)
    {
        ASSERT_EQ(i+1, result[i]);
    }

    check_balance(t);
}


template<size_t degree>
MeasurableBtree<degree> tree_filled_in_mixed_order(size_t n)
{
    auto top = 2 * n + 10;

    MeasurableBtree<degree> t;
    for (size_t i = 0; i < n; i++)
    {
        t.add(get_kv(i));
        t.add(get_kv(top - i));
    }

    return t;
}

template<size_t degree>
void test_mixed(size_t n)
{
    auto t = tree_filled_in_mixed_order<degree>(n);
    auto result = t.dump();
    for (size_t i = 0; i < n; i++)
    {
        ASSERT_EQ(i, result[i]);
    }

    for (size_t i = 0; i < n; i++)
    {
        ASSERT_EQ(i+n+10+1, result[i+n]);
    }

    check_balance(t);
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
        t.add(get_kv(i));
    }

    auto result = t.dump();
    std::vector<int> inputs(s.begin(), s.end());
    std::sort(inputs.begin(), inputs.end());
    for (size_t i = 0; i < inputs.size(); i++)
    {
        ASSERT_EQ(inputs[i], result[i]);
    }

    check_balance(t);
}


std::vector<int> input_from_file(std::string filename);

void test_from_file(std::string filename);


#endif

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

MeasurableBtree<2> tree_with_incremental_elements(size_t num_of_elems);

void incremental_test(size_t num_of_elems);

void test_mixed(size_t n);

void test_random(size_t n);

std::vector<int> input_from_file(std::string filename);

void test_from_file(std::string filename);

MeasurableBtree<3> incremental_even_tree_with_num_of_elems(size_t n);

void test_incremental_with_odd_degree_and_num_of_elems(size_t n);

void odd_decremental_test(size_t num_of_elems);

void test_odd_mixed(size_t n);

#endif

#include "test_utils.h"
#include "gtest/gtest.h"
#include<iostream>
#include<fstream>
#include<unordered_set>


TEST(Test, should_pass_if_two_strings_are_equal_by_value) {
    const char* s1 = "kaucsuk rece";
    ASSERT_STREQ("kaucsuk rece", s1);
}

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

TEST(Keys, store_branches_in_order) {
    auto ks = create_keys(3);

    auto b1 = ks.get_branch(0);
    auto b2 = ks.get_branch(1);
    auto b3 = ks.get_branch(2);

    ASSERT_EQ(1, b1.value);
    ASSERT_EQ(1, b1.left->id);
    ASSERT_EQ(2, b1.right->id);
    ASSERT_EQ(2, b2.value);
    ASSERT_EQ(2, b2.left->id);
    ASSERT_EQ(3, b2.right->id);
    ASSERT_EQ(3, b3.value);
    ASSERT_EQ(3, b3.left->id);
    ASSERT_EQ(4, b3.right->id);

    destroy_keys(ks);
}

TEST(Keys, add_new_branch_at_beginning) {
    auto ks = create_keys(1);
    auto original_first_branch = ks.get_first_branch();

    ks.add(Branch<TestNode>(-1, new TestNode(-1), new TestNode(-2)));

    auto b1 = ks.get_branch(0);
    auto b2 = ks.get_branch(1);

    ASSERT_EQ(-1, b1.value);
    ASSERT_EQ(-1, b1.left->id);
    ASSERT_EQ(-2, b1.right->id);
    ASSERT_EQ(1, b2.value);
    ASSERT_EQ(-2, b2.left->id);
    ASSERT_EQ(2, b2.right->id);

    destroy_keys(ks);
    delete original_first_branch.left;
}

TEST(Keys, add_new_branch_at_end) {
    auto ks = create_keys(1);
    auto original_first_branch = ks.get_first_branch();

    ks.add(Branch<TestNode>(9, new TestNode(9), new TestNode(10)));

    auto b1 = ks.get_branch(0);
    auto b2 = ks.get_branch(1);

    ASSERT_EQ(1, b1.value);
    ASSERT_EQ(1, b1.left->id);
    ASSERT_EQ(9, b1.right->id);
    ASSERT_EQ(9, b2.value);
    ASSERT_EQ(9, b2.left->id);
    ASSERT_EQ(10, b2.right->id);

    destroy_keys(ks);
    delete original_first_branch.right;
}

TEST(Keys, add_new_branch_at_middle) {
    Keys<TestNode> ks(9);
    auto common_node = new TestNode(100);
    ks.add(Branch<TestNode>(1, new TestNode(1), common_node));
    ks.add(Branch<TestNode>(100, common_node, new TestNode(101)));

    ks.add(Branch<TestNode>(9, new TestNode(9), new TestNode(10)));

    auto b1 = ks.get_branch(0);
    auto b2 = ks.get_branch(1);
    auto b3 = ks.get_branch(2);

    ASSERT_EQ(1, b1.value);
    ASSERT_EQ(1, b1.left->id);
    ASSERT_EQ(9, b1.right->id);
    ASSERT_EQ(9, b2.value);
    ASSERT_EQ(9, b2.left->id);
    ASSERT_EQ(10, b2.right->id);
    ASSERT_EQ(100, b3.value);
    ASSERT_EQ(10, b3.left->id);
    ASSERT_EQ(101, b3.right->id);

    destroy_keys(ks);
    delete common_node;
}

TEST(Keys, is_present) {
    Keys<TestNode> ks(4);

    ks.add(3);
    ks.add(5);
    ks.add(2);
    ks.add(1);
    ks.add(4);

    ASSERT_EQ(false, ks.is_present(6));
    ASSERT_EQ(true, ks.is_present(3));
    ASSERT_EQ(true, ks.is_present(1));
    ASSERT_EQ(true, ks.is_present(5));
}

TEST(Keys, empty_get_median_with_new_key) {
    Keys<TestNode> ks(4);

    ASSERT_EQ(1, ks.get_median_with_new_key(1));
}

TEST(Keys, one_element_get_median_with_new_key) {
    Keys<TestNode> ks(4);

    ks.add(4);

    ASSERT_EQ(1, ks.get_median_with_new_key(1));
}

TEST(Keys, two_elements_get_median_with_new_key) {
    Keys<TestNode> ks(4);

    ks.add(2);
    ks.add(3);

    ASSERT_EQ(2, ks.get_median_with_new_key(1));
}

TEST(Keys, three_elements_get_median_with_new_key) {
    Keys<TestNode> ks(4);

    ks.add(2);
    ks.add(3);
    ks.add(4);

    ASSERT_EQ(2, ks.get_median_with_new_key(1));
}

TEST(Keys, even_get_left_half_of_keys) {
    auto ks = create_keys(2);

    auto half = ks.get_left_half_of_keys();

    ASSERT_EQ(1, half.size());
    ASSERT_EQ(1, half.get_branch(0));
    ASSERT_EQ(1, half.get_branch(0).left->id);
    ASSERT_EQ(2, half.get_branch(0).right->id);

    destroy_keys(ks);
}

TEST(Keys, even_get_right_half_of_keys) {
    auto ks = create_keys(2);

    auto half = ks.get_right_half_of_keys();

    ASSERT_EQ(1, half.size());
    ASSERT_EQ(2, half.get_branch(0));
    ASSERT_EQ(2, half.get_branch(0).left->id);
    ASSERT_EQ(3, half.get_branch(0).right->id);

    destroy_keys(ks);
}

TEST(Keys, odd_get_left_half_of_keys) {
    auto ks = create_keys(3);

    auto half = ks.get_left_half_of_keys();

    ASSERT_EQ(1, half.size());
    ASSERT_EQ(1, half.get_branch(0));
    ASSERT_EQ(1, half.get_branch(0).left->id);
    ASSERT_EQ(2, half.get_branch(0).right->id);

    destroy_keys(ks);
}

TEST(Keys, odd_get_right_half_of_keys) {
    auto ks = create_keys(3);

    auto half = ks.get_right_half_of_keys();

    ASSERT_EQ(2, half.size());
    ASSERT_EQ(2, half.get_branch(0));
    ASSERT_EQ(2, half.get_branch(0).left->id);
    ASSERT_EQ(3, half.get_branch(0).right->id);
    ASSERT_EQ(3, half.get_branch(1));
    ASSERT_EQ(3, half.get_branch(1).left->id);
    ASSERT_EQ(4, half.get_branch(1).right->id);

    destroy_keys(ks);
}

TEST(Keys, change_first_to_for_keys_with_one_element) {
    auto ks = create_keys(1);
    auto branch_to_be_replaced = ks.get_first_branch();

    ks.change_first_to(Branch<TestNode>(9, new TestNode(9), new TestNode(10)));

    ASSERT_EQ(1, ks.size());
    ASSERT_EQ(9, ks.get_branch(0));
    ASSERT_EQ(9, ks.get_branch(0).left->id);
    ASSERT_EQ(10, ks.get_branch(0).right->id);

    destroy_keys(ks);
    delete branch_to_be_replaced.left;
    delete branch_to_be_replaced.right;
}

TEST(Keys, change_first_to_for_keys_with_multiple_elements) {
    auto ks = create_keys(2);
    auto branch_to_be_replaced = ks.get_first_branch();
    auto second_branch = ks.get_branch(1);

    ks.change_first_to(Branch<TestNode>(9, new TestNode(9), new TestNode(10)));

    ASSERT_EQ(2, ks.size());
    ASSERT_EQ(2, ks.get_branch(0));
    ASSERT_EQ(2, ks.get_branch(0).left->id);
    ASSERT_EQ(9, ks.get_branch(0).right->id);
    ASSERT_EQ(9, ks.get_branch(1));
    ASSERT_EQ(9, ks.get_branch(1).left->id);
    ASSERT_EQ(10, ks.get_branch(1).right->id);

    destroy_keys(ks);
    delete branch_to_be_replaced.left;
    delete second_branch.right;
}

TEST(Keys, change_last_to_for_keys_with_one_element) {
    auto ks = create_keys(1);
    auto branch_to_be_replaced = ks.get_last_branch();

    ks.change_last_to(Branch<TestNode>(9, new TestNode(9), new TestNode(10)));

    ASSERT_EQ(1, ks.size());
    ASSERT_EQ(9, ks.get_branch(0));
    ASSERT_EQ(9, ks.get_branch(0).left->id);
    ASSERT_EQ(10, ks.get_branch(0).right->id);

    destroy_keys(ks);
    delete branch_to_be_replaced.left;
    delete branch_to_be_replaced.right;
}

TEST(Keys, change_last_to_for_keys_with_multiple_elements) {
    auto ks = create_keys(2);
    auto branch_to_be_replaced = ks.get_last_branch();

    ks.change_last_to(Branch<TestNode>(9, new TestNode(9), new TestNode(10)));

    ASSERT_EQ(2, ks.size());
    ASSERT_EQ(1, ks.get_branch(0));
    ASSERT_EQ(1, ks.get_branch(0).left->id);
    ASSERT_EQ(9, ks.get_branch(0).right->id);
    ASSERT_EQ(9, ks.get_branch(1));
    ASSERT_EQ(9, ks.get_branch(1).left->id);
    ASSERT_EQ(10, ks.get_branch(1).right->id);

    destroy_keys(ks);
    delete branch_to_be_replaced.left;
    delete branch_to_be_replaced.right;
}


TEST(MeasurableTree, no_children) {
    MeasurableTree t;
    t.walk();
    ASSERT_EQ(0, Measurable::deepest);
    ASSERT_EQ(0, Measurable::shallowest);
}

TEST(MeasurableTree, list) {
    MeasurableTree t;
    t.l = new MeasurableTree();
    t.l->l = new MeasurableTree();
    t.l->l->l = new MeasurableTree();
    t.walk();
    ASSERT_EQ(3, Measurable::deepest);
    ASSERT_EQ(0, Measurable::shallowest);
}

TEST(MeasurableTree, 1_node_here_1_there) {
    MeasurableTree t;
    t.l = new MeasurableTree();
    t.r = new MeasurableTree();
    t.walk();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(MeasurableTree, 2_node_here_1_there) {
    MeasurableTree t;
    t.l = new MeasurableTree();
    t.l->l = new MeasurableTree();
    t.r = new MeasurableTree();
    t.walk();
    ASSERT_EQ(2, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, only_root_deepest_and_shallowest_branches) {
    MeasurableBtree<2> t;
    t.walk();
    ASSERT_EQ(0, Measurable::deepest);
    ASSERT_EQ(0, Measurable::shallowest);
}

TEST(Btree, add_elements_to_root) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    ASSERT_EQ(1, t.get_keys()[0]);
    ASSERT_EQ(2, t.get_keys()[1]);

    t.walk();
    ASSERT_EQ(0, Measurable::deepest);
    ASSERT_EQ(0, Measurable::shallowest);
}

TEST(Btree, elements_added_to_root_are_sorted) {
    MeasurableBtree<2> t;
    t.add(2);
    t.add(1);
    ASSERT_EQ(1, t.get_keys()[0]);
    ASSERT_EQ(2, t.get_keys()[1]);

    t.walk();
    ASSERT_EQ(0, Measurable::deepest);
    ASSERT_EQ(0, Measurable::shallowest);
}

TEST(Btree, grow) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
    ASSERT_EQ(2, t.get_keys()[0]);

    t.walk();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, find_element_which_is_not_present) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
    t.find_node_with_key(4);
    ASSERT_EQ(nullptr, t.find_node_with_key(4));

    t.walk();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, find_element_in_root) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
    t.find_node_with_key(4);
    ASSERT_EQ(2, t.find_node_with_key(2)->get_keys()[0]);

    t.walk();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, find_element_in_left_leaf) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
    ASSERT_EQ(1, t.find_node_with_key(1)->get_keys()[0]);

    t.walk();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, find_element_in_right_leaf) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
    t.find_node_with_key(3)->get_keys();
    ASSERT_EQ(3, t.find_node_with_key(3)->get_keys()[0]);

    t.walk();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
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

TEST(Btree, dump) {
    incremental_test(3);
}

TEST(Btree, finds_place_of_new_element_in_the_tree) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
    t.add(4);

    auto result = t.dump();
    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(3, result[2]);
    ASSERT_EQ(4, result[3]);

    auto n = t.find_node_with_key(4);
    ASSERT_EQ(3, n->get_keys()[0]);
    ASSERT_EQ(4, n->get_keys()[1]);

    t.walk();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, adding_new_element_moves_element_up_from_leaf) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
    t.add(4);
    t.add(5);

    auto result = t.dump();
    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(3, result[2]);
    ASSERT_EQ(4, result[3]);
    ASSERT_EQ(5, result[4]);

    auto n = t.find_node_with_key(4);
    ASSERT_EQ(2, n->get_keys()[0]);
    ASSERT_EQ(4, n->get_keys()[1]);

    t.walk();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, adding_new_element_moves_element_up_from_leaf_and_grows_tree) {
    incremental_test(7);
}

TEST(Btree, big_tree) {
    incremental_test(100);
}

void decremental_test(size_t num_of_elems)
{
    MeasurableBtree<2> t;
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

TEST(Btree, decrement_adding_grow) {
    decremental_test(3);
}

TEST(Btree, decrement_find_node) {
    decremental_test(4);
}

TEST(Btree, decrement_upwards_add) {
    decremental_test(5);
}

TEST(Btree, decrement_big_tree) {
    decremental_test(100);
}

TEST(Btree, mixed_grow) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(20);
    t.add(2);

    auto result = t.dump();

    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(20, result[2]);

    t.walk();
    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}

TEST(Btree, mixed_find) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(20);
    t.add(2);
    t.add(19);

    auto result = t.dump();

    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(19, result[2]);
    ASSERT_EQ(20, result[3]);

    t.walk();
    ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
}

TEST(Btree, mixed_recursive_grow) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(20);
    t.add(2);
    t.add(19);
    t.add(3);
    t.add(18);
    t.add(4);

    auto result = t.dump();

    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(3, result[2]);
    ASSERT_EQ(4, result[3]);
    ASSERT_EQ(18, result[4]);
    ASSERT_EQ(19, result[5]);
    ASSERT_EQ(20, result[6]);

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

TEST(Btree, mixed_big) {
    test_mixed(100);
}

TEST(Btree, full_grow) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(101);
    t.add(1001);
    t.add(2);
    t.add(1002);
    t.add(102);

    auto result = t.dump();
    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(101, result[2]);
    ASSERT_EQ(102, result[3]);
    ASSERT_EQ(1001, result[4]);
    ASSERT_EQ(1002, result[5]);

    t.walk();
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

TEST(Btree, duplicate_key_root) {
    MeasurableBtree<2> t;
    t.add(0);
    ASSERT_THROW(t.add(0), invalid_key_exception);
}

TEST(Btree, duplicate_key_in_leaf) {
    MeasurableBtree<2> t = tree_with_incremental_elements(9);

    ASSERT_THROW(t.add(1), invalid_key_exception);
}

TEST(Btree, duplicate_key_in_node) {
    MeasurableBtree<2> t = tree_with_incremental_elements(9);

    ASSERT_THROW(t.add(5), invalid_key_exception);
}

TEST(Btree, duplicate_key_in_root_recursive) {
    MeasurableBtree<2> t = tree_with_incremental_elements(9);

    ASSERT_THROW(t.add(3), invalid_key_exception);
}

TEST(Btree, random_big) {
    test_random(100);
}

TEST(Btree, preorder_walk) {
    MeasurableBtree<2> t = tree_with_incremental_elements(9);

    std::vector<int> result;
    t.preorder_walk([&result] (int k) {
        result.push_back(k);
    });

    ASSERT_EQ(3, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(0, result[2]);
    ASSERT_EQ(2, result[3]);
    ASSERT_EQ(5, result[4]);
    ASSERT_EQ(7, result[5]);
    ASSERT_EQ(4, result[6]);
    ASSERT_EQ(6, result[7]);
    ASSERT_EQ(8, result[8]);
}

TEST(Btree, postorder_walk) {
    MeasurableBtree<2> t = tree_with_incremental_elements(9);

    std::vector<int> result;
    t.postorder_walk([&result] (int k) {
        result.push_back(k);
    });

    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(1, result[2]);
    ASSERT_EQ(4, result[3]);
    ASSERT_EQ(6, result[4]);
    ASSERT_EQ(8, result[5]);
    ASSERT_EQ(5, result[6]);
    ASSERT_EQ(7, result[7]);
    ASSERT_EQ(3, result[8]);
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

TEST(Btree, odd_degree_root) {
    test_incremental_with_odd_degree_and_num_of_elems(3);
}

TEST(Btree, odd_degree_incremental_grow) {
    test_incremental_with_odd_degree_and_num_of_elems(4);
}

TEST(Btree, odd_degree_incremental_big) {
    test_incremental_with_odd_degree_and_num_of_elems(100);
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

TEST(Btree, odd_degree_decremental_grow) {
    odd_decremental_test(4);
}

TEST(Btree, odd_degree_decremental_big) {
    odd_decremental_test(100);
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

TEST(Btree, odd_degree_mixed_grow) {
    test_odd_mixed(2);
}

TEST(Btree, odd_degree_mixed_recursive_grow) {
    test_odd_mixed(5);
}

TEST(Btree, odd_degree_mixed_big) {
    test_odd_mixed(100);
}

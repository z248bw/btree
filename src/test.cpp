#include "btree.h"
#include "gtest/gtest.h"


TEST(Test, should_pass_if_two_strings_are_equal_by_value) {
    const char* s1 = "kaucsuk rece";
    ASSERT_STREQ("kaucsuk rece", s1);
}

TEST(TraversableTree, no_children) {
    TraversableTree t;
    t.walk();
    ASSERT_EQ(0, Traversable::deepest);
    ASSERT_EQ(0, Traversable::shallowest);
}

TEST(TraversableTree, list) {
    TraversableTree t;
    t.l = new TraversableTree();
    t.l->l = new TraversableTree();
    t.l->l->l = new TraversableTree();
    t.walk();
    ASSERT_EQ(3, Traversable::deepest);
    ASSERT_EQ(0, Traversable::shallowest);
}

TEST(TraversableTree, 1_node_here_1_there) {
    TraversableTree t;
    t.l = new TraversableTree();
    t.r = new TraversableTree();
    t.walk();
    ASSERT_EQ(1, Traversable::deepest);
    ASSERT_EQ(1, Traversable::shallowest);
}

TEST(TraversableTree, 2_node_here_1_there) {
    TraversableTree t;
    t.l = new TraversableTree();
    t.l->l = new TraversableTree();
    t.r = new TraversableTree();
    t.walk();
    ASSERT_EQ(2, Traversable::deepest);
    ASSERT_EQ(1, Traversable::shallowest);
}

TEST(Btree, only_root_deepest_and_shallowest_branches) {
    Btree t;
    t.walk();
    ASSERT_EQ(0, Traversable::deepest);
    ASSERT_EQ(0, Traversable::shallowest);
}

TEST(Btree, add_elements_to_root) {
    Btree t;
    t.add(1);
    t.add(2);
    ASSERT_EQ(1, t.get_keys()[0]);
    ASSERT_EQ(2, t.get_keys()[1]);

    t.walk();
    ASSERT_EQ(0, Traversable::deepest);
    ASSERT_EQ(0, Traversable::shallowest);
}

TEST(Btree, elements_added_to_root_are_sorted) {
    Btree t;
    t.add(2);
    t.add(1);
    ASSERT_EQ(1, t.get_keys()[0]);
    ASSERT_EQ(2, t.get_keys()[1]);

    t.walk();
    ASSERT_EQ(0, Traversable::deepest);
    ASSERT_EQ(0, Traversable::shallowest);
}

TEST(Btree, grow) {
    Btree t;
    t.add(1);
    t.add(2);
    t.add(3);
    ASSERT_EQ(2, t.get_keys()[0]);

    t.walk();
    ASSERT_EQ(1, Traversable::deepest);
    ASSERT_EQ(1, Traversable::shallowest);
}

TEST(Btree, find_element_which_is_not_present) {
    Btree t;
    t.add(1);
    t.add(2);
    t.add(3);
    t.find_node_with_key(4);
    ASSERT_EQ(nullptr, t.find_node_with_key(4));

    t.walk();
    ASSERT_EQ(1, Traversable::deepest);
    ASSERT_EQ(1, Traversable::shallowest);
}

TEST(Btree, find_element_in_root) {
    Btree t;
    t.add(1);
    t.add(2);
    t.add(3);
    t.find_node_with_key(4);
    ASSERT_EQ(2, t.find_node_with_key(2)->get_keys()[0]);

    t.walk();
    ASSERT_EQ(1, Traversable::deepest);
    ASSERT_EQ(1, Traversable::shallowest);
}

TEST(Btree, find_element_in_left_leaf) {
    Btree t;
    t.add(1);
    t.add(2);
    t.add(3);
    ASSERT_EQ(1, t.find_node_with_key(1)->get_keys()[0]);

    t.walk();
    ASSERT_EQ(1, Traversable::deepest);
    ASSERT_EQ(1, Traversable::shallowest);
}

TEST(Btree, find_element_in_right_leaf) {
    Btree t;
    t.add(1);
    t.add(2);
    t.add(3);
    t.find_node_with_key(3)->get_keys();
    ASSERT_EQ(3, t.find_node_with_key(3)->get_keys()[0]);

    t.walk();
    ASSERT_EQ(1, Traversable::deepest);
    ASSERT_EQ(1, Traversable::shallowest);
}

void incremental_test(size_t num_of_elems)
{
    Btree t;
    for (size_t i = 0; i < num_of_elems; i++)
    {
        t.add(i);
    }

    auto result = t.dump();
    for (size_t i = 0; i < result.size(); i++)
    {
        ASSERT_EQ(i, result[i]);
    }

    t.walk();
    ASSERT_EQ(Traversable::shallowest, Traversable::deepest);
}

TEST(Btree, dump) {
    incremental_test(3);
}

TEST(Btree, finds_place_of_new_element_in_the_tree) {
    Btree t;
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
    ASSERT_EQ(1, Traversable::deepest);
    ASSERT_EQ(1, Traversable::shallowest);
}

TEST(Btree, adding_new_element_moves_element_up_from_leaf) {
    Btree t;
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
    ASSERT_EQ(1, Traversable::deepest);
    ASSERT_EQ(1, Traversable::shallowest);
}

TEST(Btree, adding_new_element_moves_element_up_from_leaf_and_grows_tree) {
    incremental_test(7);
}

TEST(Btree, big_tree) {
    incremental_test(100);
}

void decremental_test(size_t num_of_elems)
{
    Btree t;
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
    ASSERT_EQ(Traversable::shallowest, Traversable::deepest);
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
    Btree t;
    t.add(1);
    t.add(20);
    t.add(2);

    auto result = t.dump();

    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(20, result[2]);

    t.walk();
    ASSERT_EQ(Traversable::shallowest, Traversable::deepest);
}

TEST(Btree, mixed_find) {
    Btree t;
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
    ASSERT_EQ(Traversable::shallowest, Traversable::deepest);
}

TEST(Btree, mixed_recursive_grow) {
    Btree t;
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
    ASSERT_EQ(Traversable::shallowest, Traversable::deepest);
}

void test_mixed(size_t n)
{
    auto top = 2 * n + 10;

    Btree t;
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

    ASSERT_EQ(Traversable::shallowest, Traversable::deepest);
}

TEST(Btree, mixed_big) {
    test_mixed(100);
}

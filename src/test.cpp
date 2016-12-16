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


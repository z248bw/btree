#include "btree.h"
#include "gtest/gtest.h"


TEST(Test, should_pass_if_two_strings_are_equal_by_value) {
    const char* s1 = "kaucsuk rece";
    ASSERT_STREQ("kaucsuk rece", s1);
}

TEST(Btree, only_root_deepest_and_shallowest_branches) {
    Btree t;
    t.walk();
    ASSERT_EQ(0, Traversable::deepest);
    ASSERT_EQ(0, Traversable::shallowest);
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

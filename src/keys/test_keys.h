#ifndef TEST_KEYS_H_
#define TEST_KEYS_H_

#include "gtest/gtest.h"

#include "keys_test_utils.h"


TEST(Keys, storeBranchesInOrder) {
    auto keys_factory = KeysFactoryRAII();
    auto ks = keys_factory.create_keys(3);

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
}

TEST(Keys, addNewBranchAtBeginning) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(1);

    ks.add(Branch<TestNode>(-1, test_node_factory.create(-1), test_node_factory.create(-2)));

    auto b1 = ks.get_branch(0);
    auto b2 = ks.get_branch(1);

    ASSERT_EQ(-1, b1.value);
    ASSERT_EQ(-1, b1.left->id);
    ASSERT_EQ(-2, b1.right->id);
    ASSERT_EQ(1, b2.value);
    ASSERT_EQ(-2, b2.left->id);
    ASSERT_EQ(2, b2.right->id);
}

TEST(Keys, addNewBranchAtEnd) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(1);

    ks.add(Branch<TestNode>(9, test_node_factory.create(9), test_node_factory.create(10)));

    auto b1 = ks.get_branch(0);
    auto b2 = ks.get_branch(1);

    ASSERT_EQ(1, b1.value);
    ASSERT_EQ(1, b1.left->id);
    ASSERT_EQ(9, b1.right->id);
    ASSERT_EQ(9, b2.value);
    ASSERT_EQ(9, b2.left->id);
    ASSERT_EQ(10, b2.right->id);
}

TEST(Keys, addNewBranchAtMiddle) {
    Keys<TestNode> ks(9);
    auto test_node_factory = TestNodeFactoryRAII();
    auto common_node = test_node_factory.create(100);
    ks.add(Branch<TestNode>(1, test_node_factory.create(1), common_node));
    ks.add(Branch<TestNode>(100, common_node, test_node_factory.create(101)));

    ks.add(Branch<TestNode>(9, test_node_factory.create(9), test_node_factory.create(10)));

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
}

TEST(Keys, isPresent) {
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

TEST(Keys, emptyGetMedianWithNewKey) {
    Keys<TestNode> ks(4);

    ASSERT_EQ(1, ks.get_median_with_new_key(1));
}

TEST(Keys, oneElementGetMedianWithNewKey) {
    Keys<TestNode> ks(4);

    ks.add(4);

    ASSERT_EQ(1, ks.get_median_with_new_key(1));
}

TEST(Keys, twoElementsGetMedianWithNewKey) {
    Keys<TestNode> ks(4);

    ks.add(2);
    ks.add(3);

    ASSERT_EQ(2, ks.get_median_with_new_key(1));
}

TEST(Keys, threeElementsGetMedianWithNewKey) {
    Keys<TestNode> ks(4);

    ks.add(2);
    ks.add(3);
    ks.add(4);

    ASSERT_EQ(2, ks.get_median_with_new_key(1));
}

TEST(Keys, evenGetLeftHalfOfKeys) {
    auto keys_factory = KeysFactoryRAII();
    auto ks = keys_factory.create_keys(2);

    auto half = ks.get_left_half_of_keys();

    ASSERT_EQ(1, half.size());
    ASSERT_EQ(1, half.get_branch(0));
    ASSERT_EQ(1, half.get_branch(0).left->id);
    ASSERT_EQ(2, half.get_branch(0).right->id);
}

TEST(Keys, evenGetRightHalfOfKeys) {
    auto keys_factory = KeysFactoryRAII();
    auto ks = keys_factory.create_keys(2);

    auto half = ks.get_right_half_of_keys();

    ASSERT_EQ(1, half.size());
    ASSERT_EQ(2, half.get_branch(0));
    ASSERT_EQ(2, half.get_branch(0).left->id);
    ASSERT_EQ(3, half.get_branch(0).right->id);
}

TEST(Keys, oddGetLeftHalfOfKeys) {
    auto keys_factory = KeysFactoryRAII();
    auto ks = keys_factory.create_keys(3);

    auto half = ks.get_left_half_of_keys();

    ASSERT_EQ(1, half.size());
    ASSERT_EQ(1, half.get_branch(0));
    ASSERT_EQ(1, half.get_branch(0).left->id);
    ASSERT_EQ(2, half.get_branch(0).right->id);
}

TEST(Keys, oddGetRightHalfOfKeys) {
    auto keys_factory = KeysFactoryRAII();
    auto ks = keys_factory.create_keys(3);

    auto half = ks.get_right_half_of_keys();

    ASSERT_EQ(2, half.size());
    ASSERT_EQ(2, half.get_branch(0));
    ASSERT_EQ(2, half.get_branch(0).left->id);
    ASSERT_EQ(3, half.get_branch(0).right->id);
    ASSERT_EQ(3, half.get_branch(1));
    ASSERT_EQ(3, half.get_branch(1).left->id);
    ASSERT_EQ(4, half.get_branch(1).right->id);
}

TEST(Keys, changeFirstToForKeysWithOneElement) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(1);

    ks.change_first_to(Branch<TestNode>(9, test_node_factory.create(9), test_node_factory.create(10)));

    ASSERT_EQ(1, ks.size());
    ASSERT_EQ(9, ks.get_branch(0));
    ASSERT_EQ(9, ks.get_branch(0).left->id);
    ASSERT_EQ(10, ks.get_branch(0).right->id);
}

TEST(Keys, changeFirstToForKeysWithMultipleElements) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(2);

    ks.change_first_to(Branch<TestNode>(9, test_node_factory.create(9), test_node_factory.create(10)));

    ASSERT_EQ(2, ks.size());
    ASSERT_EQ(2, ks.get_branch(0));
    ASSERT_EQ(2, ks.get_branch(0).left->id);
    ASSERT_EQ(9, ks.get_branch(0).right->id);
    ASSERT_EQ(9, ks.get_branch(1));
    ASSERT_EQ(9, ks.get_branch(1).left->id);
    ASSERT_EQ(10, ks.get_branch(1).right->id);
}

TEST(Keys, changeLastToForKeysWithOneElement) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(1);

    ks.change_last_to(Branch<TestNode>(9, test_node_factory.create(9), test_node_factory.create(10)));

    ASSERT_EQ(1, ks.size());
    ASSERT_EQ(9, ks.get_branch(0));
    ASSERT_EQ(9, ks.get_branch(0).left->id);
    ASSERT_EQ(10, ks.get_branch(0).right->id);
}

TEST(Keys, changeLastToForKeysWithMultipleElements) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(2);

    ks.change_last_to(Branch<TestNode>(9, test_node_factory.create(9), test_node_factory.create(10)));

    ASSERT_EQ(2, ks.size());
    ASSERT_EQ(1, ks.get_branch(0));
    ASSERT_EQ(1, ks.get_branch(0).left->id);
    ASSERT_EQ(9, ks.get_branch(0).right->id);
    ASSERT_EQ(9, ks.get_branch(1));
    ASSERT_EQ(9, ks.get_branch(1).left->id);
    ASSERT_EQ(10, ks.get_branch(1).right->id);
}

#endif

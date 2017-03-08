#ifndef TEST_KEYS_H_
#define TEST_KEYS_H_

#include <string>

#include "gtest/gtest.h"

#include "keys/keys.h"
#include "keys_test_utils.h"


TEST(Keys, storeBranchesInOrder) {
    auto keys_factory = KeysFactoryRAII();
    auto ks = keys_factory.create_keys(3);

    auto b1 = ks.get_branch(0);
    auto b2 = ks.get_branch(1);
    auto b3 = ks.get_branch(2);

    ASSERT_EQ(1, b1.value);
    ASSERT_EQ(1, b1.left->kv);
    ASSERT_EQ(2, b1.right->kv);
    ASSERT_EQ(2, b2.value);
    ASSERT_EQ(2, b2.left->kv);
    ASSERT_EQ(3, b2.right->kv);
    ASSERT_EQ(3, b3.value);
    ASSERT_EQ(3, b3.left->kv);
    ASSERT_EQ(4, b3.right->kv);
}

TEST(Keys, addNewBranchAtBeginning) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(1);

    ks.add(Branch<TestNode<>>(get_kv(-1), test_node_factory.create(-1), test_node_factory.create(-2)));

    auto b1 = ks.get_branch(0);
    auto b2 = ks.get_branch(1);

    ASSERT_EQ(-1, b1.value);
    ASSERT_EQ(-1, b1.left->kv);
    ASSERT_EQ(-2, b1.right->kv);
    ASSERT_EQ(1, b2.value);
    ASSERT_EQ(-2, b2.left->kv);
    ASSERT_EQ(2, b2.right->kv);
}

TEST(Keys, addNewBranchAtEnd) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(1);

    ks.add(Branch<TestNode<>>(get_kv(9), test_node_factory.create(9), test_node_factory.create(10)));

    auto b1 = ks.get_branch(0);
    auto b2 = ks.get_branch(1);

    ASSERT_EQ(1, b1.value);
    ASSERT_EQ(1, b1.left->kv);
    ASSERT_EQ(9, b1.right->kv);
    ASSERT_EQ(9, b2.value);
    ASSERT_EQ(9, b2.left->kv);
    ASSERT_EQ(10, b2.right->kv);
}

TEST(Keys, addNewBranchAtMiddle) {
    Keys<TestNode<>> ks(9);
    auto test_node_factory = TestNodeFactoryRAII();
    auto common_node = test_node_factory.create(100);
    ks.add(Branch<TestNode<>>(get_kv(1), test_node_factory.create(1), common_node));
    ks.add(Branch<TestNode<>>(get_kv(100), common_node, test_node_factory.create(101)));

    ks.add(Branch<TestNode<>>(get_kv(9), test_node_factory.create(9), test_node_factory.create(10)));

    auto b1 = ks.get_branch(0);
    auto b2 = ks.get_branch(1);
    auto b3 = ks.get_branch(2);

    ASSERT_EQ(1, b1.value);
    ASSERT_EQ(1, b1.left->kv);
    ASSERT_EQ(9, b1.right->kv);
    ASSERT_EQ(9, b2.value);
    ASSERT_EQ(9, b2.left->kv);
    ASSERT_EQ(10, b2.right->kv);
    ASSERT_EQ(100, b3.value);
    ASSERT_EQ(10, b3.left->kv);
    ASSERT_EQ(101, b3.right->kv);
}

TEST(Keys, isPresent) {
    Keys<TestNode<>> ks(4);

    ks.add(get_kv(3));
    ks.add(get_kv(5));
    ks.add(get_kv(2));
    ks.add(get_kv(1));
    ks.add(get_kv(4));

    ASSERT_FALSE(ks.is_present(6));
    ASSERT_TRUE(ks.is_present(3));
    ASSERT_TRUE(ks.is_present(1));
    ASSERT_TRUE(ks.is_present(5));
}

TEST(Keys, emptyGetMedianWithNewKey) {
    Keys<TestNode<>> ks(4);

    ASSERT_EQ(1, ks.get_median_KV_with_new_key(get_kv(1)));
}

TEST(Keys, oneElementGetMedianWithNewKey) {
    Keys<TestNode<>> ks(4);

    ks.add(get_kv(4));

    ASSERT_EQ(1, ks.get_median_KV_with_new_key(get_kv(1)));
}

TEST(Keys, twoElementsGetMedianWithNewKey) {
    Keys<TestNode<>> ks(4);

    ks.add(get_kv(2));
    ks.add(get_kv(3));

    ASSERT_EQ(2, ks.get_median_KV_with_new_key(get_kv(1)));
}

TEST(Keys, threeElementsGetMedianWithNewKey) {
    Keys<TestNode<>> ks(4);

    ks.add(get_kv(2));
    ks.add(get_kv(3));
    ks.add(get_kv(4));

    ASSERT_EQ(2, ks.get_median_KV_with_new_key(get_kv(1)));
}

TEST(Keys, evenGetLeftHalfOfKeys) {
    auto keys_factory = KeysFactoryRAII();
    auto ks = keys_factory.create_keys(2);

    auto half = ks.get_left_half_of_keys();

    ASSERT_EQ(1, half.size());
    ASSERT_EQ(1, half.get_branch(0).value);
    ASSERT_EQ(1, half.get_branch(0).left->kv);
    ASSERT_EQ(2, half.get_branch(0).right->kv);
}

TEST(Keys, evenGetRightHalfOfKeys) {
    auto keys_factory = KeysFactoryRAII();
    auto ks = keys_factory.create_keys(2);

    auto half = ks.get_right_half_of_keys();

    ASSERT_EQ(1, half.size());
    ASSERT_EQ(2, half.get_branch(0).value);
    ASSERT_EQ(2, half.get_branch(0).left->kv);
    ASSERT_EQ(3, half.get_branch(0).right->kv);
}

TEST(Keys, oddGetLeftHalfOfKeys) {
    auto keys_factory = KeysFactoryRAII();
    auto ks = keys_factory.create_keys(3);

    auto half = ks.get_left_half_of_keys();

    ASSERT_EQ(1, half.size());
    ASSERT_EQ(1, half.get_branch(0).value);
    ASSERT_EQ(1, half.get_branch(0).left->kv);
    ASSERT_EQ(2, half.get_branch(0).right->kv);
}

TEST(Keys, oddGetRightHalfOfKeys) {
    auto keys_factory = KeysFactoryRAII();
    auto ks = keys_factory.create_keys(3);

    auto half = ks.get_right_half_of_keys();

    ASSERT_EQ(2, half.size());
    ASSERT_EQ(2, half.get_branch(0).value);
    ASSERT_EQ(2, half.get_branch(0).left->kv);
    ASSERT_EQ(3, half.get_branch(0).right->kv);
    ASSERT_EQ(3, half.get_branch(1).value);
    ASSERT_EQ(3, half.get_branch(1).left->kv);
    ASSERT_EQ(4, half.get_branch(1).right->kv);
}

TEST(Keys, changeFirstToForKeysWithOneElement) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(1);

    ks.change_first_to(
        Branch<TestNode<>>(get_kv(9), test_node_factory.create(9), test_node_factory.create(10))
    );

    ASSERT_EQ(1, ks.size());
    ASSERT_EQ(9, ks.get_branch(0).value);
    ASSERT_EQ(9, ks.get_branch(0).left->kv);
    ASSERT_EQ(10, ks.get_branch(0).right->kv);
}

TEST(Keys, changeFirstToForKeysWithMultipleElements) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(2);

    ks.change_first_to(
        Branch<TestNode<>>(get_kv(9), test_node_factory.create(9), test_node_factory.create(10))
    );

    ASSERT_EQ(2, ks.size());
    ASSERT_EQ(2, ks.get_branch(0).value);
    ASSERT_EQ(2, ks.get_branch(0).left->kv);
    ASSERT_EQ(9, ks.get_branch(0).right->kv);
    ASSERT_EQ(9, ks.get_branch(1).value);
    ASSERT_EQ(9, ks.get_branch(1).left->kv);
    ASSERT_EQ(10, ks.get_branch(1).right->kv);
}

TEST(Keys, changeLastToForKeysWithOneElement) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(1);

    ks.change_last_to(
        Branch<TestNode<>>(get_kv(9), test_node_factory.create(9), test_node_factory.create(10))
    );

    ASSERT_EQ(1, ks.size());
    ASSERT_EQ(9, ks.get_branch(0).value);
    ASSERT_EQ(9, ks.get_branch(0).left->kv);
    ASSERT_EQ(10, ks.get_branch(0).right->kv);
}

TEST(Keys, changeLastToForKeysWithMultipleElements) {
    auto keys_factory = KeysFactoryRAII();
    auto test_node_factory = TestNodeFactoryRAII();
    auto ks = keys_factory.create_keys(2);

    ks.change_last_to(
        Branch<TestNode<>>(get_kv(9), test_node_factory.create(9), test_node_factory.create(10))
    );

    ASSERT_EQ(2, ks.size());
    ASSERT_EQ(1, ks.get_branch(0).value);
    ASSERT_EQ(1, ks.get_branch(0).left->kv);
    ASSERT_EQ(9, ks.get_branch(0).right->kv);
    ASSERT_EQ(9, ks.get_branch(1).value);
    ASSERT_EQ(9, ks.get_branch(1).left->kv);
    ASSERT_EQ(10, ks.get_branch(1).right->kv);
}

TEST(Keys, getValueForEmptyKeys) {
    Keys<TestNode<>> ks(10);

    ASSERT_FALSE(ks.find_and_get_value(99).is_present);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
    ASSERT_THROW(static_cast<const char*>(ks.find_and_get_value(99)), key_does_not_exist_exception);
#pragma GCC diagnostic pop
}

TEST(Keys, getValueForNoneExistantKey) {
    auto keys_factory = KeysFactoryRAII();
    auto ks = keys_factory.create_keys(2);

    ASSERT_FALSE(ks.find_and_get_value(99).is_present);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
    ASSERT_THROW(static_cast<const char*>(ks.find_and_get_value(99)), key_does_not_exist_exception);
#pragma GCC diagnostic pop
}

TEST(Keys, getValue) {
    Keys<TestNode<int, std::string>> ks(2);

    ks.add(KeyValue<int, std::string>(3, "a"));

    ASSERT_TRUE(ks.find_and_get_value(3).is_present);
    ASSERT_EQ("a", static_cast<std::string>(ks.find_and_get_value(3)));
}

TEST(Keys, getValueReturnsReference) {
    Keys<TestNode<int, std::string>> ks(2);
    ks.add(KeyValue<int, std::string>(3, "a"));

    std::string& ref = ks.find_and_get_value(3);
    ref = "b";

    ASSERT_TRUE(ks.find_and_get_value(3).is_present);
    ASSERT_EQ("b", static_cast<std::string>(ks.find_and_get_value(3)));
}

#endif

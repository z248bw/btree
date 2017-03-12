#ifndef TEST_MEASURABLE_H_
#define TEST_MEASURABLE_H_

#include "gtest/gtest.h"

#include "measurable.hpp"
#include "measurable_test_utils.hpp"


using namespace btree;

TEST(Test, shouldPassIfTwoStringsAreEqualByValue) {
    const char* s1 = "kaucsuk rece";
    ASSERT_STREQ("kaucsuk rece", s1);
}


TEST(MeasurableTree, noChildren) {
    MeasurableTree t;
    t.measure();
    ASSERT_EQ(0, Measurable::deepest);
    ASSERT_EQ(0, Measurable::shallowest);
}

TEST(MeasurableTree, list) {
    MeasurableTree t;
    t.l = new MeasurableTree();
    t.l->l = new MeasurableTree();
    t.l->l->l = new MeasurableTree();
    t.measure();
    ASSERT_EQ(3, Measurable::deepest);
    ASSERT_EQ(0, Measurable::shallowest);
}

TEST(MeasurableTree, 1NodeHere_1There) {
    MeasurableTree t;
    t.l = new MeasurableTree();
    t.r = new MeasurableTree();
    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(MeasurableTree, 2NodeHere_1There) {
    MeasurableTree t;
    t.l = new MeasurableTree();
    t.l->l = new MeasurableTree();
    t.r = new MeasurableTree();
    t.measure();
    ASSERT_EQ(2, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}


TEST(Btree, onlyRootDeepestAndShallowestBranches) {
    MeasurableBtree<2> t;
    check_balance(t);
}

TEST(Btree, addElementsToRoot) {
    MeasurableBtree<2> t;
    t.add(1, "hello");
    t.add(2, "world");
    ASSERT_EQ(1, t.get_keys()[0]);
    ASSERT_EQ(2, t.get_keys()[1]);

    t.measure();
    ASSERT_EQ(0, Measurable::deepest);
    ASSERT_EQ(0, Measurable::shallowest);
}

TEST(Btree, elementsAddedToRootAreSorted) {
    MeasurableBtree<2> t;
    t.add(2, "world");
    t.add(1, "hello");
    ASSERT_EQ(1, t.get_keys()[0]);
    ASSERT_EQ(2, t.get_keys()[1]);

    t.measure();
    ASSERT_EQ(0, Measurable::deepest);
    ASSERT_EQ(0, Measurable::shallowest);
}

TEST(Btree, grow) {
    MeasurableBtree<2> t;
    t.add(1, "hello");
    t.add(2, "world");
    t.add(3, "world");
    ASSERT_EQ(2, t.get_keys()[0]);

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, findElementWhichIsNotPresent) {
    MeasurableBtree<2> t;
    t.add(1, "hello");
    t.add(2, "world");
    t.add(3, "world");
    t.find_node_with_key(4);
    ASSERT_EQ(nullptr, t.find_node_with_key(4));

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, findElementInRoot) {
    MeasurableBtree<2> t;
    t.add(1, "hello");
    t.add(2, "world");
    t.add(3, "world");
    t.find_node_with_key(4);
    ASSERT_EQ(2, t.find_node_with_key(2)->get_keys()[0]);

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, findElementInLeftLeaf) {
    MeasurableBtree<2> t;
    t.add(1, "hello");
    t.add(2, "world");
    t.add(3, "world");
    ASSERT_EQ(1, t.find_node_with_key(1)->get_keys()[0]);

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, findElementInRightLeaf) {
    MeasurableBtree<2> t;
    t.add(1, "hello");
    t.add(2, "world");
    t.add(3, "world");
    t.find_node_with_key(3)->get_keys();
    ASSERT_EQ(3, t.find_node_with_key(3)->get_keys()[0]);

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, dump) {
    test_incremental<2>(3);
}

TEST(Btree, findsPlaceOfNewElementInTheTree) {
    MeasurableBtree<2> t;
    t.add(1, "hello");
    t.add(2, "world");
    t.add(3, "world");
    t.add(4, "world");

    auto result = t.dump();
    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(3, result[2]);
    ASSERT_EQ(4, result[3]);

    auto n = t.find_node_with_key(4);
    ASSERT_EQ(3, n->get_keys()[0]);
    ASSERT_EQ(4, n->get_keys()[1]);

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, addingNewElementMovesElementUpFromLeaf) {
    auto t = tree_with_incremental_elements<2>(5);

    auto result = t.dump();
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);

    auto n = t.find_node_with_key(3);
    ASSERT_EQ(1, n->get_keys()[0]);
    ASSERT_EQ(3, n->get_keys()[1]);

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, addingNewElementMovesElementUpFromLeafAndGrowsTree) {
    test_incremental<2>(7);
}

TEST(Btree, bigTree) {
    test_incremental<2>(100);
}

TEST(Btree, decrementAddingGrow) {
    test_decremental<2>(3);
}

TEST(Btree, decrementFindNode) {
    test_decremental<2>(4);
}

TEST(Btree, decrementUpwardsAdd) {
    test_decremental<2>(5);
}

TEST(Btree, decrementBigTree) {
    test_decremental<2>(100);
}

TEST(Btree, mixedGrow) {
    MeasurableBtree<2> t;
    t.add(1, "hello");
    t.add(20, "world");
    t.add(2, "world");

    auto result = t.dump();

    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(20, result[2]);

    check_balance(t);
}

TEST(Btree, mixedFind) {
    MeasurableBtree<2> t;
    t.add(1, "hello");
    t.add(20, "world");
    t.add(2, "world");
    t.add(19, "world");

    auto result = t.dump();

    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(19, result[2]);
    ASSERT_EQ(20, result[3]);

    check_balance(t);
}

TEST(Btree, mixedRecursiveGrow) {
    MeasurableBtree<2> t;
    t.add(1, "hello");
    t.add(20, "world");
    t.add(2, "world");
    t.add(19, "world");
    t.add(3, "world");
    t.add(18, "world");
    t.add(4, "world");

    auto result = t.dump();

    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(3, result[2]);
    ASSERT_EQ(4, result[3]);
    ASSERT_EQ(18, result[4]);
    ASSERT_EQ(19, result[5]);
    ASSERT_EQ(20, result[6]);

    check_balance(t);
}

TEST(Btree, mixedBig) {
    test_mixed<2>(100);
}

TEST(Btree, fullGrow) {
    MeasurableBtree<2> t;
    t.add(1, "hello");
    t.add(101, "world");
    t.add(1001, "world");
    t.add(2, "world");
    t.add(1002, "world");
    t.add(102, "world");

    auto result = t.dump();
    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(101, result[2]);
    ASSERT_EQ(102, result[3]);
    ASSERT_EQ(1001, result[4]);
    ASSERT_EQ(1002, result[5]);

    check_balance(t);
}

TEST(Btree, duplicateKeyRoot) {
    MeasurableBtree<2> t;
    t.add(0, "world");
    ASSERT_THROW(t.add(0, "hello"), duplicated_key_exception);
}

TEST(Btree, duplicateKeyInLeaf) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(9);

    ASSERT_THROW(t.add(1, "hello"), duplicated_key_exception);
}

TEST(Btree, duplicateKeyInNode) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(9);

    ASSERT_THROW(t.add(5, "hello"), duplicated_key_exception);
}

TEST(Btree, duplicateKeyInRootRecursive) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(9);

    ASSERT_THROW(t.add(3, "hello"), duplicated_key_exception);
}

TEST(Btree, randomBig) {
    test_random<2>(100);
}

TEST(Btree, preorderWalk) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(9);

    std::vector<int> result;
    t.preorder_walk([&result] (KeyValue<int, const char*> k) {
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

TEST(Btree, postorderWalk) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(9);

    std::vector<int> result;
    t.postorder_walk([&result] (KeyValue<int, const char*> k) {
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

TEST(Btree, oddDegreeRoot) {
    test_incremental<3>(3);
}

TEST(Btree, oddDegree_incrementalGrow) {
    test_incremental<3>(4);
}

TEST(Btree, oddDegreeIncrementalBig) {
    test_incremental<3>(100);
}

TEST(Btree, oddDegreeDecrementalGrow) {
    test_decremental<3>(4);
}

TEST(Btree, oddDegreeDecrementalBig) {
    test_decremental<3>(100);
}

TEST(Btree, oddDegreeMixedGrow) {
    test_mixed<3>(2);
}

TEST(Btree, oddDegreeMixedRecursiveGrow) {
    test_mixed<3>(5);
}

TEST(Btree, oddDegreeMixedBig) {
    test_mixed<3>(100);
}

TEST(Btree, oddDegreeRandomBig) {
    test_random<3>(100);
}

TEST(Btree, copy_assignment) {
    MeasurableBtree<2> orig = tree_with_incremental_elements<2>(10);

    auto copy = orig;

    auto copied_elems = copy.dump();
    auto orig_elems = orig.dump();

    for (size_t i = 0; i < orig_elems.size(); i++)
    {
        ASSERT_EQ(copied_elems[i], orig_elems[i]);
    }
}

TEST(Btree, getValueForEmptyTreeShouldThrowException) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(0);

    ASSERT_THROW(t.get(99), key_does_not_exist_exception);
}

TEST(Btree, getValueForNonExistentKeyShouldThrowException) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(10);

    ASSERT_THROW(t.get(99), key_does_not_exist_exception);
}

TEST(Btree, getValueWithOneValueStored) {
    Btree<int, const char*, 2> t;

    t.add(1, "hello");

    ASSERT_STREQ("hello", t.get(1));
}

TEST(Btree, getValueWithMultipleValuesStored) {
    Btree<int, const char*, 2> t;

    t.add(1, "hello");
    t.add(2, "world");

    ASSERT_STREQ("world", t.get(2));
}

TEST(Btree, getValueFromBigTree) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(99);

    t.add(101, "hello");

    ASSERT_STREQ("hello", t.get(101));
}

TEST(Btree, getValueReturnsReference) {
    Btree<int, const char*, 2> t;
    t.add(1, "a");

    t.get(1) = "b";

    ASSERT_STREQ("b", t.get(1));
}

TEST(Btree, storeSimpleInt) {
    Btree<int, int, 2> t;
    t.add(1, 1);

    t.get(1) = 2;

    ASSERT_EQ(2, t.get(1));
}

TEST(Btree, storeIntPointer) {
    Btree<int, int*, 2> t;
    int p = 2;
    int new_p = 3;
    t.add(1, &p);

    t.get(1) = &new_p;

    ASSERT_EQ(new_p, *(t.get(1)));
}

TEST(Btree, storeObject) {
    Btree<int, std::vector<int>, 2> t;
    t.add(1, std::vector<int>(4, 9));

    t.get(1) = std::vector<int>(3, 8);

    ASSERT_EQ(3, t.get(1).size());
    ASSERT_EQ(8, t.get(1)[2]);
}

TEST(Btree, stringAsKey) {
    Btree<std::string, int, 2> t;
    t.add("hello", 1);

    t.get("hello") = 2;

    ASSERT_EQ(2, t.get("hello"));
}

TEST(Btree, multipleStringAsKey) {
    Btree<std::string, int, 2> t;
    t.add("a", 1);
    t.add("c", 1);
    t.add("b", 1);

    auto elems = t.dump();

    ASSERT_EQ("a", elems[0].key);
    ASSERT_EQ("b", elems[1].key);
    ASSERT_EQ("c", elems[2].key);
}


#endif

#ifndef TEST_MEASURABLE_H_
#define TEST_MEASURABLE_H_

#include "gtest/gtest.h"

#include "measurable.h"
#include "measurable_test_utils.h"



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
    t.add(1);
    t.add(2);
    ASSERT_EQ(1, t.get_keys()[0]);
    ASSERT_EQ(2, t.get_keys()[1]);

    t.measure();
    ASSERT_EQ(0, Measurable::deepest);
    ASSERT_EQ(0, Measurable::shallowest);
}

TEST(Btree, elementsAddedToRootAreSorted) {
    MeasurableBtree<2> t;
    t.add(2);
    t.add(1);
    ASSERT_EQ(1, t.get_keys()[0]);
    ASSERT_EQ(2, t.get_keys()[1]);

    t.measure();
    ASSERT_EQ(0, Measurable::deepest);
    ASSERT_EQ(0, Measurable::shallowest);
}

TEST(Btree, grow) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
    ASSERT_EQ(2, t.get_keys()[0]);

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, findElementWhichIsNotPresent) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
    t.find_node_with_key(4);
    ASSERT_EQ(nullptr, t.find_node_with_key(4));

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, findElementInRoot) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
    t.find_node_with_key(4);
    ASSERT_EQ(2, t.find_node_with_key(2)->get_keys()[0]);

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, findElementInLeftLeaf) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
    ASSERT_EQ(1, t.find_node_with_key(1)->get_keys()[0]);

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, findElementInRightLeaf) {
    MeasurableBtree<2> t;
    t.add(1);
    t.add(2);
    t.add(3);
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

    t.measure();
    ASSERT_EQ(1, Measurable::deepest);
    ASSERT_EQ(1, Measurable::shallowest);
}

TEST(Btree, addingNewElementMovesElementUpFromLeaf) {
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
    t.add(1);
    t.add(20);
    t.add(2);

    auto result = t.dump();

    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(20, result[2]);

    check_balance(t);
}

TEST(Btree, mixedFind) {
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

    check_balance(t);
}

TEST(Btree, mixedRecursiveGrow) {
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

    check_balance(t);
}

TEST(Btree, mixedBig) {
    test_mixed<2>(100);
}

TEST(Btree, fullGrow) {
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

    check_balance(t);
}

TEST(Btree, duplicateKeyRoot) {
    MeasurableBtree<2> t;
    t.add(0);
    ASSERT_THROW(t.add(0), invalid_key_exception);
}

TEST(Btree, duplicateKeyInLeaf) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(9);

    ASSERT_THROW(t.add(1), invalid_key_exception);
}

TEST(Btree, duplicateKeyInNode) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(9);

    ASSERT_THROW(t.add(5), invalid_key_exception);
}

TEST(Btree, duplicateKeyInRootRecursive) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(9);

    ASSERT_THROW(t.add(3), invalid_key_exception);
}

TEST(Btree, randomBig) {
    test_random<2>(100);
}

TEST(Btree, preorderWalk) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(9);

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

TEST(Btree, postorderWalk) {
    MeasurableBtree<2> t = tree_with_incremental_elements<2>(9);

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

#endif

#ifndef TEST_UTILS_H_
#define TEST_UTILS_H_

#include "utils.h"
#include "measurable/measurable.h"


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

#endif

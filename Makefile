# A sample Makefile for building Google Test and using it in user
# tests.  Please tweak it to suit your environment and project.  You
# may want to move it to your project's root directory.
#
# SYNOPSIS:
#
#   make [all]  - makes everything.
#   make TARGET - makes the given target.
#   make clean  - removes all files generated by make.

# Please tweak the following variable definitions as needed by your
# project, except GTEST_HEADERS, which you can use in your own targets
# but shouldn't modify.

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = googletest/

# Where to find user code.
USER_DIR = src/

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include

# Flags passed to the C++ compiler.
# set all compiler warnings on
# pthread is probably needed by the gtest framework (link pthread lib as well)
# use cpp11 standard
CXXFLAGS += -Isrc -g -Wall -Wextra -pthread -std=c++11

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = test

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h


# House-keeping build targets.

all : $(TESTS)

# Profile target specific compiler params, for coverage measurement
profile: CXXFLAGS += --coverage
profile: $(TESTS)
	mkdir coverage
	./test
	lcov --directory . --capture --no-external --output-file coverage/coverage.info
	genhtml --output-directory coverage coverage/coverage.info

check:
	make all
	valgrind --leak-check=yes ./test

clean :
	rm -rf $(TESTS) gtest.a gtest_main.a *.o coverage/ *.gcno *.gcda

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^


btree.o : $(USER_DIR)/btree.cpp $(USER_DIR)/btree.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/btree.cpp

keys.o : $(USER_DIR)/keys.cpp $(USER_DIR)/keys.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/keys.cpp

test_utils.o : $(USER_DIR)/test_utils.cpp $(USER_DIR)/test_utils.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/test_utils.cpp

test.o : $(USER_DIR)/test.cpp \
                     $(USER_DIR)/btree.h $(USER_DIR)/keys.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/test.cpp

test : keys.o btree.o test_utils.o test.o gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

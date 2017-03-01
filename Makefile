USER_DIR = src
LIB_DIR = lib
BIN_DIR = bin

GTEST_DIR = $(LIB_DIR)/gtest
GTEST_LIB = $(GTEST_DIR)/gtest_main.a
GTEST_HEADERS = $(GTEST_DIR)/include/

PRE_TARGET_STEP = mkdir -p bin;

# Flags passed to the C++ compiler.
# set all compiler warnings on
# use cpp11 standard
CXXFLAGS += -o $@ -I $(USER_DIR) -I $(GTEST_HEADERS) -L $(GTEST_LIB) -g -Wall -Wextra -pthread -std=c++11

TESTS = test

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
	rm -rf $(TESTS) $(BIN_DIR)/*.o coverage/ $(BIN_DIR)/*.gcno $(BIN_DIR)/*.gcda


$(BIN_DIR)/keys.o : \
	$(USER_DIR)/keys/keys.cpp \
	$(USER_DIR)/keys/keys.h
	$(PRE_TARGET_STEP) $(CXX) $(CXXFLAGS) -c $(USER_DIR)/keys/keys.cpp

$(BIN_DIR)/btree.o : \
	$(BIN_DIR)/keys.o \
	$(USER_DIR)/btree.cpp \
	$(USER_DIR)/btree.h
	$(PRE_TARGET_STEP) $(CXX) $(CXXFLAGS) -c $(USER_DIR)/btree.cpp

$(BIN_DIR)/measurable.o : \
	$(BIN_DIR)/btree.o \
	$(USER_DIR)/measurable/measurable.cpp \
	$(USER_DIR)/measurable/measurable.h
	$(PRE_TARGET_STEP) $(CXX) $(CXXFLAGS) -c $(USER_DIR)/measurable/measurable.cpp

$(BIN_DIR)/keys_test_utils.o : \
	$(BIN_DIR)/keys.o \
	$(USER_DIR)/keys/keys_test_utils.cpp \
	$(USER_DIR)/keys/keys_test_utils.h
	$(PRE_TARGET_STEP) $(CXX) $(CXXFLAGS) -c $(USER_DIR)/keys/keys_test_utils.cpp

$(BIN_DIR)/keys_test.o : \
	$(BIN_DIR)/keys_test_utils.o \
	$(BIN_DIR)/keys.o \
	$(USER_DIR)/keys/keys.h \
	$(USER_DIR)/keys/test_keys.cpp \
	$(USER_DIR)/keys/test_keys.h
	$(PRE_TARGET_STEP) $(CXX) $(CXXFLAGS) -c $(USER_DIR)/keys/test_keys.cpp

$(BIN_DIR)/measurable_test_utils.o : \
	$(BIN_DIR)/measurable.o \
	$(USER_DIR)/measurable/measurable_test_utils.cpp \
	$(USER_DIR)/measurable/measurable_test_utils.h
	$(PRE_TARGET_STEP) $(CXX) $(CXXFLAGS) -c $(USER_DIR)/measurable/measurable_test_utils.cpp

$(BIN_DIR)/measurable_test.o : \
	$(BIN_DIR)/measurable_test_utils.o \
	$(BIN_DIR)/measurable.o \
	$(USER_DIR)/measurable/test_measurable.cpp \
	$(USER_DIR)/measurable/test_measurable.h
	$(PRE_TARGET_STEP) $(CXX) $(CXXFLAGS) -c $(USER_DIR)/measurable/test_measurable.cpp

$(BIN_DIR)/all_test.o : \
	$(BIN_DIR)/keys_test.o \
	$(BIN_DIR)/measurable_test.o \
	$(USER_DIR)/test.cpp \
	$(GTEST_HEADERS)
	$(PRE_TARGET_STEP) $(CXX) $(CXXFLAGS) -c $(USER_DIR)/test.cpp

test : \
	$(BIN_DIR)/keys.o \
	$(BIN_DIR)/btree.o \
	$(BIN_DIR)/measurable.o \
	$(BIN_DIR)/keys_test_utils.o \
	$(BIN_DIR)/keys_test.o \
	$(BIN_DIR)/measurable_test_utils.o \
	$(BIN_DIR)/measurable_test.o \
	$(BIN_DIR)/all_test.o \
	$(GTEST_LIB)
	$(CXX) $(CXXFLAGS) -lpthread $^

SRC_DIR = src
LIB_DIR = lib
BIN_DIR = bin

GTEST_DIR = $(LIB_DIR)/gtest
GTEST_LIB = $(GTEST_DIR)/gtest_main.a
GTEST_HEADERS = $(GTEST_DIR)/include/

PRE_TARGET_STEP = mkdir -p bin;

# Flags passed to the C++ compiler.
# set all compiler warnings on
# use cpp11 standard
CXXFLAGS += -o $@ -I $(SRC_DIR) -I $(GTEST_HEADERS) -L $(GTEST_LIB) -g -Wall -Wextra -pthread -std=c++11

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
	rm -rf $(TESTS) $(BIN_DIR)/ coverage/

COMPILE = $(PRE_TARGET_STEP) $(CXX) $(CXXFLAGS) -c $<

# compile files under keys
$(BIN_DIR)/%.o: $(SRC_DIR)/keys/%.cpp $(SRC_DIR)/keys/%.h
	$(COMPILE)

# compile files under measurable
$(BIN_DIR)/%.o: $(SRC_DIR)/measurable/%.cpp $(SRC_DIR)/measurable/%.h
	$(COMPILE)

# compile files under btree
$(BIN_DIR)/%.o : $(SRC_DIR)/btree/%.cpp $(SRC_DIR)/btree/%.h
	$(COMPILE)

# compile main_test.cpp
$(BIN_DIR)/main_test.o : $(SRC_DIR)/main_test.cpp
	$(COMPILE)

# define the required object files
_OBJ = keys.o \
       btree.o \
       measurable.o \
       keys_test_utils.o \
       test_keys.o \
       measurable_test_utils.o \
       test_measurable.o \
       main_test.o

# add bin dir as prefix to the required object files
OBJ = $(patsubst %,$(BIN_DIR)/%,$(_OBJ))

# link the object files and create test executable
test : \
	$(OBJ) \
	$(GTEST_LIB)
	$(CXX) $(CXXFLAGS) -lpthread $^

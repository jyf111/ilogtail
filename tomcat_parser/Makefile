CXX = g++
INCLUDE = -I$(shell pwd)

SRCS = $(wildcard *.cpp)
DEPS = $(wildcard *.h)
OBJS = $(SRCS:.cpp=.o)

TEST_SRCS = $(wildcard test/*.cpp)
TEST_DEPS = $(wildcard test/*.h)
TEST_TARGETS = test/basic test/stress
TEST_OBJS = $(filter-out $(TEST_TARGETS:=.o), $(TEST_SRCS:.cpp=.o))

.PHONY: all debug test format clean

DEFAULT_FLAGS = -std=c++11 -Wall -Wextra
CXXFLAGS = $(DEFAULT_FLAGS) -O3 -DNDEBUG

all: $(OBJS)

debug: CXXFLAGS = $(DEFAULT_FLAGS) -O0 -DDEBUG
debug: test

test: $(OBJS) $(TEST_OBJS) $(TEST_TARGETS)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(TEST_TARGETS): test/%: test/%.o $(OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@

format:
	clang-format -i $(SRCS) $(TEST_SRCS) $(DEPS) $(TEST_DEPS)

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TEST_TARGETS) $(TEST_TARGETS:=.o)
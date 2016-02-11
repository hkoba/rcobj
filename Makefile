# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -I.
# For lambda.
CXXFLAGS += -std=c++11
# -Wextra
LDLIBS += -lpthread -lgtest
CC = g++

.PHONY: all
all: test

include gtest_depend.make

test : $(TESTS)
	for t in $(TESTS); do ./$$t || break; done

clean :
	rm -f $(TESTS) $(OBJS)

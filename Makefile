# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -I.
# -Wextra
LDLIBS += -lpthread -lgtest
CC = g++

.PHONY: test

include gtest_depend.make

test : $(TESTS)
	for t in $(TESTS); do ./$$t || break; done

clean :
	rm -f $(TESTS) $(OBJS)

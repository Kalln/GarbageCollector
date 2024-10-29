all: main unittests inlupp2 demo_tests
.PHONY: clean test coverage demo_tests
.SUFFIXES:

CC       = gcc
CFLAGS   = -Wall -pedantic -g -Werror
INCLUDES = -I /opt/homebrew/Cellar/cunit/2.1-3/include
LDLIBS   = -lm -lcunit
LDFLAGS  = -L /opt/homebrew/lib/

ifdef SANITIZE
CFLAGS += -fsanitize=$(SANITIZE)
LDFLAGS += -fsanitize=$(SANITIZE)
endif

ifdef COVERAGE
CFLAGS += -coverage
LDFLAGS += -coverage
endif

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
# On macOS, use `leaks` instead of valgrind
LEAK_SAN = leaks --atExit --
else
# Otherwise, use valgrind
LEAK_SAN = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
endif

DEMO_LIB_OBJECTS = demo/equality_functions.o demo/hash_table.o demo/iterator.o demo/linked_list.o demo/store_logic.o demo/utils.o

%.o:  %.c Makefile
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%_nostatic.o:  %.c Makefile
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ -D REFMEM_DISABLE_STATIC

src/refmem.o test/test_refmem.o: src/refmem.h src/refmem_internal.h

src/linked_list.o: src/linked_list.h

test/test_refmem.o: src/refmem_testing.h

main: src/refmem.o src/linked_list.o

unittests: src/refmem_nostatic.o test/test_refmem.o src/linked_list.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

example: src/refmem.o demo/example.o src/linked_list.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

inlupp2: src/refmem.o src/linked_list.o $(DEMO_LIB_OBJECTS) demo/ui.o demo/main.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# `backend`, `hash_table_unit`, `linked_list_unit`, `utils_unit` - _tests
%_tests: src/refmem.o src/linked_list.o $(DEMO_LIB_OBJECTS) test/%_tests.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) -D REFMEM_DISABLE_STATIC

demo_tests: hash_table_unit_tests linked_list_unit_tests utils_unit_tests backend_tests

test_refmem: unittests
	$(LEAK_SAN) ./unittests

test: unittests demo_tests
	./unittests
	./hash_table_unit_tests
	./linked_list_unit_tests
	./utils_unit_tests
	# ./backend_tests

memtest: unittests demo_tests
	$(LEAK_SAN) ./unittests
	$(LEAK_SAN) ./hash_table_unit_tests
	$(LEAK_SAN) ./linked_list_unit_tests
	$(LEAK_SAN) ./utils_unit_tests
	$(LEAK_SAN) ./backend_tests

memtest_link: unittests demo_tests
	$(LEAK_SAN) ./linked_list_unit_tests

memtest_hs: unittests demo_tests
	$(LEAK_SAN) ./hash_table_unit_tests

memtest_utils: unittests demo_tests
	$(LEAK_SAN) ./utils_unit_tests

memtest_back: unittests demo_tests
	$(LEAK_SAN) ./backend_tests

clean:
	find . \( -type f -name "*.o" -o -name "*.gcno" -o -name "*.gcda" -o -name "*.info" \) -delete
	rm -f unittests inlupp2 hash_table_unit_tests linked_list_unit_tests utils_unit_tests backend_tests

coverage: clean
	$(MAKE) test COVERAGE=true
	lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1
	genhtml coverage.info --output-directory coverage --branch-coverage --legend

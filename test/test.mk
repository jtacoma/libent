ALL_TARGETS   += test/uncovered.txt ent-test
CLEAN_TARGETS += clean-test

test_SOURCES := $(wildcard test/*.c)
test_HEADERS := $(wildcard test/*.h)
test_CFLAGS   = -Ilibent -Itest
test_LIBS     = 

test_OBJECTS := $(test_SOURCES:.c=.o)

test/%.o: test/%.c $(test_HEADERS)
	$(CC) $(ENT_CPPFLAGS) $(ENT_CFLAGS) $(test_CFLAGS) -c -o $@ $<

test/ent_all_tests.h: test/*_test.c test/gen
	./test/gen test/*_test.c > $@

test/main.o: test/ent_all_tests.h

ent-testcov: libent.so $(test_OBJECTS)
	$(CC) -fprofile-arcs -ftest-coverage -o $@ $(test_OBJECTS) $(ENT_LIBS) $(test_LIBS) -O0

ent-test: libent.so $(test_OBJECTS)
	$(CC) -o $@ $(test_OBJECTS) $(ENT_LIBS) $(test_LIBS)

test/uncovered.txt: ent-testcov
	rm -f *.gcda
	export LD_LIBRARY_PATH=. ; ./ent-testcov
	llvm-cov gcov *.gcda > /dev/null
	ls *.gcov \
		| grep -v '_test\|main' \
		| xargs grep '#####' \
		| grep -v 'out of memory (for real)' \
		| grep -v ':	*}$$' \
		| cut -d: -f1,3,4- \
		| sed -e 's/: */:/' -e 's/\.gcov//' \
		| grep -v 'EOF' \
		| tee $@ || true

clean-test:
	rm -f $(test_OBJECTS) ent-testcov ent-test *.gcno


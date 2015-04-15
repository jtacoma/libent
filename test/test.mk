ALL_TARGETS   += bin/ent-test
TEST_TARGETS  += test/uncovered.txt
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

bin/ent-testcov: lib/libent.so $(test_OBJECTS)
	@[ -d bin ] || mkdir bin
	$(CC) -o $@ $(test_OBJECTS) $(ENT_LIBS) $(test_LIBS) -O0

bin/ent-test: lib/libent.so $(test_OBJECTS)
	@[ -d bin ] || mkdir bin
	$(CC) -o $@ $(test_OBJECTS) $(ENT_LIBS) $(test_LIBS)

test/uncovered.txt: bin/ent-testcov
	rm -f *.gcda
	export LD_LIBRARY_PATH=lib ; ./bin/ent-testcov
	llvm-cov gcov libent/*.gcda > /dev/null
	ls *.gcov \
		| grep -v '_test\|main' \
		| xargs grep '#####' \
		| grep -v 'out of memory (for real)' \
		| grep -v ':	*}$$' \
		| cut -d: -f1,3,4- \
		| sed -e 's/: */:/' -e 's/\.gcov//' \
		| grep -v 'EOF' \
		| tee $@ || true

.PHONY: clean-test
clean-test:
	rm -f $(test_OBJECTS) bin/ent-testcov bin/ent-test test/*.gcda test/*.gcno *.gcov

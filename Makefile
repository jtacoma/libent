CC=clang
CFLAGS+=-std=c11 -g -Wall -Wextra -Werror -I. -Iinclude

.PHONY: all
all: .styled tags test/uncovered.txt run-benchmarks bin/ent-demo

.PHONY: run-benchmarks
run-benchmarks: bin/ent-bench
	./bin/ent-bench

.styled: *.[ch] include/*.h test/*.[ch] bench/*.[ch] demo/*.[ch]
	astyle $?
	touch $@

tags: *.c test/*.c bench/*.c demo/*.c
	ctags $^

test/ent_all_tests.h: test/*_test.c test/gen
	./test/gen test/*_test.c > $@

bin/ent-testcov: *.[ch] include/*.h test/*.[ch] test/ent_all_tests.h
	@mkdir -p bin
	$(CC) -fprofile-arcs -ftest-coverage -o $@ *.c test/*.c $(CFLAGS) -O0

test/uncovered.txt: bin/ent-testcov
	rm -f *.gcda
	./bin/ent-testcov
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

lib/libent.so: *.[ch] include/*.h
	@mkdir -p lib
	$(CC) -o $@ *.c $(CFLAGS) -O2 -shared -fPIC

bin/ent-test: lib/libent.so test/*.[ch]
	@mkdir -p bin
	$(CC) *.c test/*.c -o $@ -L./lib -lent $(CFLAGS)

bin/ent-bench: bench/*.c lib/libent.so
	@mkdir -p bin
	$(CC) -o $@ bench/*.c -I. -L./lib -lent -lrt $(CFLAGS) -O2 -std=gnu11

bin/ent-demo: lib/libent.so demo/*.c
	@mkdir -p bin
	$(CC) -o $@ demo/*.c -I. -L./lib -lent $(CFLAGS)

clean:
	rm -f .styled tags *.gcno *.gcda *.gcov
	rm -rf lib bin

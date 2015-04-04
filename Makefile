#CFLAGS=-O3
#CFLAGS=-g -O2
CFLAGS=-g -fprofile-arcs -ftest-coverage -O0
CFLAGS+=-std=c11 -Wall -Wextra -Werror -Iinclude

.PHONY: all
all: .styled tags test bench bin/ent-demo

.PHONY: test
test: bin/ent-test
	./bin/ent-test
	gcov *.gcda > /dev/null
	ls *.gcov \
		| grep -v '_test\|main' \
		| xargs grep '#####' \
		| cut -d: -f1,3,4- \
		| sed -e 's/: */:/' -e 's/\.gcov//' \
		| grep -v 'EOF' \
		> test/cov_miss_actual.txt
	diff test/cov_miss_expected.txt test/cov_miss_actual.txt


.PHONY: bench
bench: bin/ent-bench
	bin/ent-bench

.styled: *.[ch] include/*.h test/*.[ch] bench/*.[ch] demo/*.[ch]
	astyle $?
	touch $@

tags: *.c test/*.c bench/*.c demo/*.c
	ctags $^

lib/libent.so: *.[ch]
	mkdir -p lib
	gcc $(CFLAGS) *.c -o $@ -shared -fPIC

bin/ent-test: lib/libent.so test/*.[ch]
	mkdir -p bin
	gcc test/*.c -o $@ -I. -L./lib -lent $(CFLAGS)

# Compile entire library along with bench mark code to a single binary because
# GCC produces more consistent optimizations across alternative algorithms when
# it has access to all relevant code at once.  When libent is used for real it's
# going to be compiled with -O3 so we run benchmarks that way too.  Finally, we
# specify -std=gnu11 to make some time.h functions available (this could
# probably be refactored so that -std=c11 will do).
bin/ent-bench: *.[ch] bench/*.c
	mkdir -p bin
	gcc -pg *.c bench/*.c -o $@ -I. -lrt $(CFLAGS) -O3 -std=gnu11

bin/ent-demo: lib/libent.so demo/*.c
	mkdir -p bin
	gcc demo/*.c -o $@ -I. -L./lib -lent $(CFLAGS)

clean:
	rm -f .styled tags lib/libent.so bin/ent-test bin/ent-bench bin/ent-demo *.gcno *.gcda *.gcov

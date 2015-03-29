#CFLAGS=-O3
#CFLAGS=-g -O2
CFLAGS=-g -fprofile-arcs -ftest-coverage -O0
CFLAGS+=-std=c11 -Wall -Wextra -Werror -Iinclude

.PHONY: all
all: .styled tags bin/ent-test bin/ent-demo
	./bin/ent-test
	valgrind -v --leak-check=full ./bin/ent-test
	gcov *.gcda > /dev/null
	grep '#####' *.gcov | cut -d: -f1,3,4- | sed -e 's/: */:/' -e 's/\.gcov//' | grep -v 'EOF' > test/cov_miss_actual.txt
	diff test/cov_miss_expected.txt test/cov_miss_actual.txt

.styled: *.[ch] include/*.h test/*.[ch]
	astyle $?
	touch $@

tags: *.c
	ctags *.c

lib/libent.so: *.[ch]
	mkdir -p lib
	gcc *.c -o $@ -shared -fPIC $(CFLAGS)

bin/ent-test: lib/libent.so test/*.[ch]
	mkdir -p bin
	gcc test/*.c -o $@ -I. -L. -lent $(CFLAGS)

bin/ent-demo: lib/libent.so demo/*.c
	mkdir -p bin
	gcc demo/*.c -o $@ -I. -L. -lent $(CFLAGS)

clean:
	rm -f .styled tags lib/libent.so bin/ent-test *.gcno *.gcda *.gcov

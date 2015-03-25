#CFLAGS=-O3
#CFLAGS=-g -O2
CFLAGS=-g -fprofile-arcs -ftest-coverage -O0
CFLAGS+=-std=c11 -Wall -Wextra -Werror

.PHONY: all
all: .styled tags testent
	./testent
	valgrind -v --leak-check=full ./testent
	gcov *.gcda > /dev/null
	grep '#####' *.gcov | cut -d: -f1,3,4- | sed -e 's/: */:/' -e 's/\.gcov//' | grep -v 'out of memory'

.styled: *.[ch] test/*.[ch]
	astyle $?
	touch $@

tags: *.c
	ctags *.c

lib/libent.so: *.[ch]
	mkdir -p lib
	gcc *.c -o $@ -shared -fPIC $(CFLAGS)

testent: lib/libent.so test/*.[ch]
	gcc test/*.c -o $@ -I. -L. -lent $(CFLAGS)

clean:
	rm -f .styled tags lib/libent.so testent *.gcno *.gcda *.gcov

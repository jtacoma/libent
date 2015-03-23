#CFLAGS=-O3
#CFLAGS=-g -O2
CFLAGS=-g -fprofile-arcs -ftest-coverage -O0
CFLAGS+=-std=c11 -Wall -Wextra -Werror

.PHONY: all
all: .styled tags debug
	valgrind -v --leak-check=full ./testent
	gcov *.gcda > /dev/null
	grep '#####' *.gcov | cut -d: -f1,3,4- | sed -e 's/: */:/' -e 's/\.gcov//' | grep -v 'extraordinary'

.PHONY: debug
debug: testent
	./testent || ( gdb ./testent && false )

.codegen: *.[ch]
	python gen.py *.[ch]
	touch $@

.styled: .codegen *.[ch] test/*.[ch]
	astyle $?
	touch $@

tags: *.c
	ctags *.c

libent.so: *.[ch]
	gcc *.c -o $@ -shared -fPIC $(CFLAGS)

testent: libent.so test/*.[ch]
	gcc test/*.c -o $@ -I. -L. -lent $(CFLAGS)

clean:
	rm -f .styled tags libent.so testent

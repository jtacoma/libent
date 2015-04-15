ALL_TARGETS   += bin/ent-hello
CLEAN_TARGETS += clean-hello

hello_SOURCES := $(wildcard hello/*.c)
hello_HEADERS := $(wildcard hello/*.h)

hello_OBJECTS := $(hello_SOURCES:.c=.o)

hello/%.o: hello/%.c $(hello_HEADERS)
	$(CC) $(ENT_CPPFLAGS) $(ENT_CFLAGS) -c -o $@ $<

bin/ent-hello: $(hello_OBJECTS) lib/libent.so
	@[ -d bin ] || mkdir bin
	$(CC) -o $@ $(hello_OBJECTS) $(ENT_LIBS)

.PHONY: clean-hello
clean-hello:
	rm -f $(hello_OBJECTS) bin/ent-hello

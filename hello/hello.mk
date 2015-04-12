ALL_TARGETS   += ent-hello
CLEAN_TARGETS += clean-hello

hello_SOURCES := $(wildcard hello/*.c)
hello_HEADERS := $(wildcard hello/*.h)

hello_OBJECTS := $(hello_SOURCES:.c=.o)

hello/%.o: hello/%.c $(hello_HEADERS)
	$(CC) $(ENT_CPPFLAGS) $(ENT_CFLAGS) -c -o $@ $<

ent-hello: $(hello_OBJECTS) libent.so
	$(CC) -o $@ $(hello_OBJECTS) $(ENT_LIBS)

clean-hello:
	rm -f $(hello_OBJECTS) ent-hello


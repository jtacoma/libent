ALL_TARGETS   += bin/ent-xent
CLEAN_TARGETS += clean-xent

xent_SOURCES := $(wildcard xent/*.c)
xent_HEADERS := $(wildcard xent/*.h)
xent_CFLAGS   = -Ixent $(shell pkg-config --cflags gl glu x11 xi)
xent_LIBS     = $(shell pkg-config --libs gl glu x11 xi)

xent_OBJECTS := $(xent_SOURCES:.c=.o)

xent/%.o: xent/%.c $(xent_HEADERS)
	$(CC) $(ENT_CPPFLAGS) $(ENT_CFLAGS) $(xent_CFLAGS) -c -o $@ $<

bin/ent-xent: $(xent_OBJECTS) lib/libent.so
	@[ -d bin ] || mkdir bin
	$(CC) $(ENT_CFLAGS) $(ENT_CFLAGS) -o $@ $(xent_OBJECTS) $(ENT_LIBS) $(xent_LIBS)

.PHONY: clean-xent
clean-xent:
	rm -f $(xent_OBJECTS) bin/ent-xent

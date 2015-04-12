ALL_TARGETS += ent-xent
CLEAN_TARGETS += clean-xent

xent_SOURCES := $(wildcard xent/*.c)
xent_HEADERS := $(wildcard xent/*.h)
xent_CFLAGS   = -Ixent -Ilibent $(shell pkg-config --cflags gl glu x11 xi)
xent_LIBS     = -L. -lent $(shell pkg-config --libs gl glu x11 xi)

xent_OBJECTS := $(xent_SOURCES:.c=.o)

xent/%.o: xent/%.c $(xent_HEADERS)
	$(CC) $(ENT_CPPFLAGS) $(ENT_CFLAGS) $(xent_CFLAGS) -c -o $@ $<

ent-xent: $(xent_OBJECTS) libent.so
	$(CC) $(ENT_CFLAGS) $(ENT_CFLAGS) -o $@ $(xent_OBJECTS) $(xent_LIBS)

clean-xent:
	rm -f $(xent_OBJECTS) ent-xent


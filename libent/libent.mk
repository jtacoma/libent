ALL_TARGETS   +=
CLEAN_TARGETS += clean-libent

libent_SOURCES := $(wildcard libent/*.c)
libent_HEADERS := $(wildcard libent/*.h)
libent_CFLAGS   = -fPIC
libent_LIBS     =

libent_OBJECTS := $(libent_SOURCES:.c=.o)

libent/%.o: libent/%.c $(libent_HEADERS)
	$(CC) $(ENT_CPPFLAGS) $(ENT_CFLAGS) $(libent_CFLAGS) -c -o $@ $<

libent.a: $(libent_OBJECTS)
	$(AR) rcs $@ $^ $(libent_LIBS)

libent.so: $(libent_OBJECTS)
	$(CC) -o $@ $(libent_OBJECTS) -shared

clean-libent:
	rm -f $(libent_OBJECTS) libent.a libent.so


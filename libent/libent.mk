ALL_TARGETS   += lib/libent.so lib/libent.a
CLEAN_TARGETS += clean-libent

libent_SOURCES := $(wildcard libent/*.c)
libent_HEADERS := $(wildcard libent/*.h)
libent_CFLAGS   = -fPIC
libent_LIBS     =

libent_OBJECTS := $(libent_SOURCES:.c=.o)

libent/%.o: libent/%.c $(libent_HEADERS)
	$(CC) $(ENT_CPPFLAGS) $(ENT_CFLAGS) $(COVERAGE_CFLAGS) $(libent_CFLAGS) -c -o $@ $<

lib/libent.a: $(libent_OBJECTS)
	@[ -d lib ] || mkdir lib
	$(AR) rcs $@ $^ $(libent_LIBS)

lib/libent.so: $(libent_OBJECTS)
	@[ -d lib ] || mkdir lib
	$(CC) -o $@ -shared $(libent_OBJECTS)

.PHONY: clean-libent
clean-libent:
	rm -f $(libent_OBJECTS) lib/libent.a lib/libent.so libent/*.gcda libent/*.gcno

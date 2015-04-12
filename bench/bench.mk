ALL_TARGETS   += ent-bench
CLEAN_TARGETS += clean-bench

bench_SOURCES := $(wildcard bench/*.c)
bench_HEADERS := $(wildcard bench/*.h)
bench_CFLAGS  += -Ibench -Ilibent
bench_LIBS    += -lrt

# TODO: remove dependency on gnu11 by using c11 time functions
bench_CFLAGS  += -std=gnu11

bench_OBJECTS := $(bench_SOURCES:.c=.o)

bench/%.o: bench/%.c $(bench_HEADERS)
	$(CC) $(ENT_CPPFLAGS) $(ENT_CFLAGS) $(bench_CFLAGS) -c -o $@ $<

ent-bench: $(bench_OBJECTS) libent.so
	$(CC) -o $@ $(bench_OBJECTS) $(ENT_LIBS) $(bench_LIBS)

run-benchmarks: ent-bench libent.so
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:. ; ./ent-bench

clean-bench:
	rm -f $(bench_OBJECTS) ent-bench


ALL_TARGETS   += bin/ent-bench
TEST_TARGETS  += run-benchmarks
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

bin/ent-bench: $(bench_OBJECTS) lib/libent.so
	@[ -d bin ] || mkdir bin
	$(CC) -o $@ $(bench_OBJECTS) $(ENT_LIBS) $(bench_LIBS)

.PHONY: run-benchmarks
run-benchmarks: bin/ent-bench
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:lib ; ./bin/ent-bench

.PHONY: clean-bench
clean-bench:
	rm -f $(bench_OBJECTS) bin/ent-bench

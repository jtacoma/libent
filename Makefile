include common.mk

ALL_TARGETS = .styled
CLEAN_TARGETS =
STATIC_SOURCES =

all: real-all

include libent/libent.mk
include test/test.mk
include bench/bench.mk
include hello/hello.mk
include xent/xent.mk

real-all: $(ALL_TARGETS)

clean: $(CLEAN_TARGETS)

.PHONY: all
all: test/uncovered.txt run-benchmarks ent-hello ent-xent

.styled: */*.[ch]
	astyle $?
	touch $@

tags: $(STATIC_SOURCES)
	ctags $^

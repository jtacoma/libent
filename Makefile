include common.mk

ALL_TARGETS =
TEST_TARGETS =
CLEAN_TARGETS =

all: real-all

include libent/libent.mk
include test/test.mk
include bench/bench.mk
include hello/hello.mk
include xent/xent.mk

real-all: .styled tags $(ALL_TARGETS) $(TEST_TARGETS)

clean: $(CLEAN_TARGETS)
	rm -f .styled tags

.styled: */*.[ch]
	astyle $?
	touch $@

tags: */*.[ch]
	ctags $^

LDLIBS += -lcheck
CFLAGS += -g -std=c99 -I../src

CLEANFILES += *.[od] check

.PHONY: all run clean
all: run

clean:
	-rm -rf $(CLEANFILES)

clobber: clean
	-rm -rf $(CLOBBERFILES)

run: check
	$(shell dirname $<)/$<

vpath %.c ../src
check: ops.o ops_impl.o sim.o hc08.o


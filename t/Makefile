LDLIBS   += -lcheck
CPPFLAGS += -I../src
CFLAGS   += -g -std=c99
CFILES   += $(wildcard *.c)

CLEANFILES += *.[od] check $(shell cat .gitignore)

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

%.rel: %.asm
	sdas6808 -ols $@ $<

%.lnk: %.rel
	sed s/XXX/$*/g _tmpl.lnk > $@.$$$$ && \
	mv $@.$$$$ $@

%.s19: %.lnk
	sdld6808 -nf $<

%.asm: %.s
	cat _tmpl.asm $< > $@.$$$$ && \
	mv $@.$$$$ $@

%.s19.genH: %.s19
	../tools/s19_to_c.pl $^ > $@.$$$$ && \
	mv $@.$$$$ $@

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

INHIBIT_DEPS = $(words $(filter %clean% clean% clean clobber,$(MAKECMDGOALS)))

ifeq ($(INHIBIT_DEPS),0)
-include $(notdir $(addsuffix .d,$(basename $(CFILES))))
endif


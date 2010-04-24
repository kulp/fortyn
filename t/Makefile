LDLIBS   += -lcheck
CPPFLAGS += -I../src
CFLAGS   += -g -std=c99 -Wall -Wextra -Werror -pedantic-errors -Wno-unused-parameter
CFILES   += $(wildcard *.c)

HC08ASM = sdas6808
HC08LNK = sdld6808

CLEANFILES   += *.[od] check $(shell cat .gitignore)
CLOBBERFILES += *.s19.genH

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

##############################################################################
# Create includable files from .s assembly sources

%.rel: %.asm
	$(HC08ASM) -ols $@ $<

%.lnk: _tmpl.tlnk %.rel
	sed s/XXX/$*/g $< > $@.$$$$ && \
	mv $@.$$$$ $@

%.s19: %.lnk
	$(HC08LNK) -nf $<

%.asm: _tmpl.tasm %.s
	cat $^ > $@.$$$$ && \
	mv $@.$$$$ $@

%.s19.genH: %.s19
	../tools/s19_to_c.pl $^ > $@.$$$$ && \
	mv $@.$$$$ $@

##############################################################################
# Dependency generation
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M -MG $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

INHIBIT_DEPS = $(words $(filter %clean% clean% clean clobber,$(MAKECMDGOALS)))

ifeq ($(INHIBIT_DEPS),0)
-include $(notdir $(addsuffix .d,$(basename $(CFILES))))
endif


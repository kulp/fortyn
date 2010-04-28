LDLIBS   += -lcheck
CPPFLAGS += -I../src
CFLAGS   += -g -std=c99 -Wall -Wextra -Werror -pedantic-errors -Wno-unused-parameter
CFILES   += $(wildcard *.c)

DEFINES += _GNU_SOURCE

override CFLAGS += $(patsubst %,-D%,$(DEFINES))
override CFLAGS += $(patsubst %,-I%,$(INCLUDE))

HC08ASM = sdas6808
HC08LNK = sdld6808

CLEANFILES   += *.[od] check $(shell cat .gitignore)
CLOBBERFILES += *.s19.genH suites.h

.PHONY: all regen run clean
all: run

# if suites.h is removed, "make regen" must be run before "make [all]"
regen: suites.h ;

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
# Test generation

suites.h: suites.xi
	sed 's/SUITE_(\(.*\))/#include "\1.t"/' $^ > $@.$$$$ && \
	mv $@.$$$$ $@

##############################################################################
# Dependency generation
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MMD -MG $(CPPFLAGS) $< -MF $@.$$$$ -E -o /dev/null; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

INHIBIT_DEPS = $(words $(filter regen %clean% clean% clean clobber,$(MAKECMDGOALS)))

ifeq ($(INHIBIT_DEPS),0)
-include $(notdir $(addsuffix .d,$(basename $(CFILES))))
endif


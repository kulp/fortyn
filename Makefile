HC_MODEL ?= MC9S08GB60A
CFILES = $(wildcard src/*.c)
INCLUDE += . src

UIS = $(patsubst %.c,%,$(wildcard ui/*/main.c))
CLEANFILES += $(UIS)

vpath %.h src
vpath %.c src

DEFINES += _BSD_SOURCE
LDLIBS += -lbsd-compat

override CFLAGS += -g -std=c99 -W -Wall -Wextra -pedantic-errors -Werror
override CFLAGS += $(patsubst %,-D%,$(DEFINES))
override CFLAGS += $(patsubst %,-I%,$(INCLUDE))

all: $(UIS)

SUBDIRS = t
.PHONY: all check subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

check:
	$(MAKE) -C t run

ui/curses/main: LDLIBS += -lncurses

# Ordering is important here: there may be multiple weak definitions of any
# particular op handler, and in such a case the first definition is taken (which
# should not be the default handler from ops.o).
$(UIS): sim.o hc08.o ops_impl.o ops.o 

ops_impl.o: CFLAGS += -Wno-unused-parameter

CLOBBERFILES += src/ops.[ch]

src/ops.h: src/ops.c ; $(NOOP)
src/ops.c: $(HC_MODEL).txt ./tools/make_opcode_table.pl
	perl -ne 'print if /Opcode Map/.../Opcode Map/' $< | \
		./tools/make_opcode_table.pl -d $(dir $@) -b $(notdir $(basename $@)) \
			-h $(notdir $(basename $@)).h -c $(notdir $(basename $@)).c

.SECONDARY: $(HC_MODEL).txt
CLOBBERFILES += $(HC_MODEL).txt
$(HC_MODEL).txt: $(HC_MODEL).pdf
	pdftotext -raw $< $@

#CLOBBERFILES += $(HC_MODEL).pdf
$(HC_MODEL).pdf:
	wget http://www.freescale.com/files/microcontrollers/doc/data_sheet/$(notdir $@)

CLEANFILES += *.o src/*.d
.PHONY: clean clobber todo
clean: subdirs
	-rm -rf $(CLEANFILES)

clobber: clean subdirs
	-rm -rf $(CLOBBERFILES)

%.bin: %.s19
	srec_cat -Output $@ -Binary $< -Motorola

ifneq ($(MAKECMDGOALS),clean)
-include $(CFILES:.c=.d)
endif

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) $(CFLAGS) -MM -MG -MF $@.$$$$ $<; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


HC_MODEL ?= MC9S08GB60A
CFILES = $(wildcard *.c)
DEFINES += _LITTLE_ENDIAN

override CFLAGS += -g -std=c99 -W -Wall -pedantic-errors #-Werror
override CFLAGS += $(patsubst %,-D%,$(DEFINES))
all: sim

# Ordering is important here: there may be multiple weak definitions of any
# particular op handler, and in such a case the first definition is taken (which
# should not be the default handler from ops.o).
sim: hc08.o ops_impl.o ops.o 

ops_impl.o: CFLAGS += -Wno-unused-parameter

CLEANFILES += sim

CLOBBERFILES += ops.h ops.c

ops.c ops.h: $(HC_MODEL).txt ./make_opcode_table.pl
	perl -ne 'print if /Opcode Map/.../Opcode Map/' $< | \
		./make_opcode_table.pl -b $(basename $@) -h $(basename $@).h -c $(basename $@).c

.SECONDARY: $(HC_MODEL).txt
CLOBBERFILES += $(HC_MODEL).txt
$(HC_MODEL).txt: $(HC_MODEL).pdf
	pdftotext -raw $< $@

CLOBBERFILES += $(HC_MODEL).pdf
$(HC_MODEL).pdf:
	wget http://www.freescale.com/files/microcontrollers/doc/data_sheet/$(notdir $@)

CLEANFILES += *.[od]
.PHONY: clean clobber todo
clean:
	-rm -rf $(CLEANFILES)

clobber: clean
	-rm -rf $(CLOBBERFILES)

todo:
	@-UNHANDLED=$$(nm sim | grep UNHANDLED | cut -d' ' -f1); \
	nm sim | grep handle_op | grep $$UNHANDLED | cut -d_ -f3 |grep -v UNHANDLED

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


#include "hc08.h"
#include "ops.h"

#include <string.h>

int hc_state_init(hc_state_t *st)
{
    int rc = 0;

    // Clear registers
    memset(&st->regs, 0, sizeof st->regs);

    // Set always-on bits
    st->regs.CCR.whole = 0x60;

    return rc;
}

int hc_do_reset(hc_state_t *st)
{
    int rc = 0;

    // Set PC to the reset vector located at $FFFE:$FFFF
    st->regs.PC.bytes.PCH = st->mem[0xFFFE];
    st->regs.PC.bytes.PCL = st->mem[0xFFFF];

    return rc;
}

int hc_op_page(hc_state_t *st)
{
    int rc = 0; // default page is zero, the no-prefix page

    uint16_t pc = st->regs.PC.whole;
    bool found = true;
    for (int i = 0; !found && i < pages_size; i++) {
        for (int j = 0; j < pages[i].prebyte_cnt; j++) {
            if (pages[i].prebyte_val[j] != st->mem[pc + j]) {
                found = false;
                break;
            }
        }

        if (found)
            rc = i;
    }

    return rc;
}

int hc_do_op(hc_state_t *st)
{
    int rc = 0;

    int page = hc_op_page(st);
    st->offset += pages[page].prebyte_cnt;

    uint16_t pc = st->regs.PC.whole;

    const struct opinfo *info = &opinfos[page][st->mem[pc + st->offset]];
    enum op op = info->type;

    return rc;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


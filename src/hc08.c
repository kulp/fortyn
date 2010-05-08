#include "hc08.h"
#include "ops.h"

#include <assert.h>
#include <string.h>

int hc_state_init(struct hc_state *st)
{
    int rc = 0;

    // Clear registers
    memset(&st->regs, 0, sizeof st->regs);

    // Set always-on bits
    st->regs.CCR.byte = 0x60;

    return rc;
}

int hc_do_reset(struct hc_state *st)
{
    int rc = 0;

    // Set PC to the reset vector located at $FFFE:$FFFF
    st->regs.PC.bytes.PCH = st->mem[0xFFFE];
    st->regs.PC.bytes.PCL = st->mem[0xFFFF];

    // reset puts the stack pointer at 0xFF, which is a terrible place for
    // a stack pointer to point.
    st->regs.SP.word = 0xFF;

    // disable interrupts so the application can safely set up the new (and
    // reasonable) stack pointer
    st->regs.CCR.bits.I = 1;

    // mark our new state
    st->state = RUNNING;

    return rc;
}

int hc_op_page(struct hc_state *st)
{
    int rc = 0; // default page is zero, the no-prefix page

    uint16_t pc = st->regs.PC.word;
    for (int i = pages_size - 1; i >= 0; i--) {
        bool found = true;
        for (int j = 0; j < pages[i].prebyte_cnt; j++) {
            if (pages[i].prebyte_val[j] != st->mem[pc + j]) {
                found = false;
                break;
            }
        }

        if (found) {
            rc = i;
            break;
        }
    }

    return rc;
}

int hc_do_op(struct hc_state *st)
{
    int rc = 0;

    int page = hc_op_page(st);
    assert(page < pages_size);
    st->offset += pages[page].prebyte_cnt;
    assert(st->offset < MAX_INSN_LEN);

    uint16_t *pc = &st->regs.PC.word;

    const struct opinfo *info = &opinfos[page][st->mem[*pc + st->offset]];
    enum op op = info->type;
    st->offset++; // account of opcode byte, so offset now points to "args"
    assert(st->offset < MAX_INSN_LEN);

    /// @todo if the PC wraps, do we set flags in CCR ?
    /// @todo a real machine doesn't have such an assertion as this, but it may
    /// execute some trap or some such thing; find out.
    assert(*pc + info->bytes < MEMORY_SIZE);
    (*pc) += info->bytes;
    rc = actors[op](st, info);

    st->cycle_count += info->cycles;
    st->offset = 0;

    return rc;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


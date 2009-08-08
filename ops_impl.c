#include "ops.h"

int handle_op_LDHX(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    uint16_t addr;
    enum mode mode = info->mode;
    uint16_t pc    = state->regs.PC.whole;
    uint16_t off   = state->offset;
    uint16_t n     = pc - info->bytes + off;

    /// @todo move this into a helper function
    switch (mode) {
        case MODE_IMM: addr = n;                                    break;
        case MODE_DIR: addr = state->mem[n];                        break;
        case MODE_EXT: addr = WORD(state->mem[n]);                  break;
        case MODE_SP1: addr = state->regs.SP.whole + state->mem[n]; break;
        case MODE_IX1: /* FALLTHROUGH */
        case MODE_IX2: /* FALLTHROUGH */
        case MODE_IX : addr = HX(*state);
                       if (mode == MODE_IX1) addr += state->mem[n];
                       if (mode == MODE_IX2) addr += WORD(state->mem[n]);
                       break;

        default:
            hc_abort("Invalid mode %s for op %s\n",
                     modenames[mode], opnames[info->type]);
            break;
    }

    state->regs.H = state->mem[addr];
    state->regs.X = state->mem[addr + 1];
    state->regs.CCR.bits.V = 0;
    state->regs.CCR.bits.N = !!state->regs.H & (1 << 7);
    state->regs.CCR.bits.Z = *(uint16_t*)&state->mem[addr] == 0;

    return rc;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


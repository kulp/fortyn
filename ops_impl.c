#include "ops.h"

/**
 * Decodes addresses.
 *
 * @param state         the uC state
 * @param info          the op info
 * @param pos           the address of the operand input data
 * @param [out] from    the address of the first operand
 * @param [out] to      the address of the second operand
 *
 * @return an error code, or zero on success
 */
static int _decode_addr(hc_state_t *state, const struct opinfo *info,
                        addr_t pos, addr_t *from, addr_t *to)
{
    int rc = 0;

    enum mode mode  = info->mode;
    addr_t    pc    = state->regs.PC.word;      // current PC (1 + current op)
    addr_t    sp    = state->regs.SP.word;

    switch (mode) {
        case MODE_IMM : *from = pos;                                    break;
        case MODE_DIR : *from = state->mem[pos];                        break;
        case MODE_EXT : *from = WORD(state->mem[pos]);                  break;
        case MODE_REL : *from = pc + (uint16_t)(int8_t)state->mem[pos]; break;
        case MODE_SP1 : *from = sp + state->mem[pos];                   break;
        case MODE_SP2 : *from = sp + WORD(state->mem[pos]);             break;

        case MODE_IX  :
        case MODE_IXP :
        case MODE_IX1 :
        case MODE_IX1P:
        case MODE_IX2 :
            *from = state->regs.HX.word;
            switch (mode) {
                case MODE_IXP : state->regs.HX.word++;
                case MODE_IX1P: if (mode != MODE_IX1P) break;
                case MODE_IX1 : *from += state->mem[pos];       break;
                case MODE_IX2 : *from += WORD(state->mem[pos]); break;
                default: break;
            }
            break;

        case MODE_IXPD: hc_error("Unimplemented mode IX+/D"); /// @todo
        case MODE_DIXP: hc_error("Unimplemented mode D/IX+"); /// @todo

        case MODE_DD  : *from = state->mem[pos++]; *to = state->mem[pos]; break;
        case MODE_IMD : *from =            pos++ ; *to = state->mem[pos]; break;

        case MODE_INH : // inherent: no address
        default       : if (from) *from = 0; if (to) *to = 0; break;
    }

    return rc;
}

int handle_op_ADC(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t   pc  = state->regs.PC.word;     // current PC (1 + current op)
    uint16_t off = state->offset;           // how many bytes past the PC
    addr_t   n   = pc - info->bytes + off;  // previous PC + offset

    uint16_t addr;
    rc = _decode_addr(state, info, n, &addr, NULL);

    uint8_t  a = state->regs.A;
    uint8_t  m = state->mem[addr];
    uint8_t  c = state->regs.CCR.bits.C;
    uint16_t r = a + m + c;

    state->regs.A = r & 0xFF;

    state->regs.CCR.bits.V = CHECK_OVERFLOW(a,m,r);
    state->regs.CCR.bits.H = CHECK_HALF_CARRY(a,m,c);
    state->regs.CCR.bits.N = CHECK_MSB(r);
    state->regs.CCR.bits.C = CHECK_CARRY(r);
    state->regs.CCR.bits.Z = r == 0;

    return rc;
}

int handle_op_LDHX(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t   pc  = state->regs.PC.word;     // current PC (1 + current op)
    uint16_t off = state->offset;           // how many bytes past the PC
    addr_t   n   = pc - info->bytes + off;  // previous PC + offset

    uint16_t addr;
    rc = _decode_addr(state, info, n, &addr, NULL);

    state->regs.HX.bytes.H = state->mem[addr];
    state->regs.HX.bytes.X = state->mem[addr + 1];

    state->regs.CCR.bits.V = 0;
    state->regs.CCR.bits.N = CHECK_MSB(state->regs.HX.bytes.H);
    state->regs.CCR.bits.Z = WORD(state->mem[addr]) == 0;

    return rc;
}

int handle_op_TXS(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    state->regs.SP.word = state->regs.HX.word - 1;

    return rc;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


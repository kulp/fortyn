#include "ops.h"

#include <signal.h>

/**
 * Decodes operand addresses.
 *
 * @param state         the uC state
 * @param info          the op info
 * @param [out] from    the address of the first operand
 * @param [out] to      the address of the second operand
 *
 * @return an error code, or zero on success
 */
static int _decode_addrs(hc_state_t *state, const struct opinfo *info,
                         addr_t *from, addr_t *to)
{
    int rc = 0;

    addr_t    pc   = state->regs.PC.word;       // current PC (1 + current op)
    uint16_t  off  = state->offset;             // how many bytes past the PC
    addr_t    pos  = pc - info->bytes + off;    // previous PC + offset
    enum mode mode = info->mode;
    addr_t    sp   = state->regs.SP.word;

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

static inline int _push(hc_state_t *state, uint8_t value)
{
    state->mem[state->regs.SP.word--] = value;
    return 0;
}

static inline int _pull(hc_state_t *state, uint8_t *value)
{
    *value = state->mem[++state->regs.SP.word];
    return 0;
}

int _handle_op_ADC_ADD(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    uint16_t addr;
    rc = _decode_addrs(state, info, &addr, NULL);

    enum op op = info->type;

    uint8_t  a = state->regs.A;
    uint8_t  m = state->mem[addr];
    uint8_t  c = state->regs.CCR.bits.C;
    uint16_t r = a + m + (op == OP_ADC ? c : 0);

    state->regs.A = r & 0xFF;

    state->regs.CCR.bits.V = CHECK_OVERFLOW(a,m,r);
    state->regs.CCR.bits.H = CHECK_HALF_CARRY(a,m,c);
    state->regs.CCR.bits.N = CHECK_MSB(r);
    state->regs.CCR.bits.C = CHECK_CARRY(r);
    state->regs.CCR.bits.Z = r == 0;

    return rc;
}

#pragma weak handle_op_ADC = _handle_op_ADC_ADD
#pragma weak handle_op_ADD = _handle_op_ADC_ADD

int handle_op_AIS(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    state->regs.SP.word += (addr_t)(int8_t)state->mem[addr];

    return rc;
}

int handle_op_AIX(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    state->regs.HX.word += (addr_t)(int8_t)state->mem[addr];

    return rc;
}

int _handle_op_AND_BIT(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    uint8_t temp = state->regs.A & state->mem[addr];
    if (info->type == OP_AND) state->regs.A = temp;
    // otherwise we are doing a BIT; only change CCR

    state->regs.CCR.bits.V = 0;
    state->regs.CCR.bits.N = CHECK_MSB(state->regs.A);
    state->regs.CCR.bits.Z = state->regs.A == 0;

    return rc;
}

#pragma weak handle_op_AND = _handle_op_AND_BIT
#pragma weak handle_op_BIT = _handle_op_AND_BIT

int _handle_op_ASR_LSR_ROR(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    enum op op = info->type;
    uint8_t *r;
    bool logical = false;
    switch (op) {
        case OP_LSR : logical = true; /* FALLTHROUGH */
        case OP_ROR :                 /* FALLTHROUGH */
        case OP_ASR : r = &state->mem[addr];        break;
        case OP_LSRA: logical = true; /* FALLTHROUGH */
        case OP_RORA:                 /* FALLTHROUGH */
        case OP_ASRA: r = &state->regs.A;           break;
        case OP_LSRX: logical = true; /* FALLTHROUGH */
        case OP_RORX:                 /* FALLTHROUGH */
        case OP_ASRX: r = &state->regs.HX.bytes.X;  break;
        default: hc_error("Invalid op");
    }

    unsigned oldc = state->regs.CCR.bits.C;
    unsigned c = state->regs.CCR.bits.C = *r & 1;
    *r = (logical ? *r >> 1 : (int8_t)*r >> 1);

    switch (op) {
        case OP_ROR: case OP_RORA: case OP_RORX:
            *r |= oldc << 7;
        default: break;
    }

    unsigned n = state->regs.CCR.bits.N = CHECK_MSB(*r);
    state->regs.CCR.bits.V = n ^ c;
    state->regs.CCR.bits.Z = *r == 0;

    return rc;
}

#pragma weak handle_op_ASR  = _handle_op_ASR_LSR_ROR
#pragma weak handle_op_ASRA = _handle_op_ASR_LSR_ROR
#pragma weak handle_op_ASRX = _handle_op_ASR_LSR_ROR
#pragma weak handle_op_LSR  = _handle_op_ASR_LSR_ROR
#pragma weak handle_op_LSRA = _handle_op_ASR_LSR_ROR
#pragma weak handle_op_LSRX = _handle_op_ASR_LSR_ROR
#pragma weak handle_op_ROR  = _handle_op_ASR_LSR_ROR
#pragma weak handle_op_RORA = _handle_op_ASR_LSR_ROR
#pragma weak handle_op_RORX = _handle_op_ASR_LSR_ROR

int handle_op_BGND(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    // breaking into the debugger is as close as we can get to making this
    // instruction mean something in simulation context
    raise(SIGTRAP);

    return rc;
}

int _handle_op_BRANCHES(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    bool cond = false;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);
    bool m = state->mem[addr];

    bool c = state->regs.CCR.bits.C;
    bool h = state->regs.CCR.bits.H;
    bool i = state->regs.CCR.bits.I;
    bool n = state->regs.CCR.bits.N;
    bool v = state->regs.CCR.bits.V;
    bool z = state->regs.CCR.bits.Z;

    enum op op = info->type;
    switch (op) {
        case OP_BLE : cond ^= 1; /* FALLTHROUGH */
        case OP_BGT : cond ^= !(z | (n ^ v));   break;
        case OP_BLT : cond ^= 1; /* FALLTHROUGH */
        case OP_BGE : cond ^= !(n ^ v);         break;
        case OP_BNE : cond ^= 1; /* FALLTHROUGH */
        case OP_BEQ : cond ^= z;                break;

        case OP_BLS : cond ^= 1; /* FALLTHROUGH */
        case OP_BHI : cond ^= !(c | z);         break;
        case OP_BCS : cond ^= 1; /* FALLTHROUGH */
        case OP_BCC : cond ^= !c;               break;

        case OP_BPL : cond ^= 1; /* FALLTHROUGH */
        case OP_BMI : cond ^= n;                break;
        case OP_BMC : cond ^= 1; /* FALLTHROUGH */
        case OP_BMS : cond ^= i;                break;
        case OP_BHCC: cond ^= 1; /* FALLTHROUGH */
        case OP_BHCS: cond ^= h;                break;

        case OP_BIL : cond ^= 1; /* FALLTHROUGH */
        case OP_BIH : cond ^= 0; /** @todo */   break;

        case OP_BRN : cond ^= 1; /* FALLTHROUGH */
        case OP_BRA : cond ^= 1;                break;

        case OP_BRCLR0: case OP_BRCLR1: case OP_BRCLR2: case OP_BRCLR3:
        case OP_BRCLR4: case OP_BRCLR5: case OP_BRCLR6: case OP_BRCLR7:
            cond ^= 1; /* FALLTHROUGH */
        case OP_BRSET0: case OP_BRSET1: case OP_BRSET2: case OP_BRSET3:
        case OP_BRSET4: case OP_BRSET5: case OP_BRSET6: case OP_BRSET7:
            cond ^= !!(m & (1 << ((op - OP_BRSET0) >> 1)));
            break;

        default: break;
    }

    if (cond) {
        addr_t   pc  = state->regs.PC.word;     // current PC (1 + current op)
        uint16_t off = state->offset;           // how many bytes past the PC
        addr_t   pos = pc - info->bytes + off;  // previous PC + offset

        int8_t rel = state->mem[pos];
        state->regs.PC.word = pc + rel;
    }

    return rc;
}

#pragma weak handle_op_BRA    = _handle_op_BRANCHES
#pragma weak handle_op_BLE    = _handle_op_BRANCHES
#pragma weak handle_op_BGT    = _handle_op_BRANCHES
#pragma weak handle_op_BLT    = _handle_op_BRANCHES
#pragma weak handle_op_BGE    = _handle_op_BRANCHES
#pragma weak handle_op_BNE    = _handle_op_BRANCHES
#pragma weak handle_op_BEQ    = _handle_op_BRANCHES
#pragma weak handle_op_BLS    = _handle_op_BRANCHES
#pragma weak handle_op_BHI    = _handle_op_BRANCHES
#pragma weak handle_op_BCS    = _handle_op_BRANCHES
#pragma weak handle_op_BCC    = _handle_op_BRANCHES
#pragma weak handle_op_BPL    = _handle_op_BRANCHES
#pragma weak handle_op_BMI    = _handle_op_BRANCHES
#pragma weak handle_op_BMC    = _handle_op_BRANCHES
#pragma weak handle_op_BMS    = _handle_op_BRANCHES
#pragma weak handle_op_BHCC   = _handle_op_BRANCHES
#pragma weak handle_op_BHCS   = _handle_op_BRANCHES
#pragma weak handle_op_BIL    = _handle_op_BRANCHES
#pragma weak handle_op_BIH    = _handle_op_BRANCHES
#pragma weak handle_op_BRN    = _handle_op_BRANCHES
#pragma weak handle_op_BRA    = _handle_op_BRANCHES

#pragma weak handle_op_BRSET0 = _handle_op_BRANCHES
#pragma weak handle_op_BRSET1 = _handle_op_BRANCHES
#pragma weak handle_op_BRSET2 = _handle_op_BRANCHES
#pragma weak handle_op_BRSET3 = _handle_op_BRANCHES
#pragma weak handle_op_BRSET4 = _handle_op_BRANCHES
#pragma weak handle_op_BRSET5 = _handle_op_BRANCHES
#pragma weak handle_op_BRSET6 = _handle_op_BRANCHES
#pragma weak handle_op_BRSET7 = _handle_op_BRANCHES
#pragma weak handle_op_BRCLR0 = _handle_op_BRANCHES
#pragma weak handle_op_BRCLR1 = _handle_op_BRANCHES
#pragma weak handle_op_BRCLR2 = _handle_op_BRANCHES
#pragma weak handle_op_BRCLR3 = _handle_op_BRANCHES
#pragma weak handle_op_BRCLR4 = _handle_op_BRANCHES
#pragma weak handle_op_BRCLR5 = _handle_op_BRANCHES
#pragma weak handle_op_BRCLR6 = _handle_op_BRANCHES
#pragma weak handle_op_BRCLR7 = _handle_op_BRANCHES

int _handle_op_BSET_BCLR(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    enum op op = info->type;
    uint8_t mask = 1 << ((op - OP_BRSET0) >> 1);

    // BCLRX opcodes are odd, BSETX are even
    if (op & 1) state->mem[addr] &= ~mask;
    else        state->mem[addr] |=  mask;

    return rc;
}

#pragma weak handle_op_BSET0 = _handle_op_BSET_BCLR
#pragma weak handle_op_BSET1 = _handle_op_BSET_BCLR
#pragma weak handle_op_BSET2 = _handle_op_BSET_BCLR
#pragma weak handle_op_BSET3 = _handle_op_BSET_BCLR
#pragma weak handle_op_BSET4 = _handle_op_BSET_BCLR
#pragma weak handle_op_BSET5 = _handle_op_BSET_BCLR
#pragma weak handle_op_BSET6 = _handle_op_BSET_BCLR
#pragma weak handle_op_BSET7 = _handle_op_BSET_BCLR
#pragma weak handle_op_BCLR0 = _handle_op_BSET_BCLR
#pragma weak handle_op_BCLR1 = _handle_op_BSET_BCLR
#pragma weak handle_op_BCLR2 = _handle_op_BSET_BCLR
#pragma weak handle_op_BCLR3 = _handle_op_BSET_BCLR
#pragma weak handle_op_BCLR4 = _handle_op_BSET_BCLR
#pragma weak handle_op_BCLR5 = _handle_op_BSET_BCLR
#pragma weak handle_op_BCLR6 = _handle_op_BSET_BCLR
#pragma weak handle_op_BCLR7 = _handle_op_BSET_BCLR

int handle_op_CLC(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    state->regs.CCR.bits.C = 0;

    return rc;
}

int handle_op_CLI(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    state->regs.CCR.bits.I = 0;

    return rc;
}

int _handle_op_CLR(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    enum op op = info->type;
    uint8_t *r;
    switch (op) {
        case OP_CLR : r = &state->mem[addr];        break;
        case OP_CLRA: r = &state->regs.A;           break;
        case OP_CLRH: r = &state->regs.HX.bytes.H;  break;
        case OP_CLRX: r = &state->regs.HX.bytes.X;  break;
        default: hc_error("Invalid op");
    }

    *r = 0;

    state->regs.CCR.bits.N = 0;
    state->regs.CCR.bits.V = 0;
    state->regs.CCR.bits.Z = 1;

    return rc;
}

#pragma weak handle_op_CLR  = _handle_op_CLR
#pragma weak handle_op_CLRA = _handle_op_CLR
#pragma weak handle_op_CLRH = _handle_op_CLR
#pragma weak handle_op_CLRX = _handle_op_CLR

int _handle_op_COM_COMA_COMX(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    uint8_t *r;
    switch (info->type) {
        case OP_COM : r = &state->mem[addr];        break;
        case OP_COMA: r = &state->regs.A;           break;
        case OP_COMX: r = &state->regs.HX.bytes.X;  break;
        default: break;
    }

    *r = ~*r;

    state->regs.CCR.bits.C = 1;
    state->regs.CCR.bits.N = CHECK_MSB(*r);
    state->regs.CCR.bits.V = 0;
    state->regs.CCR.bits.Z = *r == 0;

    return rc;
}

#pragma weak handle_op_COM  = _handle_op_COM_COMA_COMX
#pragma weak handle_op_COMA = _handle_op_COM_COMA_COMX
#pragma weak handle_op_COMX = _handle_op_COM_COMA_COMX

int handle_op_DAA(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    /// @note From Table A-2 in the HCS08 Family Reference Manual
    static const struct {
        uint8_t iC;
        uint8_t iLoTopA, iHiTopA;
        uint8_t iH;
        uint8_t iLoBotA, iHiBotA;
        uint8_t factor;
        uint8_t oC;
    } t[] = {
        { 0, 0x0, 0x9, 0, 0x0, 0x9, 0x00, 0 },
        { 0, 0x0, 0x8, 0, 0xA, 0xF, 0x06, 0 },
        { 0, 0x0, 0x9, 1, 0x0, 0x3, 0x06, 0 },
        { 0, 0xA, 0xF, 0, 0x0, 0x9, 0x60, 1 },
        { 0, 0x9, 0xF, 0, 0xA, 0xF, 0x66, 1 },
        { 0, 0xA, 0xF, 1, 0x0, 0x3, 0x66, 1 },
        { 1, 0x0, 0x2, 0, 0x0, 0x9, 0x60, 1 },
        { 1, 0x0, 0x2, 0, 0xA, 0xF, 0x66, 1 },
        { 1, 0x0, 0x3, 1, 0x0, 0x3, 0x66, 1 },
    };

    uint8_t c = state->regs.CCR.bits.C;
    uint8_t h = state->regs.CCR.bits.H;
    uint8_t TopA = state->regs.A >> 4;
    uint8_t BotA = state->regs.A & 0xF;

    for (unsigned i = 0; i < countof(t); i++) {
        if (c    == t[i].iC &&
            TopA >= t[i].iLoTopA &&
            TopA <= t[i].iHiTopA &&
            BotA >= t[i].iLoBotA &&
            BotA <= t[i].iHiBotA &&
            h    == t[i].iH)
        {
            state->regs.CCR.bits.C = t[i].oC;
            state->regs.A += t[i].factor;
        }
    }

    return rc;
}

int _handle_op_DBNZ_DBNZA_DBNZX(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    addr_t   pc  = state->regs.PC.word;     // current PC (1 + current op)
    uint16_t off = state->offset;           // how many bytes past the PC
    addr_t   pos = pc - info->bytes + off;  // previous PC + offset

    uint8_t *r;
    enum op op = info->type;
    switch (op) {
        case OP_DBNZ : r = &state->mem[addr]; pos++;    break;
        case OP_DBNZA: r = &state->regs.A;              break;
        case OP_DBNZX: r = &state->regs.HX.bytes.X;     break;
        default: break;
    }

    if ((*r)-- != 0) {
        int8_t rel = state->mem[pos];
        state->regs.PC.word = pc + rel;
    }

    return rc;
}

#pragma weak handle_op_DBNZ  = _handle_op_DBNZ_DBNZA_DBNZX
#pragma weak handle_op_DBNZA = _handle_op_DBNZ_DBNZA_DBNZX
#pragma weak handle_op_DBNZX = _handle_op_DBNZ_DBNZA_DBNZX

int _handle_op_DEC(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    enum op op = info->type;
    uint8_t *r;
    switch (op) {
        case OP_DEC : r = &state->mem[addr];        break;
        case OP_DECA: r = &state->regs.A;           break;
        case OP_DECX: r = &state->regs.HX.bytes.X;  break;
        default: hc_error("Invalid op");
    }

    uint8_t orig = *r;
    (*r)--;

    state->regs.CCR.bits.N = CHECK_MSB(*r);
    state->regs.CCR.bits.V = !(*r & 0x80) && (orig & 0x80);
    state->regs.CCR.bits.Z = *r == 0;

    return rc;
}

#pragma weak handle_op_DEC  = _handle_op_DEC
#pragma weak handle_op_DECA = _handle_op_DEC
#pragma weak handle_op_DECX = _handle_op_DEC

int handle_op_DIV(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    uint16_t dividend  = (state->regs.HX.bytes.H) << 8 | state->regs.A;
    uint8_t  divisor   =  state->regs.HX.bytes.X;

    if (divisor == 0) {
        state->regs.CCR.bits.C = 1;
    } else {
        uint16_t quotient  = dividend / divisor;
        uint8_t  remainder = dividend % divisor;

        if (quotient & 0xFF00) state->regs.CCR.bits.C = 1;
        state->regs.CCR.bits.Z = quotient == 0;

        state->regs.A = quotient & 0xFF;
        state->regs.HX.bytes.H = remainder;
    }

    return rc;
}

int _handle_op_EOR_ORA(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    enum op op = info->type;
    switch (op) {
        case OP_EOR: state->regs.A ^= state->mem[addr]; break;
        case OP_ORA: state->regs.A |= state->mem[addr]; break;
        default: break;
    }

    state->regs.CCR.bits.V = 0;
    state->regs.CCR.bits.N = CHECK_MSB(state->regs.A);
    state->regs.CCR.bits.Z = state->regs.A == 0;

    return rc;
}

#pragma weak handle_op_ORA = _handle_op_EOR_ORA
#pragma weak handle_op_EOR = _handle_op_EOR_ORA

int handle_op_INC(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    uint8_t result;

    if (info->mode == MODE_INH) {
        switch (info->type) {
            case OP_INCA: result = ++state->regs.A;          break;
            case OP_INCX: result = ++state->regs.HX.bytes.X; break;
            default: break;
        }
    } else {
        addr_t addr;
        _decode_addrs(state, info, &addr, NULL);

        result = ++state->mem[addr];
    }

    state->regs.CCR.bits.V = !CHECK_MSB(state->regs.A) & CHECK_MSB(result);
    state->regs.CCR.bits.N = CHECK_MSB(state->regs.A);
    state->regs.CCR.bits.Z = result == 0;

    return rc;
}

#pragma weak handle_op_INCA = handle_op_INC
#pragma weak handle_op_INCX = handle_op_INC

int _handle_op_JSR_BSR(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    uint16_t addr;
    rc = _decode_addrs(state, info, &addr, NULL);

    _push(state, state->regs.PC.bytes.PCL);
    _push(state, state->regs.PC.bytes.PCH);
    state->regs.PC.word = addr;

    return rc;
}

#pragma weak handle_op_BSR = _handle_op_JSR_BSR
#pragma weak handle_op_JSR = _handle_op_JSR_BSR

int _handle_op_LDA_LDX_STA_STX(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    bool store = false;

    uint8_t *r;
    switch (info->type) {
        case OP_STA: store = true; /* FALLTHROUGH */
        case OP_LDA: r = &state->regs.A;          break;
        case OP_STX: store = true; /* FALLTHROUGH */
        case OP_LDX: r = &state->regs.HX.bytes.X; break;
        default: break;
    }

    if (store)
        state->mem[addr] = *r;
    else
        *r = state->mem[addr];

    state->regs.CCR.bits.V = 0;
    state->regs.CCR.bits.N = CHECK_MSB(*r);
    state->regs.CCR.bits.Z = *r == 0;

    return rc;
}

#pragma weak handle_op_LDA = _handle_op_LDA_LDX_STA_STX
#pragma weak handle_op_LDX = _handle_op_LDA_LDX_STA_STX
#pragma weak handle_op_STA = _handle_op_LDA_LDX_STA_STX
#pragma weak handle_op_STX = _handle_op_LDA_LDX_STA_STX

int _handle_op_LDHX_STHX(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    uint16_t addr;
    rc = _decode_addrs(state, info, &addr, NULL);

    if (info->type == OP_STHX) {
        state->mem[addr]     = state->regs.HX.bytes.H;
        state->mem[addr + 1] = state->regs.HX.bytes.X;
    } else { // OP_LDHX
        state->regs.HX.bytes.H = state->mem[addr];
        state->regs.HX.bytes.X = state->mem[addr + 1];
    }

    state->regs.CCR.bits.V = 0;
    state->regs.CCR.bits.N = CHECK_MSB(state->regs.HX.bytes.H);
    state->regs.CCR.bits.Z = WORD(state->mem[addr]) == 0;

    return rc;
}

#pragma weak handle_op_LDHX = _handle_op_LDHX_STHX
#pragma weak handle_op_STHX = _handle_op_LDHX_STHX

int _handle_op_LSL_ROL(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    enum op op = info->type;
    uint8_t *r;
    switch (op) {
        case OP_LSL : r = &state->mem[addr];        break;
        case OP_LSLA: r = &state->regs.A;           break;
        case OP_LSLX: r = &state->regs.HX.bytes.X;  break;
        default: hc_error("Invalid op");
    }

    unsigned oldc = state->regs.CCR.bits.C;
    unsigned c = state->regs.CCR.bits.C = !!(*r & 0x80);
    *r <<= 1;

    switch (op) {
        case OP_ROL: case OP_ROLA: case OP_ROLX:
            *r |= oldc;
        default: break;
    }

    unsigned n = state->regs.CCR.bits.N = CHECK_MSB(*r);
    state->regs.CCR.bits.V = n ^ c;
    state->regs.CCR.bits.Z = *r == 0;

    return rc;
}

#pragma weak handle_op_LSL  = _handle_op_LSL_ROL
#pragma weak handle_op_LSLA = _handle_op_LSL_ROL
#pragma weak handle_op_LSLX = _handle_op_LSL_ROL
#pragma weak handle_op_ROL  = _handle_op_LSL_ROL
#pragma weak handle_op_ROLA = _handle_op_LSL_ROL
#pragma weak handle_op_ROLX = _handle_op_LSL_ROL

int handle_op_MUL(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    uint16_t result = state->regs.HX.bytes.X * state->regs.A;
    state->regs.HX.bytes.X = result >> 8;
    state->regs.A = result & 0xFF;

    state->regs.CCR.bits.H = 0;
    state->regs.CCR.bits.C = 0;

    return rc;
}

int _handle_op_NEG(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    enum op op = info->type;
    uint8_t *r;
    switch (op) {
        case OP_NEG : r = &state->mem[addr];        break;
        case OP_NEGA: r = &state->regs.A;           break;
        case OP_NEGX: r = &state->regs.HX.bytes.X;  break;
        default: hc_error("Invalid op");
    }

    uint8_t orig = *r;
    if (*r != 0x80)
        *r = -*r;

    state->regs.CCR.bits.C = !!*r;
    state->regs.CCR.bits.N = CHECK_MSB(*r);
    state->regs.CCR.bits.V = !!(*r & orig & 0x80);
    state->regs.CCR.bits.Z = *r == 0;

    return rc;
}

#pragma weak handle_op_NEG  = _handle_op_NEG
#pragma weak handle_op_NEGA = _handle_op_NEG
#pragma weak handle_op_NEGX = _handle_op_NEG

int handle_op_NOP(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    // do nothing

    return rc;
}

int _handle_op_PSH(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    uint8_t *r;
    switch (info->type) {
        case OP_PSHA: r = &state->regs.A;           break;
        case OP_PSHH: r = &state->regs.HX.bytes.H;  break;
        case OP_PSHX: r = &state->regs.HX.bytes.X;  break;
        default: hc_error("Invalid op");
    }

    _push(state, *r);

    return rc;
}

#pragma weak handle_op_PSHA = _handle_op_PSH
#pragma weak handle_op_PSHH = _handle_op_PSH
#pragma weak handle_op_PSHX = _handle_op_PSH

int _handle_op_PUL(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    uint8_t *r;
    switch (info->type) {
        case OP_PULA: r = &state->regs.A;           break;
        case OP_PULH: r = &state->regs.HX.bytes.H;  break;
        case OP_PULX: r = &state->regs.HX.bytes.X;  break;
        default: hc_error("Invalid op");
    }

    _pull(state, r);

    return rc;
}

#pragma weak handle_op_PULA = _handle_op_PUL
#pragma weak handle_op_PULH = _handle_op_PUL
#pragma weak handle_op_PULX = _handle_op_PUL

int handle_op_RSP(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    state->regs.SP.bytes.SPH = 0xFF;

    return rc;
}

int handle_op_RTS(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    _pull(state, &state->regs.PC.bytes.PCH);
    _pull(state, &state->regs.PC.bytes.PCL);

    return rc;
}

int _handle_op_SBC_SUB_CMP(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    enum op op = info->type;

    uint8_t o = state->regs.A;
    uint8_t m = state->mem[addr];
    uint8_t r = state->regs.A - m;

    if (op != OP_CMP) r = state->regs.A -= m;
    if (op == OP_SBC) r = state->regs.A -= state->regs.CCR.bits.C;

    state->regs.CCR.bits.C = state->mem[addr] > o;
    state->regs.CCR.bits.N = CHECK_MSB(r);
    state->regs.CCR.bits.V = ( (o & 0x80) && !(m & 0x80) && !(r & 0x80)) ||
                             (!(o & 0x80) &&  (m & 0x80) &&  (r & 0x80));
    state->regs.CCR.bits.Z = r == 0;

    return rc;
}

#pragma weak handle_op_SBC = _handle_op_SBC_SUB_CMP
#pragma weak handle_op_SUB = _handle_op_SBC_SUB_CMP
#pragma weak handle_op_CMP = _handle_op_SBC_SUB_CMP

int handle_op_SEC(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    state->regs.CCR.bits.C = 1;

    return rc;
}

int handle_op_SEI(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    state->regs.CCR.bits.I = 1;

    return rc;
}

int handle_op_SWI(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    _push(state, state->regs.PC.bytes.PCL);
    _push(state, state->regs.PC.bytes.PCH);
    _push(state, state->regs.HX.bytes.X);
    _push(state, state->regs.A);
    _push(state, state->regs.CCR.byte);
    state->regs.CCR.bits.I = 1;
    state->regs.PC.bytes.PCH = state->mem[0xFFFC];
    state->regs.PC.bytes.PCL = state->mem[0xFFFD];

    return rc;
}

int handle_op_TAP(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    state->regs.CCR.byte = state->regs.A;

    return rc;
}

int handle_op_TAX(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    state->regs.HX.bytes.X = state->regs.A;

    return rc;
}

int handle_op_TPA(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    state->regs.A = state->regs.CCR.byte;

    return rc;
}

int _handle_op_TST_TSTA_TSTX(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    addr_t addr;
    _decode_addrs(state, info, &addr, NULL);

    uint8_t r;
    enum op op = info->type;
    switch (op) {
        case OP_TST : r = state->mem[addr];         break;
        case OP_TSTA: r = state->regs.A;            break;
        case OP_TSTX: r = state->regs.HX.bytes.X;   break;
        default: break;
    }

    state->regs.CCR.bits.V = 0;
    state->regs.CCR.bits.N = CHECK_MSB(r);
    state->regs.CCR.bits.Z = r == 0;

    return rc;
}

#pragma weak handle_op_TST  = _handle_op_TST_TSTA_TSTX
#pragma weak handle_op_TSTA = _handle_op_TST_TSTA_TSTX
#pragma weak handle_op_TSTX = _handle_op_TST_TSTA_TSTX

int handle_op_TSX(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    state->regs.HX.word = state->regs.SP.word + 1;

    return rc;
}

int handle_op_TXA(hc_state_t *state, const struct opinfo *info)
{
    int rc = 0;

    state->regs.A = state->regs.HX.bytes.X;

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


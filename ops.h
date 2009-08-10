#ifndef OPS_H_
#define OPS_H_

#include "hc08.h"
#include <stdbool.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Type definitions
////////////////////////////////////////////////////////////////////////////////

/// Distinct operations (not literal values)
enum op {
    OP_INVALID,
    OP_ADC,
    OP_ADD,
    OP_AIS,
    OP_AIX,
    OP_AND,
    OP_ASR,
    OP_ASRA,
    OP_ASRX,
    OP_BCC,
    OP_BCLR0,
    OP_BCLR1,
    OP_BCLR2,
    OP_BCLR3,
    OP_BCLR4,
    OP_BCLR5,
    OP_BCLR6,
    OP_BCLR7,
    OP_BCS,
    OP_BEQ,
    OP_BGE,
    OP_BGND,
    OP_BGT,
    OP_BHCC,
    OP_BHCS,
    OP_BHI,
    OP_BIH,
    OP_BIL,
    OP_BIT,
    OP_BLE,
    OP_BLS,
    OP_BLT,
    OP_BMC,
    OP_BMI,
    OP_BMS,
    OP_BNE,
    OP_BPL,
    OP_BRA,
    OP_BRCLR0,
    OP_BRCLR1,
    OP_BRCLR2,
    OP_BRCLR3,
    OP_BRCLR4,
    OP_BRCLR5,
    OP_BRCLR6,
    OP_BRCLR7,
    OP_BRN,
    OP_BRSET0,
    OP_BRSET1,
    OP_BRSET2,
    OP_BRSET3,
    OP_BRSET4,
    OP_BRSET5,
    OP_BRSET6,
    OP_BRSET7,
    OP_BSET0,
    OP_BSET1,
    OP_BSET2,
    OP_BSET3,
    OP_BSET4,
    OP_BSET5,
    OP_BSET6,
    OP_BSET7,
    OP_BSR,
    OP_CBEQ,
    OP_CBEQA,
    OP_CBEQX,
    OP_CLC,
    OP_CLI,
    OP_CLR,
    OP_CLRA,
    OP_CLRH,
    OP_CLRX,
    OP_CMP,
    OP_COM,
    OP_COMA,
    OP_COMX,
    OP_CPHX,
    OP_CPX,
    OP_DAA,
    OP_DBNZ,
    OP_DBNZA,
    OP_DBNZX,
    OP_DEC,
    OP_DECA,
    OP_DECX,
    OP_DIV,
    OP_EOR,
    OP_INC,
    OP_INCA,
    OP_INCX,
    OP_JMP,
    OP_JSR,
    OP_LDA,
    OP_LDHX,
    OP_LDX,
    OP_LSL,
    OP_LSLA,
    OP_LSLX,
    OP_LSR,
    OP_LSRA,
    OP_LSRX,
    OP_MOV,
    OP_MUL,
    OP_NEG,
    OP_NEGA,
    OP_NEGX,
    OP_NOP,
    OP_NSA,
    OP_ORA,
    OP_PSHA,
    OP_PSHH,
    OP_PSHX,
    OP_PULA,
    OP_PULH,
    OP_PULX,
    OP_ROL,
    OP_ROLA,
    OP_ROLX,
    OP_ROR,
    OP_RORA,
    OP_RORX,
    OP_RSP,
    OP_RTI,
    OP_RTS,
    OP_SBC,
    OP_SEC,
    OP_SEI,
    OP_STA,
    OP_STHX,
    OP_STOP,
    OP_STX,
    OP_SUB,
    OP_SWI,
    OP_TAP,
    OP_TAX,
    OP_TPA,
    OP_TST,
    OP_TSTA,
    OP_TSTX,
    OP_TSX,
    OP_TXA,
    OP_TXS,
    OP_WAIT,
    OP_MAX
};

/// Addressing modes
enum mode {
    MODE_DD,
    MODE_DIR,
    MODE_DIXP,
    MODE_EXT,
    MODE_IMD,
    MODE_IMM,
    MODE_INH,
    MODE_IX,
    MODE_IXP,
    MODE_IXPD,
    MODE_IX1,
    MODE_IX1P,
    MODE_IX2,
    MODE_REL,
    MODE_SP1,
    MODE_SP2,
    MODE_MAX
};

/// General opcode information
struct opinfo {
    enum op     type;   ///< distinct operation type
    enum mode   mode;   ///< addressing mode
    uint8_t     opcode; ///< entire numeric operation code
    uint8_t     cycles; ///< how many cycles to complete
    uint8_t     bytes;  ///< how many bytes in operation
};

struct page {
    uint8_t index;          ///< which page this is
    bool    prebyte_cnt;    ///< how many prebytes are required for this page
    uint8_t prebyte_val[1]; ///< what the prebyte value(s) is / are
};

/// @todo define what the return value of an actor_t means
typedef int (*actor_t)(hc_state_t *state, const struct opinfo *info);

extern const char *opnames[];               ///< string names for each op
extern int opnames_size;                    ///< how many elements in opnames

extern const char *modenames[];             ///< string names for each mode
extern int modenames_size;                  ///< how many elements in modenames

extern const actor_t actors[];              ///< mapping from ops to executors
extern int actors_size;                     ///< how many elements in actors

extern const struct page pages[];           ///< descriptions of op pages
extern int pages_size;                      ///< how many elements in pages

extern const struct opinfo opinfos[][256];  ///< detailed op descriptions
extern int opinfos_size[];                  ///< how many elements in opinfos

#endif

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


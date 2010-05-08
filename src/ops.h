#ifndef OPS_H_
#define OPS_H_

#include "hc08.h"

#include <stdbool.h>
#include <stdint.h>

#ifndef countof
#define countof(X) (sizeof (X) / sizeof (X)[0])
#endif

#define OPS_   \
    R_(ADC)    \
    R_(ADD)    \
    R_(AIS)    \
    R_(AIX)    \
    R_(AND)    \
    R_(ASR)    \
    R_(ASRA)   \
    R_(ASRX)   \
    R_(BCC)    \
    R_(BCLR0)  \
    R_(BCLR1)  \
    R_(BCLR2)  \
    R_(BCLR3)  \
    R_(BCLR4)  \
    R_(BCLR5)  \
    R_(BCLR6)  \
    R_(BCLR7)  \
    R_(BCS)    \
    R_(BEQ)    \
    R_(BGE)    \
    R_(BGND)   \
    R_(BGT)    \
    R_(BHCC)   \
    R_(BHCS)   \
    R_(BHI)    \
    R_(BIH)    \
    R_(BIL)    \
    R_(BIT)    \
    R_(BLE)    \
    R_(BLS)    \
    R_(BLT)    \
    R_(BMC)    \
    R_(BMI)    \
    R_(BMS)    \
    R_(BNE)    \
    R_(BPL)    \
    R_(BRA)    \
    R_(BRCLR0) \
    R_(BRCLR1) \
    R_(BRCLR2) \
    R_(BRCLR3) \
    R_(BRCLR4) \
    R_(BRCLR5) \
    R_(BRCLR6) \
    R_(BRCLR7) \
    R_(BRN)    \
    R_(BRSET0) \
    R_(BRSET1) \
    R_(BRSET2) \
    R_(BRSET3) \
    R_(BRSET4) \
    R_(BRSET5) \
    R_(BRSET6) \
    R_(BRSET7) \
    R_(BSET0)  \
    R_(BSET1)  \
    R_(BSET2)  \
    R_(BSET3)  \
    R_(BSET4)  \
    R_(BSET5)  \
    R_(BSET6)  \
    R_(BSET7)  \
    R_(BSR)    \
    R_(CBEQ)   \
    R_(CBEQA)  \
    R_(CBEQX)  \
    R_(CLC)    \
    R_(CLI)    \
    R_(CLR)    \
    R_(CLRA)   \
    R_(CLRH)   \
    R_(CLRX)   \
    R_(CMP)    \
    R_(COM)    \
    R_(COMA)   \
    R_(COMX)   \
    R_(CPHX)   \
    R_(CPX)    \
    R_(DAA)    \
    R_(DBNZ)   \
    R_(DBNZA)  \
    R_(DBNZX)  \
    R_(DEC)    \
    R_(DECA)   \
    R_(DECX)   \
    R_(DIV)    \
    R_(EOR)    \
    R_(INC)    \
    R_(INCA)   \
    R_(INCX)   \
    R_(JMP)    \
    R_(JSR)    \
    R_(LDA)    \
    R_(LDHX)   \
    R_(LDX)    \
    R_(LSL)    \
    R_(LSLA)   \
    R_(LSLX)   \
    R_(LSR)    \
    R_(LSRA)   \
    R_(LSRX)   \
    R_(MOV)    \
    R_(MUL)    \
    R_(NEG)    \
    R_(NEGA)   \
    R_(NEGX)   \
    R_(NOP)    \
    R_(NSA)    \
    R_(ORA)    \
    R_(PSHA)   \
    R_(PSHH)   \
    R_(PSHX)   \
    R_(PULA)   \
    R_(PULH)   \
    R_(PULX)   \
    R_(ROL)    \
    R_(ROLA)   \
    R_(ROLX)   \
    R_(ROR)    \
    R_(RORA)   \
    R_(RORX)   \
    R_(RSP)    \
    R_(RTI)    \
    R_(RTS)    \
    R_(SBC)    \
    R_(SEC)    \
    R_(SEI)    \
    R_(STA)    \
    R_(STHX)   \
    R_(STOP)   \
    R_(STX)    \
    R_(SUB)    \
    R_(SWI)    \
    R_(TAP)    \
    R_(TAX)    \
    R_(TPA)    \
    R_(TST)    \
    R_(TSTA)   \
    R_(TSTX)   \
    R_(TSX)    \
    R_(TXA)    \
    R_(TXS)    \
    R_(WAIT)

// equivalent instructions (alternate mnemonics, same opcode)
#define OP_ASL  OP_LSL
#define OP_ASLA OP_LSLA
#define OP_ASLX OP_LSLX
#define OP_BLO  OP_BCS
#define OP_BHS  OP_BCC

#define MODES_ \
    R_(DD)     \
    R_(DIR)    \
    R_(DIXP)   \
    R_(EXT)    \
    R_(IMD)    \
    R_(IMM)    \
    R_(INH)    \
    R_(IX)     \
    R_(IXP)    \
    R_(IXPD)   \
    R_(IX1)    \
    R_(IX1P)   \
    R_(IX2)    \
    R_(REL)    \
    R_(SP1)    \
    R_(SP2)

////////////////////////////////////////////////////////////////////////////////
// Type definitions
////////////////////////////////////////////////////////////////////////////////

/// Distinct operations (not literal values)
enum op {
    OP_INVALID = 0,
#define R_(Op) OP_##Op,
    OPS_
#undef R_
    OP_MAX
};

#define OPCLASSES_ \
    OPCLASS_(DATA_MOVEMENT) \
    OPCLASS_(MATH) \
    OPCLASS_(LOGICAL) \
    OPCLASS_(SHIFT_ROTATE) \
    OPCLASS_(JUMP_BRANCH_LOOP_CONTROL) \
    OPCLASS_(STACK_RELATED) \
    OPCLASS_(MISC)

enum opclass {
    OPCLASS_INVALID = 0,
#define OPCLASS_(Opclass) OPCLASS_##Opclass,
    OPCLASSES_
#undef OPCLASS_
    OPCLASS_MAX
};

/// Addressing modes
enum mode {
#define R_(Mode) MODE_##Mode,
    MODES_
#undef R_
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
typedef int (*actor_t)(struct hc_state *state, const struct opinfo *info);

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

extern struct opclass_record {
    enum opclass opclass;
    size_t opcount;
    enum op *ops;
} opclass2op[];

extern enum opclass op2opclass[];

#endif

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


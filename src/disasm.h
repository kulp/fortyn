#include "hc08.h"
#include "ops.h"

struct operand {
    enum operand_type {
        OPERAND_IMMEDIATE,
        OPERAND_MEMORY,
        OPERAND_REGISTER,
        OPERAND_NONE,
    } type;
    enum mode mode; ///< addressing mode for this operand
    union {
        /// @todo move up to hc08.h if it is needed
        enum hc_register {
            HC_REGISTER_A,
            HC_REGISTER_CCR,
            HC_REGISTER_H,
            HC_REGISTER_X,
            HC_REGISTER_SP,
        } reg;
        struct {
            size_t size;    ///< size of address in bytes
            addr_t addr;    ///< address in HC memory
        } mem;
    } c;
};

/**
 * Disassembles an instruction in a given buffer.
 *
 * @param buf           the buffer from which to disassemble instructs
 * @param size          the size of the buffer
 * @param info  [out]   the decoded opinfo, or the opinfo for OP_INVALID
 * @param dlen  [out]   the length of the decoded instruction
 * @param op1   [out]   operand 1
 * @param op2   [out]   operand 2
 *
 * @return zero on success, non-zero on undifferentiated failure
 *
 * @p info is never set to null.
 */
int hc_disasm_instr(void *buf, size_t size, const struct opinfo **info,
                    size_t *dlen, struct operand *op1, struct operand *op2);

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


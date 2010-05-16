#include "disasm.h"

int hc_disasm_instr(void *buf, size_t size, const struct opinfo **info,
                    size_t *dlen, struct operand *op1, struct operand *op2)
{
    /// @todo
    if (info)
        *info = (const struct opinfo*)&opinfos[OP_INVALID];

    return -1;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


#include "disasm.h"

int hc_disasm_instr(void *buf, size_t size, struct opinfo * const *info, size_t *dlen)
{
    /// @todo
    if (info)
        *info = &opinfos[OP_INVALID];

    return 0;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


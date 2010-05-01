#ifndef HOOKS_H_
#define HOOKS_H_

#include "ops.h"

/**
 * @file
 * Defines the mechanism for hooking instructions.
 */

#define HOOK_OP_NOP   (1UL << 0)
#define HOOK_OP_READ  (1UL << 1)
#define HOOK_OP_WRITE (1UL << 2)
#define HOOK_OP_JUMP  (1UL << 3)
#define HOOK_OP_OTHER (1UL << 4)

typedef int (*hc_op_hook_t)(
    sstate_t *state,
    unsigned flags,
    enum op op
    /// @todo
    );

typedef int (*hc_opclass_hook_t)(
    sstate_t *state,
    unsigned flags,
    enum opclass opclass,
    enum op op
    /// @todo
    );

int hc_hook_instr(sstate_t *state, enum op op, hc_op_hook_t *hook);
int hc_hook_instr_class(sstate_t *state, enum opclass opclass, hc_opclass_hook_t *hook);

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

#endif


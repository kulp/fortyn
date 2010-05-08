#ifndef HOOKS_H_
#define HOOKS_H_

#include "ops.h"

/**
 * @file
 * Defines the mechanism for hooking instructions.
 */

enum hook_type {
    HOOK_TYPE_INVALID = 0,
    HOOK_TYPE_INSTR,
    HOOK_TYPE_INSTR_CLASS,
    HOOK_TYPE_PRED,
    HOOK_TYPE_MAX
};

enum hook_when {
    HOOK_WHEN_INVALID = 0,
    HOOK_WHEN_BEFORE,
    HOOK_WHEN_AFTER,
    HOOK_WHEN_INSTEAD,
    HOOK_WHEN_MAX
};

struct sim_state;
struct hc_state;

typedef int (*hc_op_hook)(
    struct sim_state *state,
    unsigned flags,
    enum op op
    /// @todo
    );

typedef int (*hc_opclass_hook)(
    struct sim_state *state,
    unsigned flags,
    enum opclass opclass,
    enum op op
    /// @todo
    );

/**
 * A predicate function; if returns true, associated hook is run. The hook is
 * passed in generically to be compared by address if needed, not to be called.
 */
/// @todo provide sim_state instead of hc_state ?
typedef bool (*hc_pred)(struct hc_state*, enum op, void (* const hook)());

/*
 * Calling convention:
 *  OP:      op, hook
 *  OPCLASS: opclass, hook
 *  PRED:    pred, hook
 */
int hc_hook_install(struct sim_state*, enum hook_when, enum hook_type, ...);

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

#endif


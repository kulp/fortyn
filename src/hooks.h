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

/// these are actually OR-able flags
enum hook_when {
    HOOK_WHEN_INVALID = 0,
    HOOK_WHEN_BEFORE  = 1,
    //HOOK_WHEN_INSTEAD = 2, /// @todo support this
    HOOK_WHEN_AFTER   = 4,
};

struct sim_state;
struct hc_state;

typedef int (*hc_op_hook)(
    struct sim_state *state,
    enum op op,
    /// @todo
    void *userdata
    );

typedef int (*hc_opclass_hook)(
    struct sim_state *state,
    enum opclass opclass,
    enum op op,
    /// @todo
    void *userdata
    );

/**
 * A predicate function; if returns true, associated hook is run. The hook is
 * passed in generically to be compared by address if needed, not to be called.
 */
/// @todo provide sim_state instead of hc_state ?
typedef bool (*hc_pred)(const struct hc_state*, const void *userdata, enum op, void (* const hook)());

/*
 * Calling convention:
 *  OP:      op, hook
 *  OPCLASS: opclass, hook
 *  PRED:    pred, hook
 */
int hc_hook_install(struct sim_state*, void *userdata, enum hook_when, enum hook_type, ...);

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

#endif


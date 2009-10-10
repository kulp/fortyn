#include "ops.h"
#include "hc08.h"
#include <stdbool.h>

#ifndef SIM_H_
#define SIM_H_

#define MAX_BREAKPOINTS 100

typedef struct sim_state sstate_t;

struct sim_state {
    hc_state_t hc_state;            ///< chip state (registers, memory, etc.)
    bool loaded;                    ///< whether memory has been loaded
    bool running;                   ///< whether simulation is running
    addr_t bp[MAX_BREAKPOINTS];     ///< breakpoint addresses
    short bpi;                      ///< index of next free slot in @c bp
    struct ui {
        int (*input)(sstate_t*);    ///< function to get UI interaction
        int (*up)(sstate_t*);       ///< function to update UI
        void *data;                 ///< UI-specific data store
    } ui;
    char memfile[200];              ///< name of loaded memory-contents file
};

int load_binary_file(struct sim_state *state, const char *filename);
int loop_iterate(struct sim_state *state);
int loop_continue(struct sim_state *state);

/// @todo don't use an enum for bitfields
enum hook_op {
    HOOK_OP_NOP   =  1,
    HOOK_OP_READ  =  2,
    HOOK_OP_WRITE =  4,
    HOOK_OP_JUMP  =  8,
    HOOK_OP_OTHER = 16,
};

typedef int (*subop_hook_t)(
    sstate_t *state,
    unsigned flags,
    enum op op
    /// @todo
    );

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

#endif /* SIM_H_ */


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
    char *memfile;                  ///< name of loaded memory-contents file
};

int load_binary_file(hc_state_t *state, const char *filename);
int loop_iterate(struct sim_state *state);
int loop_continue(struct sim_state *state);

#define HOOK_OP_NOP   (1UL << 0)
#define HOOK_OP_READ  (1UL << 1)
#define HOOK_OP_WRITE (1UL << 2)
#define HOOK_OP_JUMP  (1UL << 3)
#define HOOK_OP_OTHER (1UL << 4)

typedef int (*subop_hook_t)(
    sstate_t *state,
    unsigned flags,
    enum op op
    /// @todo
    );

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

#endif /* SIM_H_ */


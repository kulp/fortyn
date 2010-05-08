#ifndef SIM_H_
#define SIM_H_

#include "ops.h"
#include "hc08.h"
#include "hooks.h"

#include <stdbool.h>

#define MAX_BREAKPOINTS 100

struct hook_state;

struct sim_state {
    struct hc_state hc_state;            ///< chip state (regs, memory, etc.)
    bool loaded;                         ///< whether memory has been loaded
    bool running;                        ///< whether simulation is running
    addr_t bp[MAX_BREAKPOINTS];          ///< breakpoint addresses
    short bpi;                           ///< index of next free slot in @c bp
    struct ui {
        int (*input)(struct sim_state*); ///< function to get UI interaction
        int (*up)(struct sim_state*);    ///< function to update UI
        void *data;                      ///< UI-specific data store
    } ui;
    struct hook_state *hook_state;       ///< state of hooks
    char *memfile;                       ///< name of memory-contents file
};

int load_binary_file(struct hc_state *state, const char *filename);
int loop_iterate(struct sim_state *state);
int loop_continue(struct sim_state *state);

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

#endif /* SIM_H_ */


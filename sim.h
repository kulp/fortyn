#include "hc08.h"
#include <stdbool.h>

#define MAX_BREAKPOINTS 100

typedef struct sim_state sstate_t;

struct sim_state {
    hc_state_t hc_state;        ///< chip state (registers, memory, etc.)
    bool loaded;                ///< whether memory has been loaded
    bool running;               ///< whether simulation is running
    addr_t bp[MAX_BREAKPOINTS]; ///< breakpoint addresses
    short bpi;                  ///< index of next free slot in @c bp
    int (*uiinput)(sstate_t*);  ///< function to get UI interaction
    int (*uiup)(sstate_t*);     ///< function to update UI
    void *uidata;               ///< UI-specific data store
    char memfile[200];          ///< name of loaded memory-contents file
};

int load_binary_file(struct sim_state *state, const char *filename);
int loop_iterate(struct sim_state *state);
int loop_continue(struct sim_state *state);

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


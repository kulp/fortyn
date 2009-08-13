#include "hc08.h"
#include <stdbool.h>

struct sim_state {
    hc_state_t hc_state;
    bool loaded;
    bool running;
    void *uidata;
};

int load_binary_file(struct sim_state *state, const char *filename);
int loop_iterate(struct sim_state *st);


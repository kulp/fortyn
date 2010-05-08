/**
 * @file
 * *HC08 simulator
 */

#include "ops.h"
#include "sim.h"

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int loop_iterate(struct sim_state *state)
{
    int rc = 0;

    if (state->hc_state.state == RUNNING) {
        // Save a copy of the current state before we modify it, so that we can
        // compare it after an op and mark changed components of the state.
        /// @todo use a different allocation strategy for this sort of coupled
        /// malloc() / free() behavior that could use the same block
        struct hc_state *prev = malloc(sizeof *prev);
        memcpy(prev, &state->hc_state, sizeof *prev);
        prev->next = &state->hc_state;
        prev->prev = NULL;
        free(state->hc_state.prev);
        state->hc_state.prev = prev;

        hc_do_op(&state->hc_state);
    }

    free(state->hc_state.prev);
    state->hc_state.prev = NULL;

    return rc;
}

int load_binary_file(struct hc_state *state, const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return fd;

    char buf[BUFSIZ];
    ssize_t bytes;
    unsigned int pos = 0;

    while ((bytes = read(fd, buf, sizeof buf)) > 0) {
        if (pos + bytes > (signed)sizeof state->mem) {
            fprintf(stderr, "Input file size exceeds memory size\n");
            goto cleanup;
        }
        memcpy(&state->mem[pos], buf, bytes);
        pos += bytes;
    }

cleanup:
    close(fd);

    return pos;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


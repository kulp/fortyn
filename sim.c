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

    if (state->hc_state.state == RUNNING)
        hc_do_op(&state->hc_state);

    return rc;
}

int load_binary_file(struct sim_state *state, const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return fd;

    char buf[BUFSIZ];
    ssize_t bytes;
    unsigned int pos = 0;

    while ((bytes = read(fd, buf, sizeof buf)) > 0) {
        if (pos + bytes > sizeof state->hc_state.mem) {
            fprintf(stderr, "Input file size exceeds memory size\n");
            goto cleanup;
        }
        memcpy(&state->hc_state.mem[pos], buf, bytes);
        pos += bytes;
    }

    if (bytes >= 0)
        state->loaded = true;

cleanup:
    close(fd);

    return bytes;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


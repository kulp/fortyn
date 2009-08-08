/**
 * @file
 * *HC08 simulator
 */

#include "hc08.h"
#include "ops.h"

#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct sim_state {
    hc_state_t hc_state;
    bool loaded;
    bool running;
};

static const struct option longopts[] = {
    { "help" , required_argument, NULL, 'h' },
    { "image", required_argument, NULL, 'i' },
    { NULL },
};
static const char shortopts[] = "hi:";

int loop_iterate(struct sim_state *st)
{
    int rc = 0;

    return rc;
}

int load_binary_file(struct sim_state *state, const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return fd;

    char buf[BUFSIZ];
    ssize_t bytes;
    int pos = 0;

    while ((bytes = read(fd, buf, sizeof buf)) > 0) {
        memcpy(&state->hc_state.mem[pos], buf, bytes);
        pos += bytes;
    }

    if (bytes == 0)
        state->loaded = true;

    close(fd);

    return bytes;
}

void usage(const char *me)
{
    printf("Usage:\n"
           "  %s [ OPTIONS ]\n"
           "where OPTIONS are among\n"
           "  -h, --help    show this message\n"
           "  -i, --image=X load image file X (binary format)\n"
           , me);
}

int main(int argc, char *argv[])
{
    int rc = EXIT_SUCCESS;

    struct sim_state state = {
        .loaded  = false,
        .running = true,
    };

    extern char *optarg;
    int ch;
    while ((ch = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
        if (!optarg) optarg = "0";
        switch (ch) {
            case 'i': 
                rc = load_binary_file(&state, optarg);
                break;
            default : rc = EXIT_FAILURE; /* FALLTHROUGH */
            case 'h': usage(argv[0]); exit(rc);
        }
    }

    if (!state.loaded) {
        fprintf(stderr, "Image could not be loaded or was not specified\n");
        return EXIT_FAILURE;
    }

    while (state.running)
        rc = loop_iterate(&state);

    return rc;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


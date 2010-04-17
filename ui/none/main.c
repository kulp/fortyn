#include "sim.h"
#include <getopt.h>

static const struct option longopts[] = {
    { "help" ,       no_argument, NULL, 'h' },
    { "image", required_argument, NULL, 'i' },
    { NULL, 0, NULL, 0 },
};
static const char shortopts[] = "hi:";

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

    hc_state_init(&state.hc_state);
    hc_do_reset(&state.hc_state);

    while (state.running)
        rc = loop_iterate(&state);

    return rc;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


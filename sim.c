/**
 * @file
 * *HC08 simulator
 */

#include "hc08.h"

#include <stdbool.h>
#include <stdio.h>

static bool running = true;

int loop_iterate(hc_state_t *st)
{
    int rc = 0;

    return rc;
}

int main(int argc, char *argv[])
{
    int rc = 0;

    hc_state_t st;

    while (running)
        rc = loop_iterate(&st);

    return rc;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


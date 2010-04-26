#include "sim.h"
#include "hc08.h"
#include "ops.h"

#include <ncurses.h>
#include <limits.h>
#include <getopt.h>
#include <string.h>

#define EXIT_PROGRAM    2

struct curses_data {
    int rows;
    int cols;
    bool stepping;
};

static const struct option longopts[] = {
    { "help" , required_argument, NULL, 'h' },
    { "image", required_argument, NULL, 'i' },
    { NULL, 0, NULL, 0 },
};
static const char shortopts[] = "hi:";

int quit() { return EXIT_PROGRAM; }

int reload(struct sim_state *state)
{
    int rc = 0;

    rc = load_binary_file(&state->hc_state, state->memfile);
    if (rc > 0)
        state->loaded = true;

    hc_state_init(&state->hc_state);
    hc_do_reset(&state->hc_state);

    struct curses_data *uidata = state->ui.data;
    uidata->stepping = true;

    state->ui.up(state);

    return rc;
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

static int tobin(uint8_t byte, char *out)
{
    int pos = 0;

    for (int i = 0; i < CHAR_BIT; i++) {
        out[i] = '0' + !!(byte & (1 << (CHAR_BIT - i - 1)));
    }

    out[CHAR_BIT] = 0;

    return pos;
}

int set_breakpoint(struct sim_state *state)
{
    int rc = 0;

    char buf[10];
    /// @todo there has to be a better way of clearing the line
    mvprintw(10, 0, "                                                  ", MAX_BREAKPOINTS);

    if (state->bpi >= MAX_BREAKPOINTS) {
        mvprintw(10, 0, "Maximum number (%u) of breakpoints already set", MAX_BREAKPOINTS);
        return -1;
    }

    mvprintw(10, 0, "Set breakpoint at PC: 0x");
    echo();
    getnstr(buf, sizeof buf);
    addr_t addr = strtol(buf, NULL, 16);
    mvprintw(10, 0, "Breakpoint set at PC: 0x%04x", addr);
    state->bp[state->bpi++] = addr;
    noecho();

    return rc;
}

int screen_update(struct sim_state *state)
{
    int rc = 0;

    hc_state_t *hc = &state->hc_state;
    //struct curses_data *uidata = state->ui.data;

    int row = 0;

    char ccr[CHAR_BIT + 1];
    tobin(hc->regs.CCR.byte, ccr);

    int page = hc_op_page(hc);
    addr_t pc = hc->regs.PC.word;
    int pre = pages[page].prebyte_cnt;
    const struct opinfo *info = &opinfos[page][hc->mem[pc + pre]];

    // clear line
    mvprintw(row, 0, "                                               ");
    mvprintw(row++, 0, "PC : 0x%02x (%s", hc->regs.PC.word, opnames[info->type]);

    for (addr_t i = pre + 1; i < info->bytes; i++)
        printw(" 0x%02x", hc->mem[pc + i]);

    printw(")");

    mvprintw(row++, 0, "SP : 0x%04x", hc->regs.SP.word);
    mvprintw(row++, 0, "CCR: 0b%s", ccr);
    mvprintw(row++, 7,        "V__HINZC");
    mvprintw(row++, 0, "A  : 0x%02x", hc->regs.A);
    mvprintw(row++, 0, "H  : 0x%02x", hc->regs.HX.bytes.H);
    mvprintw(row++, 0, "X  : 0x%02x", hc->regs.HX.bytes.X);
    move(row++,0);

    refresh();

    return rc;
}

int loop_continue(struct sim_state *state)
{
    int rc = 0;

    struct curses_data *uidata = state->ui.data;
    uidata->stepping = false;

    return rc;
}

bool should_break(struct sim_state *state)
{
    bool breaking = false;

    for (int i = 0; !breaking && i < state->bpi; i++)
        if (state->bp[i] == state->hc_state.regs.PC.word)
            breaking = true;

    return breaking;
}

int screen_get_action(struct sim_state *state)
{
    int rc = 0;

    struct curses_data *uidata = state->ui.data;

    if (uidata->stepping || should_break(state)) {
        uidata->stepping = true;

        static int (*key_handlers[])(struct sim_state *) = {
            [' '] = loop_iterate,
            ['b'] = set_breakpoint,
            ['n'] = loop_iterate,
            ['c'] = loop_continue,
            ['q'] = quit,
            ['r'] = reload,
        };

        // wait for user input
        int ch = getch();
        if (ch > 0 && ch < (signed)countof(key_handlers) && key_handlers[ch])
            rc = key_handlers[ch](state);
    } else {
        rc = loop_iterate(state);
    }

    return rc;
}

int main(int argc, char *argv[])
{
    int rc = EXIT_SUCCESS;

    struct sim_state state = {
        .loaded  = false,
        .running = true,
        .ui = {
            .up    = screen_update,
            .input = screen_get_action,
        },
    };

    extern char *optarg;
    int ch;
    while ((ch = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
        if (!optarg) optarg = "0";
        switch (ch) {
            case 'i': 
                strncpy(state.memfile, optarg, sizeof state.memfile);
                break;
            default : rc = EXIT_FAILURE; /* FALLTHROUGH */
            case 'h': usage(argv[0]); exit(rc);
        }
    }

    initscr();
    keypad(stdscr, TRUE);
    noecho();

    struct curses_data uidata = { 0, 0, true };
    getmaxyx(stdscr,uidata.rows,uidata.cols);

    state.ui.data = &uidata;

    rc = reload(&state);

    if (!state.loaded) {
        fprintf(stderr, "Image could not be loaded or was not specified\n");
        return EXIT_FAILURE;
    }

    while (state.running && rc != EXIT_PROGRAM) {
        rc = state.ui.up(&state);
        rc = state.ui.input(&state);
    }

    endwin();

    return rc;
}

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


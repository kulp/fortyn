#include "sim.h"
#include "hc08.h"
#include "ops.h"

#include <ncurses.h>
#include <limits.h>
#include <getopt.h>

struct curses_data {
    int rows, cols;
};

static const struct option longopts[] = {
    { "help" , required_argument, NULL, 'h' },
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

int tobin(uint8_t byte, char *out)
{
    int pos = 0;

    for (int i = 0; i < CHAR_BIT; i++) {
        out[i] = '0' + !!(byte & (1 << (CHAR_BIT - i - 1)));
    }

    out[CHAR_BIT] = 0;

    return pos;
}

int screen_iterate(struct sim_state *state)
{
    int rc = 0;

    hc_state_t *hc = &state->hc_state;
    //struct curses_data *uidata = state->uidata;

    int row = 0;

    char ccr[CHAR_BIT + 1];
    tobin(hc->regs.CCR.byte, ccr);

    mvprintw(row++, 0, "PC : 0x%x", hc->regs.PC.word);
    mvprintw(row++, 0, "SP : 0x%x", hc->regs.SP.word);
    mvprintw(row++, 0, "CCR: 0b%s", ccr);
    mvprintw(row++, 0, "       V__HINZC");
    mvprintw(row++, 0, "A  : 0x%x", hc->regs.A);
    mvprintw(row++, 0, "H  : 0x%x", hc->regs.HX.bytes.H);
    mvprintw(row++, 0, "X  : 0x%x", hc->regs.HX.bytes.X);
    row++;

    int page = hc_op_page(hc);
    addr_t pc = hc->regs.PC.word;
    int pre = pages[page].prebyte_cnt;
    const struct opinfo *info = &opinfos[page][hc->mem[pc + pre]];

    // clear line
    mvprintw(row, 0, "                                               ");
    mvprintw(row++, 0, "Instruction: %s", opnames[info->type]);

    for (addr_t i = pre + 1; i < info->bytes; i++)
        printw(" 0x%02x", hc->mem[pc + i]);

    move(row++,0);

    // wait for user input
    getch();

    rc = loop_iterate(state);

    return rc;
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

    initscr();
    keypad(stdscr, TRUE);
    noecho();

    struct curses_data uidata;
    getmaxyx(stdscr,uidata.rows,uidata.cols);

    state.uidata = &uidata;

    while (state.running)
        rc = screen_iterate(&state);

    refresh();
    endwin();

    return rc;
}

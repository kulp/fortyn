static int add_hit = 0;

static int add_hook(struct sim_state *state, enum op op, void *userdata)
{
    add_hit++;

    return 0;
}

START_TEST(test_hook_add9)
{
    struct sim_state state = {
        .loaded = true,
        .running = true,
        .hc_state = {
            .mem = {
                #include "add9.s19.genH"
            },
        },
	};

    struct hc_state *hc = &state.hc_state;
    hc_state_init(hc);
    hc_do_reset(hc);
    hc_hook_install(&state, NULL, HOOK_WHEN_BEFORE | HOOK_WHEN_AFTER, HOOK_TYPE_INSTR, OP_ADD, add_hook);

	for (int i = 0; i < 7; i++)
        loop_iterate(&state);

	fail_unless(hc->regs.A == 9);
    fail_unless(add_hit == 2);
}
END_TEST


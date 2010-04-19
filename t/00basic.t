START_TEST(test_null)
{
    fail_if(0 != 0);
}
END_TEST

/// Test hc_state_init()
START_TEST(test_init)
{
    hc_state_t hc;
    uint8_t zeroes[sizeof hc.mem] = { 0 };
    hc_state_init(&hc);

    fail_unless(hc.state == POWER_OFF);
    fail_unless(hc.cycle_count == 0);
    fail_unless(hc.offset == 0);
    fail_unless(hc.regs.A == 0);
    fail_unless(hc.regs.HX.word == 0);
    fail_unless(hc.regs.SP.word == 0);
    fail_unless(hc.regs.PC.word == 0);
    fail_unless(hc.regs.CCR.byte == (1 << 5) || (1 << 6));
    fail_if(memcmp(hc.mem, zeroes, sizeof hc.mem));
    fail_unless(hc.next == hc.prev);
}
END_TEST

/// Test hc_do_reset
START_TEST(test_reset)
{
    hc_state_t hc;
    hc_state_init(&hc);
    hc_do_reset(&hc);

    fail_unless(hc.state == RUNNING);
    fail_unless(hc.regs.PC.bytes.PCH == hc.mem[0xFFFE]);
    fail_unless(hc.regs.PC.bytes.PCL == hc.mem[0xFFFF]);
    fail_unless(hc.regs.SP.word == 0xFF);
    fail_unless(hc.regs.CCR.bits.I == 1);
}
END_TEST

/// Test hc_op_page function (rather pointless check)
START_TEST(test_op_page_auto)
{
    hc_state_t hc = { .regs.PC.word = 0 };

    for (int page = 0; page < pages_size; page++)
        for (enum op op = 0; op < OP_MAX; op++)
            if (opinfos[page][op].type != OP_INVALID) {
                memcpy(&hc.mem[hc.regs.PC.word], pages[page].prebyte_val,
                        pages[page].prebyte_cnt);
                hc.mem[hc.regs.PC.word + pages[page].prebyte_cnt] = op;
                fail_unless(page == hc_op_page(&hc));
            }
}
END_TEST


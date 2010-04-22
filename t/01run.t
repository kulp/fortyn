START_TEST(test_add9)
{
    hc_state_t hc = {
		.mem = {
			[0x182C] = 0x45, 0x01, 0x80, 0x94, 0x9A, 0x9D, 0x4F, 0xAB, 0x09, 0x8F,
			[0xFFFE] = 0x18, 0x2C,
		},
	};
    hc_state_init(&hc);
    hc_do_reset(&hc);

	for (int i = 0; i < 7; i++)
		hc_do_op(&hc);

	fail_unless(hc.regs.A == 9);
}
END_TEST


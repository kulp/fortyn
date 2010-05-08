START_TEST(test_add9)
{
    struct hc_state hc = {
		.mem = {
			#include "add9.s19.genH"
		},
	};
    hc_state_init(&hc);
    hc_do_reset(&hc);

	for (int i = 0; i < 7; i++)
		hc_do_op(&hc);

	fail_unless(hc.regs.A == 9);
}
END_TEST


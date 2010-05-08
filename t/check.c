// standard
#include <stdlib.h>
#include <string.h>

// Check
#include <check.h>

// Sim
#include "sim.h"
#include "ops.h"

// Tests
#include "testlist.h"

Suite* basic_suite(void)
{
    Suite *s = suite_create("Basic");

    {
        TCase *tc = tcase_create("Core");
        tcase_add_test(tc, test_null);
        tcase_add_test(tc, test_init);
        tcase_add_test(tc, test_reset);
        tcase_add_test(tc, test_op_page_auto);
        suite_add_tcase(s, tc);
    }

    {
        TCase *tc = tcase_create("Run");
        tcase_add_test(tc, test_add9);
        suite_add_tcase(s, tc);
    }

    {
        TCase *tc = tcase_create("Hooks");
        tcase_add_test(tc, test_hook_add9);
        suite_add_tcase(s, tc);
    }

    return s;
}

int main(void)
{
    Suite *s = basic_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    int num_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


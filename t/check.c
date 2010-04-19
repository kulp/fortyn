#include <stdlib.h>
#include <check.h>

#include "testlist.h"

Suite* basic_suite(void)
{
    Suite *s = suite_create("Basic");

    TCase *tc = tcase_create("Core");
    tcase_add_test(tc, test_basic);
    suite_add_tcase(s, tc);

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


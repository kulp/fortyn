// standard
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// Check
#include <check.h>

// Sim
#include "sim.h"
#include "ops.h"

// Tests
#include "testlist.h"

#define SUITE_(SName,...) \
    Suite* SName##_suite(void) \
    { \
        Suite *Suite = suite_create(#SName); \
        __VA_ARGS__ \
        return Suite; \
    }

#define TCASE_(TName,...) \
    { \
        void (*tests[])(int) = { __VA_ARGS__ }; \
        static char names[] = #__VA_ARGS__; \
        size_t count = countof(tests); \
        TCase *tc = tcase_create(#TName); \
        char *name = names, *next; \
        for (size_t i = 0; i < count; i++) { \
            next = strchr(name, ','); \
            if (next) { \
                *next = 0; \
                while (isspace(next[1])) next++; \
            } \
            _tcase_add_test(tc, tests[i], name, 0, 0, 1); \
            name = next + 1; \
        } \
        suite_add_tcase(Suite, tc); \
    }

#include "testdefs.h"

int main(void)
{
    int num_failed = 0;

#define S_(X) \
    { \
        Suite *s = X##_suite(); \
        SRunner *sr = srunner_create(s); \
        srunner_run_all(sr, CK_VERBOSE); \
        num_failed += srunner_ntests_failed(sr); \
        srunner_free(sr); \
    }

    SUITES_

#undef S_

    return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


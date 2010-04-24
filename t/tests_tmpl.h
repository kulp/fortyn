#include "common_tmpl.h"

#define TEST_(Test) \
    FULL_(Suite,Testcase,Test)

struct test {
    enum test_id {
#define FULL_(S,T,N) TEST_ENUM_(S,T,N)=__LINE__,
#   include TEST_FILE
#undef FULL_
    } id;
    enum testcase_id testcase;
    enum suite_id suite;
    const char *name;
    void (*func)();
} TESTS_ARRAY_NAME(Suite,Testcase,Test)[] = {
#define FULL_(Suite,Testcase,Test) \
    [TEST_ENUM_(Suite,Testcase,Test)] = { TEST_ENUM_(Suite,Testcase,Test), TESTCASE_ENUM_(Suite,Testcase,Test), SUITE_ENUM_(Suite,Testcase,Test), #Test, test_##Test },
#   include TEST_FILE
#undef FULL_
    { 0 },
};


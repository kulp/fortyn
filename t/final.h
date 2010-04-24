#define TEST_ENUM_(S,T,N) TEST_##S##_##T##_##N
#define TEST_(Test) \
    FULL_(Suite,Testcase,Test)

struct test {
    enum test_id {
#define FULL_(S,T,N) TEST_ENUM_(S,T,N)=__LINE__,
#   include TEST_FILE
#undef FULL_
    } id;
    void (*func)();
} tests[] = {
#define FULL_(Suite,Testcase,Test) \
    [TEST_ENUM_(Suite,Testcase,Test)] = { TEST_ENUM_(Suite,Testcase,Test), { test_##Test, NULL } },
#   include TEST_FILE
#undef FULL_
    { 0 },
};

#define TEST_ELT_(S,T,N) \
    struct test *tests_##S##_##T##_##N[] = { \


#define TESTCASE_ENUM_(S,T,Unused) TESTCASE_##S##_##T
#define TESTCASE_(Testcase) \
    FULL_(Suite,Testcase,)

struct testcase {
    enum testcase_id {
#define FULL_(S,T,N) TESTCASE_ENUM_(S,T,Unused)=__LINE__,
#   include TESTCASE_FILE
#undef FULL_
    } id;
    struct test **tests;
} testcases[] = {
#define FULL_(Suite,Testcase,Test) \
    [TESTCASE_ENUM_(Suite,Testcase,Test)] = { TESTCASE_ENUM_(Suite,Testcase,Test), { &testcases[TEST_ENUM_(Suite,Testcase,)], NULL } },
#   include TESTCASE_FILE
#undef FULL_
    [TESTCASE_Basic_Core] = { TESTCASE_Basic_Core, { &tests[TEST_Basic_Core_Null], NULL } },
};

#define SUITE_ENUM_(S,Unused1,Unused2) SUITE_##S
#define SUITE_(Suite) \
    FULL_(Suite,,)

struct suite {
    enum suite_id {
#define FULL_(S,T,N) SUITE_ENUM_(S,T,N)=__LINE__,
#   include SUITE_FILE
#undef FULL_
    } id;
    struct testcase **testcases;
} suites[] = {
    [SUITE_Basic] = { SUITE_Basic, { &testcases[TESTCASE_Basic_Core], NULL } },
};


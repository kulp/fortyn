//#include TESTCASE_HEADER_

struct testcase TESTCASES_ARRAY_NAME_(Suite,Testcase,Test)[] = {
#define FULL_(Suite,Testcase,Test) \
    [TESTCASE_ENUM_(Suite,Testcase,Test)] = { TESTCASE_ENUM_(Suite,Testcase,Test), TESTS_ARRAY_NAME(Suite,Testcase) },
#   include TESTCASE_FILE
//    TESTCASE_(Core)
#undef FULL_
};


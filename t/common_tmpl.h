#define CAT_(X,Y) CAT__(X,Y)
#define CAT__(X,Y) X##_##Y
#define TESTS_ARRAY_NAME_(Suite,Testcase,Test) CAT_(tests,CAT_(Suite,Testcase))
#define TESTCASES_ARRAY_NAME_(Suite,Testcase,Test) CAT_(testcases,Suite)

#define TEST_ENUM_(S,T,N) TEST_##S##_##T##_##N
#define TESTCASE_ENUM_(S,T,Unused) TESTCASE_##S##_##T
#define SUITE_ENUM_(S,Unused1,Unused2) SUITE_##S


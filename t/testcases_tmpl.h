#include "common_tmpl.h"
//#include "testcases_tmpl.h"

#define TESTCASE_(Testcase) \
    FULL_(Suite,Testcase,)

extern struct testcase {
    enum testcase_id {
#define FULL_(S,T,N) TESTCASE_ENUM_(S,T,Unused)=__LINE__,
#   include TESTCASE_FILE
#undef FULL_
    } id;
    struct test **tests;
} TESTCASES_ARRAY_NAME_(Suite,Testcase,Test)[];


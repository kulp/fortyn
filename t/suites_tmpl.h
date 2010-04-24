#include "common_tmpl.h"

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
#define FULL_(Suite,Testcase,Test) \
    [SUITE_ENUM_(Suite,Testcase,Test)] = { SUITE_ENUM_(Suite,Testcase,Test), testcases_##Suite },
#   include SUITE_FILE
#undef FULL_
};


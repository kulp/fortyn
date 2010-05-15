#define SUITES_ \
    S_(Basic)

SUITE_(Basic,
    TCASE_( Core,    test_null,
                     test_init,
                     test_reset,
                     test_op_page_auto   )
    TCASE_( Run,     test_add9           )
    TCASE_( Hooks,   test_hook_add9      )
)


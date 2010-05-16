#define SUITES_ \
    S_(Basic) \
    S_(Disassembly) \


SUITE_(Basic,
    TCASE_( Core,    test_null,
                     test_init,
                     test_reset,
                     test_op_page_auto      )
    TCASE_( Run,     test_add9              )
    TCASE_( Hooks,   test_hook_add9         )
)

SUITE_(Disassembly,
    TCASE_( Core,    test_disasm_add9       )
)


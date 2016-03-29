#include "darwin_tests.h"

int main(void)
{
    int n_failed;
    SRunner *runner;
    runner = srunner_create(create_hid_utils_suite());
    srunner_add_suite(runner, create_util_suite());
    srunner_set_fork_status(runner, CK_NOFORK);

    srunner_run_all(runner, CK_ENV);
    n_failed = srunner_ntests_failed(runner);

    srunner_free(runner);

    return (n_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

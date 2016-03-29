#include "ocl_tests.h"

OCL_Link *oclink = NULL;

static void master_suite_setup(void)
{
    oclink = ocl_link_alloc();
    ocl_link_init(oclink);
}

static void master_suite_teardown(void)
{
    if (oclink == NULL) return;

    if (oclink->handle != NULL) {
        ocl_link_close(oclink);
    }

    ocl_link_free(oclink);
}

int main(void)
{
    int n_failed;
    SRunner *runner;
    runner = srunner_create(create_link_suite());
    srunner_add_suite(runner, create_fan_suite());
    srunner_add_suite(runner, create_led_suite());
    srunner_add_suite(runner, create_temp_suite());
    srunner_add_suite(runner, create_util_suite());
    srunner_set_fork_status(runner, CK_NOFORK);

    master_suite_setup();
    srunner_run_all(runner, CK_ENV);
    n_failed = srunner_ntests_failed(runner);
    master_suite_teardown();

    srunner_free(runner);

    return (n_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

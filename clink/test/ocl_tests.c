#include <check.h>
#include <stdlib.h>

#include "clink.h"

static OCL_Link *link = NULL;

static void basic_setup(void)
{
    link = ocl_link_alloc();
}

static void setup(void)
{
    basic_setup();
    ocl_link_init(link);
}

static void teardown(void)
{
    if (link == NULL) return;

    if (link->handle != NULL) {
        ocl_link_close(link);
    }

    ocl_link_free(link);
}

START_TEST(test_link_init)
{
    int rc = -1;
    rc = ocl_link_init(link);
    ck_assert_msg(rc == 0, "Failed to initialize link.");
}
END_TEST

START_TEST(test_link_get_device_id)
{
    int rc = 0;
    rc = ocl_link_get_device_id(link);
    fprintf(stderr, "Link DeviceID is %x\n", rc);
    ck_assert_int_ne(rc, 0);
}
END_TEST

START_TEST(test_get_fw_version)
{
    int rc = 0;
    rc = ocl_link_get_fw_version(link);
    fprintf(stderr, "Device FW Version is %X\n", rc);
    ck_assert_int_ne(rc, 0);
}
END_TEST

START_TEST(test_get_product_name)
{
    int rc;
    char *name;
    rc = ocl_link_get_product_name(link, &name);
    ck_assert_int_eq(rc, 0);
    fprintf(stderr, "Device ProductName is %s\n", name);
}
END_TEST

Suite *create_link_suite(void)
{
    Suite *suite;
    TCase *tc_init;
    TCase *tc_core;

    suite = suite_create("Link");

    tc_init = tcase_create("Init");
    tcase_add_checked_fixture(tc_init, basic_setup, teardown);
    tcase_add_test(tc_init, test_link_init);

    tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_link_get_device_id);
    tcase_add_test(tc_core, test_get_fw_version);
    suite_add_tcase(suite, tc_core);

    suite_add_tcase(suite, tc_init);

    return suite;
}

int main(void)
{
    int n_failed;
    Suite *link_suite;
    SRunner *runner;
    link_suite = create_link_suite();
    runner = srunner_create(link_suite);

    srunner_run_all(runner, CK_ENV);

    n_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (n_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

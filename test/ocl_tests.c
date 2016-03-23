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
    char name[256] = {0};
    rc = ocl_link_get_product_name(link, name);
    ck_assert_int_eq(rc, 0);
    fprintf(stderr, "Device ProductName is %s\n", name);
}
END_TEST

START_TEST(test_get_device_status)
{
    int rc;
    rc = ocl_link_get_device_status(link);
    ck_assert_int_eq(rc, 0);
    fprintf(stderr, "Device Status is %d\n", rc);
}
END_TEST

static OCL_Temp *temp = NULL;
static void temp_setup(void)
{
    setup();
    temp = ocl_temp_alloc(link);
}

static void temp_teardown(void)
{
    if (temp != NULL) ocl_temp_free(temp);
    teardown();
}

START_TEST(test_temp_sensors)
{
    int n_sensors;
    n_sensors = ocl_temp_get_temp_sensors(temp);
    fprintf(stderr, "Number of temp sensors: %d\n", n_sensors);
    ck_assert_int_ne(0, n_sensors);

    int rc;
    rc = ocl_temp_select_sensor(temp, 0);
    fprintf(stderr, "Selecting sensor: %d\n", rc);
    ck_assert_int_eq(rc, 0);

    float temperature = ocl_temp_get_temp(temp);
    fprintf(stderr, "Current Temperature: %.2f\n", temperature);

    rc = ocl_temp_select_sensor(temp, 1);
    fprintf(stderr, "Selecting sensor %d\n", rc);
    ck_assert_int_eq(rc, 1);

    temperature = ocl_temp_get_temp(temp);
    fprintf(stderr, "Current Temperature: %.2f\n", temperature);
}
END_TEST

Suite *create_link_suite(void)
{
    Suite *suite;
    TCase *tc_init;
    TCase *tc_core;
    TCase *tc_temp;

    suite = suite_create("Link");

    tc_init = tcase_create("Init");
    tcase_add_checked_fixture(tc_init, basic_setup, teardown);
    tcase_add_test(tc_init, test_link_init);

    tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_link_get_device_id);
    tcase_add_test(tc_core, test_get_fw_version);
    tcase_add_test(tc_core, test_get_product_name);
    tcase_add_test(tc_core, test_get_device_status);
    suite_add_tcase(suite, tc_core);

    tc_temp = tcase_create("Temp");
    tcase_add_checked_fixture(tc_temp, temp_setup, temp_teardown);
    tcase_add_test(tc_temp, test_temp_sensors);
    suite_add_tcase(suite, tc_temp);

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

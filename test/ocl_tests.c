#include <check.h>
#include <stdlib.h>

#include "clink.h"

static OCL_Link *oclink = NULL;

static void basic_setup(void)
{
    oclink = ocl_link_alloc();
}

static void setup(void)
{
    basic_setup();
    ocl_link_init(oclink);
}

static void teardown(void)
{
    if (oclink == NULL) return;

    if (oclink->handle != NULL) {
        ocl_link_close(oclink);
    }

    ocl_link_free(oclink);
}

START_TEST(test_link_get_device_id)
{
    int rc = 0;
    rc = ocl_link_get_device_id(oclink);
    fprintf(stderr, "Link DeviceID is %x\n", rc);
    ck_assert_int_ne(rc, 0);
}
END_TEST

START_TEST(test_get_fw_version)
{
    int rc = 0;
    rc = ocl_link_get_fw_version(oclink);
    fprintf(stderr, "Device FW Version is %X\n", rc);
    ck_assert_int_ne(rc, 0);
}
END_TEST

START_TEST(test_get_product_name)
{
    int rc;
    char name[256] = {0};
    rc = ocl_link_get_product_name(oclink, name);
    ck_assert_int_eq(rc, 0);
    fprintf(stderr, "Device ProductName is %s\n", name);
}
END_TEST

START_TEST(test_get_device_status)
{
    int rc;
    rc = ocl_link_get_device_status(oclink);
    ck_assert_int_eq(rc, 0);
    fprintf(stderr, "Device Status is %d\n", rc);
}
END_TEST

static OCL_Temp *temp = NULL;
static void temp_setup(void)
{
    temp = ocl_temp_alloc(oclink);
}

static void temp_teardown(void)
{
    if (temp != NULL) ocl_temp_free(temp);
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
    ck_assert_int_ne(temperature, 0);
    fprintf(stderr, "Current Temperature: %.2f\n", temperature);
}
END_TEST

START_TEST(test_temp_limits)
{
    int limit;
    limit = ocl_temp_get_temp_limit(temp);
    fprintf(stderr, "Temp Limit: %d\n", limit);
    ck_assert_int_eq(limit, 0);
}
END_TEST

static OCL_Fan *fan = NULL;

static void fan_setup(void)
{
    fan = ocl_fan_alloc(oclink);
}

static void fan_teardown(void)
{
    if (fan != NULL) ocl_fan_free(fan);
}

START_TEST(test_fan_ops)
{
    int n_fans = ocl_fan_connected_fans(fan);
    ck_assert_int_eq(n_fans, 5);
    fprintf(stderr, "Number of connected fans: %d\n", n_fans);
}
END_TEST

Suite *create_link_suite(void)
{
    Suite *suite;
    TCase *tc_core;
    TCase *tc_temp;
    TCase *tc_fan;

    suite = suite_create("Link");

    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_link_get_device_id);
    tcase_add_test(tc_core, test_get_fw_version);
    tcase_add_test(tc_core, test_get_product_name);
    tcase_add_test(tc_core, test_get_device_status);
    suite_add_tcase(suite, tc_core);

    tc_temp = tcase_create("Temp");
    tcase_add_checked_fixture(tc_temp, temp_setup, temp_teardown);
    tcase_add_test(tc_temp, test_temp_sensors);
    tcase_add_test(tc_temp, test_temp_limits);
    suite_add_tcase(suite, tc_temp);

    tc_fan = tcase_create("Fan");
    tcase_add_checked_fixture(tc_fan, fan_setup, fan_teardown);
    tcase_add_test(tc_fan, test_fan_ops);
    suite_add_tcase(suite, tc_fan);

    return suite;
}

int main(void)
{
    int n_failed;
    Suite *link_suite;
    SRunner *runner;
    setup();
    link_suite = create_link_suite();
    runner = srunner_create(link_suite);
    srunner_set_fork_status(runner, CK_NOFORK);

    srunner_run_all(runner, CK_ENV);

    n_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    teardown();

    return (n_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

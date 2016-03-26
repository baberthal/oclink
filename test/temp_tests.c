#include "ocl_tests.h"

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
    rc = ocl_temp_current_sensor(temp);
    ck_assert_int_eq(rc, 0);

    float temperature = ocl_temp_get_temp(temp);
    ck_assert_int_ne(temperature, 0);
    fprintf(stderr, "Current Temperature: %.2f\n", temperature);
}
END_TEST

START_TEST(test_temp_limits)
{
    float limit;
    int rc;
    rc = ocl_temp_set_temp_limit(temp, 75);
    ck_assert_int_eq(rc, 0);

    limit = ocl_temp_get_temp_limit(temp);
    fprintf(stderr, "Temp Limit: %.2f\n", limit);
    ck_assert_int_eq(limit, 75);

    rc = ocl_temp_set_temp_limit(temp, 70);
    ck_assert_int_eq(rc, 0);
    limit = ocl_temp_get_temp_limit(temp);
    ck_assert_int_eq(limit, 70);
}
END_TEST

Suite *create_temp_suite(void)
{
    Suite *suite;
    TCase *tc_core;
    suite = suite_create("Temperature");

    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, temp_setup, temp_teardown);
    tcase_add_test(tc_core, test_temp_sensors);
    tcase_add_test(tc_core, test_temp_limits);
    suite_add_tcase(suite, tc_core);

    return suite;
}

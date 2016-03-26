#include "ocl_tests.h"

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

    int rc = ocl_fan_current_fan(fan);
    fprintf(stderr, "Currently selected fan: %d\n", rc);
    ck_assert_int_le(rc, n_fans);

    int new_fan = 0;
    if (rc == 0) {
        new_fan++;
    }
    else {
        new_fan = rc - 1;
    }
    fprintf(stderr, "Selecting fan %d\n", new_fan);

    rc = ocl_fan_select_fan(fan, new_fan);
    ck_assert_int_eq(rc, 0);

    rc = ocl_fan_current_fan(fan);
    ck_assert_int_eq(rc, new_fan);
}
END_TEST

START_TEST(test_fan_info)
{
    int rc = ocl_fan_read_all_info(fan);
    ck_assert_int_eq(rc, 0);
    ck_assert_msg(ocl_list_count(fan->fan_info) == 5, "Wrong number of fans");

    int n_fans = ocl_fan_connected_fans(fan);
    ck_assert_int_eq(n_fans, 5);
    for (int i = 0; i < n_fans; i++) {
        rc = ocl_fan_select_fan(fan, i);
        ck_assert_int_eq(rc, 0);
        int cur_fan = ocl_fan_current_fan(fan);
        ck_assert_int_eq(cur_fan, i);
        int cur_rpm = ocl_fan_get_current_rpm(fan);
        ck_assert_int_ne(cur_rpm, 0);
        fprintf(stderr, "Selecting fan %d, current rpm is %d\n", i, cur_rpm);
    }
}
END_TEST

START_TEST(test_set_fan_info)
{
    CorsairFanInfo fan_info = {
        .mode = FixedRPM, .rpm = 1800,
    };

    int rc = ocl_fan_set_fans_info(fan, 0, fan_info);
    ck_assert_int_eq(rc, 0);

    ocl_fan_read_fans_info(fan, 0, &fan_info);
    ck_assert_int_eq(fan_info.mode, FixedRPM);

    ocl_fan_dump(fan);
}
END_TEST

Suite *create_fan_suite(void)
{
    Suite *suite;
    TCase *tc_core;
    suite = suite_create("Fan");

    tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, fan_setup, fan_teardown);
    tcase_add_test(tc_core, test_fan_ops);
    tcase_add_test(tc_core, test_fan_info);
    tcase_add_test(tc_core, test_set_fan_info);
    suite_add_tcase(suite, tc_core);

    return suite;
}

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

static OCL_Led *my_led = NULL;

static void led_setup(void)
{
    my_led = ocl_led_alloc(oclink);
}

static void led_teardown(void)
{
    if (my_led != NULL) ocl_led_free(my_led);
}

START_TEST(test_led_basic_ops)
{
    int n_leds = ocl_led_get_led_count(my_led);
    ck_assert_int_ne(n_leds, 0);
    fprintf(stderr, "Number of LEDs: %d\n", n_leds);
    int rc = ocl_led_select_led(my_led, 0);
    ck_assert_int_eq(rc, 0);

    CorsairLink_LEDMode mode = ocl_led_get_mode(my_led);
    fprintf(stderr, "LED Mode: %s (%X)\n", ocl_led_get_mode_string(mode), mode);
    struct OCLLedColor *color = ocl_led_get_color2(my_led);
    fprintf(stderr, "Color: %d, %d, %d\n", color->red, color->green,
            color->blue);
}
END_TEST

Suite *create_link_suite(void)
{
    Suite *suite;
    TCase *tc_core;
    TCase *tc_temp;
    TCase *tc_fan;
    TCase *tc_led;

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
    tcase_add_test(tc_fan, test_fan_info);
    tcase_add_test(tc_fan, test_set_fan_info);
    suite_add_tcase(suite, tc_fan);

    tc_led = tcase_create("Led");
    tcase_add_checked_fixture(tc_led, led_setup, led_teardown);
    tcase_add_test(tc_led, test_led_basic_ops);
    suite_add_tcase(suite, tc_led);

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

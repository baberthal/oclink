#include "ocl_tests.h"

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

START_TEST(test_led_modes)
{
    CorsairLink_LEDMode new_mode = StaticColor;
    int rc = ocl_led_set_mode(my_led, new_mode);
    ck_assert_int_eq(rc, new_mode);

    CorsairLink_LEDMode mode = ocl_led_get_mode(my_led);
    ck_assert_int_eq(mode, StaticColor);

    new_mode = TwoColorCycle;
    rc = ocl_led_set_mode(my_led, new_mode);
    ck_assert_int_eq(rc, 0);
}
END_TEST

START_TEST(test_led_mode_string)
{
    CorsairLink_LEDMode mode = StaticColor;
    char *mode_string = ocl_led_get_mode_string(mode);
    ck_assert_str_eq(mode_string, "Static Color");

    mode = TwoColorCycle;
    mode_string = ocl_led_get_mode_string(mode);
    ck_assert_str_eq(mode_string, "Two Color Cycle");

    mode = FourColorCycle;
    mode_string = ocl_led_get_mode_string(mode);
    ck_assert_str_eq(mode_string, "Four Color Cycle");

    mode = TemperatureColor;
    mode_string = ocl_led_get_mode_string(mode);
    ck_assert_str_eq(mode_string, "Temperature Color");

    mode = 7;

    mode_string = ocl_led_get_mode_string(mode);
    ck_assert_str_eq(mode_string, "Unknown");
}
END_TEST

START_TEST(test_led_temp_color)
{
    struct OCLLedColor led_color;
    int rc = ocl_led_get_temp_color(my_led, &led_color);
    ck_assert_int_eq(rc, 0);

    fprintf(stderr, "Led Color: %d, %d, %d\n", led_color.red, led_color.green,
            led_color.blue);
}
END_TEST

Suite *create_led_suite(void)
{
    Suite *suite;
    TCase *tc_core;

    suite = suite_create("Led");
    tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, led_setup, led_teardown);
    tcase_add_test(tc_core, test_led_basic_ops);
    tcase_add_test(tc_core, test_led_modes);
    tcase_add_test(tc_core, test_led_mode_string);
    tcase_add_test(tc_core, test_led_temp_color);
    suite_add_tcase(suite, tc_core);

    return suite;
}

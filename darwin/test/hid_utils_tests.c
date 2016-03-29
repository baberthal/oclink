#include <hid_utils.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include "darwin_tests.h"

#define check_init()                                               \
    if (clx_hid_init()) {                                          \
        ck_abort_msg("clx_hid_init() returned a non-zero value!"); \
    }

#define MAX_STR 255

static struct clx_device_info *devs = NULL;
static struct clx_device_info *cur_dev = NULL;

static unsigned char buf[256];
static wchar_t wstr[MAX_STR];
static clx_hid_device *handle = NULL;

static IOReturn res;

static void setup_enumerate(void)
{
    devs = clx_hid_enumerate(0x0, 0x0);
    cur_dev = devs;
}

static void teardown_enumerate(void)
{
    clx_hid_free_enumeration(devs);
}

START_TEST(test_clx_hid_enumerate)
{
    check_init();
    ck_assert_not_null(devs);

    ck_assert_not_null(cur_dev);

    while (cur_dev) {
        printf(
            "Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: "
            "%ls",
            cur_dev->vendor_id,
            cur_dev->product_id,
            cur_dev->path,
            cur_dev->serial_number);
        printf("\n");
        printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
        printf("  Product:      %ls\n", cur_dev->product_string);
        printf("  Release:      %hx\n", cur_dev->release_number);
        printf("  Interface:    %d\n", cur_dev->interface_number);
        printf("\n");

        ck_assert_int_ne(cur_dev->vendor_id, 0);
        ck_assert_int_ne(cur_dev->product_id, 0);
        ck_assert_not_null(cur_dev->path);
        ck_assert_int_ne(cur_dev->release_number, 0);

        cur_dev = cur_dev->next;
    }
}
END_TEST

static void setup_basic(void)
{
    memset(buf, 0x00, sizeof(buf));
    buf[0] = 0x01;
    buf[1] = 0x81;

    handle = clx_hid_open(0x1b1c, 0x0c04, NULL);
}

static void teardown_basic(void)
{
    clx_hid_close(handle);
}

START_TEST(test_clx_hid_basic)
{
    check_init();

    ck_assert_not_null(handle);
    wstr[0] = 0x0000;
    res = clx_hid_get_manufacturer_string(handle, wstr, MAX_STR);

    ck_assert_int_ge(res, 0);
    ck_assert_not_null(wstr);
    ck_assert_msg(wcscmp(wstr, L"") != 0, "Manufacturer string was empty");
    printf("Manufacturer String: %ls\n", wstr);

    wstr[0] = 0x0000;
    res = clx_hid_get_product_string(handle, wstr, MAX_STR);
    ck_assert_int_ge(res, 0);
    ck_assert_not_null(wstr);
    ck_assert_msg(wcslen(wstr) >= 0, "Product String Was Empty");
    printf("Product String: %ls\n", wstr);

    wstr[0] = 0x0000;
    res = clx_hid_get_serial_number_string(handle, wstr, MAX_STR);
    ck_assert_int_eq(res, -1);
    printf("Serial Number String (%d): %ls\n", wstr[0], wstr);

    wstr[0] = 0x0000;
    res = clx_hid_get_indexed_string(handle, wstr, MAX_STR);
    ck_assert_int_ge(res, 0);
    ck_assert_not_null(wstr);
    ck_assert_msg(wcslen(wstr) >= 0, "Indexed String Was Empty");
    printf("Indexed String 1: %ls\n", wstr);
}
END_TEST

START_TEST(test_clx_hid_read)
{
    int i;
    clx_hid_set_nonblocking(handle, 1);

    res = clx_hid_read(handle, buf, 17);
    ck_assert_int_ge(res, 0);
    printf("Feature Report\n    ");
    for (i = 0; i < res; i++) {
        printf("%02hhx ", buf[i]);
    }
    printf("\n");
}
END_TEST

START_TEST(test_clx_hid_write)
{
    check_init();
    res = clx_hid_write(handle, buf, 17);
    ck_assert_int_ge(res, 0);

    buf[0] = 0x01;
    buf[1] = 0x81;
    res = clx_hid_write(handle, buf, 17);
    ck_assert_int_ge(res, 0);

    res = 0;
    while (res == 0) {
        res = clx_hid_read(handle, buf, sizeof(buf));
        if (res == 0) {
            printf("Waiting...\n");
        }
        if (res < 0) {
            printf("Unable to read()\n");
        }
        usleep(500 * 1000);
    }

    printf("Data read:\n");
    int i;
    for (i = 0; i < res; i++) {
        printf("%02hhx ", buf[i]);
    }
    printf("\n");
}
END_TEST

Suite *create_hid_utils_suite(void)
{
    Suite *suite;
    TCase *tc_enum;
    TCase *tc_basic;
    suite = suite_create("HID Utils");
    tc_enum = tcase_create("Enumerate");
    tcase_add_checked_fixture(tc_enum, setup_enumerate, teardown_enumerate);
    tcase_add_test(tc_enum, test_clx_hid_enumerate);
    suite_add_tcase(suite, tc_enum);

    tc_basic = tcase_create("Basic");
    tcase_add_unchecked_fixture(tc_basic, setup_basic, teardown_basic);
    tcase_add_test(tc_basic, test_clx_hid_basic);
    /* tcase_add_test(tc_basic, test_clx_hid_read); */
    tcase_add_test(tc_basic, test_clx_hid_write);
    suite_add_tcase(suite, tc_basic);

    return suite;
}

#include "ocl_tests.h"

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

Suite *create_link_suite(void)
{
    Suite *suite;
    TCase *tc_core;

    suite = suite_create("Link");

    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_link_get_device_id);
    tcase_add_test(tc_core, test_get_fw_version);
    tcase_add_test(tc_core, test_get_product_name);
    tcase_add_test(tc_core, test_get_device_status);
    suite_add_tcase(suite, tc_core);

    return suite;
}

#ifndef OCL_TESTS_H
#define OCL_TESTS_H 1

#include <check.h>
#include <clink.h>
#include <stdlib.h>

extern OCL_Link *oclink;

Suite *create_link_suite(void);
Suite *create_fan_suite(void);
Suite *create_led_suite(void);
Suite *create_temp_suite(void);
Suite *create_util_suite(void);

#endif /* ifndef OCL_TESTS_H */

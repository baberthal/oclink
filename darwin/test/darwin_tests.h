#ifndef CLX_DARWIN_TESTS_H
#define CLX_DARWIN_TESTS_H 1

#include <check.h>
#include <stdlib.h>

Suite *create_hid_utils_suite(void);
Suite *create_util_suite(void);

#define ck_assert_not_null(V) ck_assert_msg((V) != NULL, #V " was NULL!")

#endif /* CLX_DARWIN_TESTS_H */

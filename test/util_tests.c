#include <check.h>
#include <stdlib.h>

#include "jml_debug.h"
#include "util.h"

static OCL_List *list = NULL;
static char *test1 = "test1 data";
static char *test2 = "test2 data";
static char *test3 = "test3 data";

static void basic_setup(void)
{
    list = ocl_list_create();
}

static void setup(void)
{
    basic_setup();
    ocl_list_unshift(list, test1);
    ocl_list_unshift(list, test2);
    ocl_list_unshift(list, test3);
}

static void teardown(void)
{
    ocl_list_destroy(list);
}

START_TEST(test_push_pop)
{
    ocl_list_push(list, test1);
    ck_assert_msg(ocl_list_last(list) == test1, "wrong last value.");

    ocl_list_push(list, test2);
    ck_assert_msg(ocl_list_last(list) == test2, "wrong last value.");

    ocl_list_push(list, test3);
    ck_assert_msg(ocl_list_last(list) == test3, "wrong last value.");
    ck_assert_msg(ocl_list_count(list) == 3, "wrong count");

    char *val = ocl_list_pop(list);
    ck_assert_msg(val == test3, "wrong value (pop)");

    val = ocl_list_pop(list);
    ck_assert_msg(val == test2, "wrong value (pop)");

    val = ocl_list_pop(list);
    ck_assert_msg(val == test1, "wrong value (pop)");
    ck_assert_int_eq(ocl_list_count(list), 0);
}
END_TEST

START_TEST(test_unshift)
{
    ocl_list_unshift(list, test1);
    ck_assert_msg(ocl_list_first(list) == test1, "wrong value (unshift)");

    ocl_list_unshift(list, test2);
    ck_assert_msg(ocl_list_first(list) == test2, "wrong value (unshift)");

    ocl_list_unshift(list, test3);
    ck_assert_msg(ocl_list_first(list) == test3, "wrong value (unshift)");
    ck_assert_int_eq(ocl_list_count(list), 3);
}
END_TEST

START_TEST(test_remove)
{
    char *value = ocl_list_remove(list, list->first->next);
    ck_assert_msg(value == test2, "wrong removed element");
    ck_assert_int_eq(ocl_list_count(list), 2);
    ck_assert_msg(ocl_list_first(list) == test3, "wrong first value (remove)");
    ck_assert_msg(ocl_list_last(list) == test1, "wrong last value (remove)");
}
END_TEST

START_TEST(test_shift)
{
    ck_assert_int_ne(ocl_list_count(list), 0);
    char *value = ocl_list_shift(list);
    ck_assert_msg(value == test3, "wrong value (shift)");

    value = ocl_list_shift(list);
    ck_assert_msg(value == test2, "wrong value (shift)");

    value = ocl_list_shift(list);
    ck_assert_msg(value == test1, "wrong value (shift)");
    ck_assert_int_eq(ocl_list_count(list), 0);
}
END_TEST

START_TEST(test_empty)
{
    ck_assert_int_eq(ocl_list_count(list), 3);
    ocl_list_empty(list);
    ck_assert_int_eq(ocl_list_count(list), 0);
}
END_TEST

static char *values_to_sort[] = {"XXXX", "1234", "abcd", "xjvef", "NDSS"};
#define VALUE_COUNT 5

static OCL_List *create_words(void)
{
    int i = 0;
    OCL_List *words = ocl_list_create();
    for (i = 0; i < VALUE_COUNT; i++) {
        ocl_list_push(words, values_to_sort[i]);
    }

    return words;
}

static int is_sorted(OCL_List *words)
{
    OCL_LIST_FOREACH(words, first, next, cur)
    {
        if (cur->next && strcmp(cur->value, cur->next->value) > 0) {
            debug_log("%s %s", (char *)cur->value, (char *)cur->next->value);
            return 0;
        }
    }

    return 1;
}

START_TEST(test_bubble_sort)
{
    OCL_List *words = create_words();
    int rc = ocl_list_bubble_sort(words, (ocl_list_compare_cb)strcmp);
    ck_assert_msg(rc == 0, "bubble sort failed.");
    ck_assert_msg(is_sorted(words), "words not sorted");

    rc = ocl_list_bubble_sort(words, (ocl_list_compare_cb)strcmp);
    ck_assert_msg(rc == 0, "bubble sort failed (list should already b sorted)");
    ck_assert_msg(is_sorted(words), "words not sorted!");

    ocl_list_destroy(words);
}
END_TEST

START_TEST(test_merge_sort)
{
    OCL_List *words = create_words();
    OCL_List *res = ocl_list_merge_sort(words, (ocl_list_compare_cb)strcmp);
    ck_assert_msg(is_sorted(res), "Words are not sorted (merge sort)");

    OCL_List *res2 = ocl_list_merge_sort(words, (ocl_list_compare_cb)strcmp);
    ck_assert_msg(is_sorted(res2), "should still be sorted");

    ocl_list_destroy(res2);
    ocl_list_destroy(res);
    ocl_list_destroy(words);
}
END_TEST

Suite *util_suite(void)
{
    Suite *suite;
    TCase *tc_core;
    TCase *tc_extra;
    TCase *tc_sort;

    suite = suite_create("Utils");
    tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, basic_setup, teardown);
    tcase_add_test(tc_core, test_push_pop);
    tcase_add_test(tc_core, test_unshift);
    suite_add_tcase(suite, tc_core);

    tc_extra = tcase_create("Extra");
    tcase_add_checked_fixture(tc_extra, setup, teardown);
    tcase_add_test(tc_extra, test_remove);
    tcase_add_test(tc_extra, test_shift);
    tcase_add_test(tc_extra, test_empty);
    suite_add_tcase(suite, tc_extra);

    tc_sort = tcase_create("Sorting Algos");
    tcase_add_test(tc_sort, test_bubble_sort);
    tcase_add_test(tc_sort, test_merge_sort);
    suite_add_tcase(suite, tc_sort);

    return suite;
}

int main(void)
{
    Suite *suite = util_suite();
    SRunner *runner = srunner_create(suite);
    int n_failed;
    srunner_run_all(runner, CK_ENV);
    n_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (n_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

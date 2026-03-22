#include "wrapper_posix.h"

#include <check.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

// Strong symbol overrides the weak posix_close_() defined in wrapper_posix.c.
// The linker picks this definition over the weak one, proving the link-seam works.

static int mock_close_called = 0;
static int32_t mock_close_retval = -99;

int32_t posix_close_(int32_t fd);

int32_t posix_close_(int32_t fd)
{
    (void)fd;
    mock_close_called = 1;
    return mock_close_retval;
}

START_TEST(test_posix_close_mock_is_invoked) {
    mock_close_called = 0;

    int32_t result = posix_close_(42);

    ck_assert_int_eq(mock_close_called, 1);
    ck_assert_int_eq(result, mock_close_retval);
}
END_TEST

static Suite *wrapper_posix_suite(void)
{
    Suite *s = suite_create("wrapper_posix");
    TCase *tc = tcase_create("link_seam");

    tcase_add_test(tc, test_posix_close_mock_is_invoked);
    suite_add_tcase(s, tc);

    return s;
}

int main(void)
{
    Suite *s = wrapper_posix_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, "reports/check/unit/wrapper_posix_test.xml");
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

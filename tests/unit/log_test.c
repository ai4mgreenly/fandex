#include "log/log.h"
#include <talloc.h>

#include <check.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Read all content written to a tmpfile into a static buffer. */
static char buf[1024];

static const char *read_tmpfile(FILE *f)
{
    rewind(f);
    size_t n = fread(buf, 1, sizeof(buf) - 1, f);
    buf[n] = '\0';
    /* Reset for the next write. */
    rewind(f);
    (void)ftruncate(fileno(f), 0);
    return buf;
}

START_TEST(test_log_info_prefix) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    fx_log_t *log = fx_log_init(ctx, f, FX_LOG_DEBUG);
    fx_log_info(log, "hello");

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, " INFO "));
    ck_assert_ptr_nonnull(strstr(out, "hello"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_warn_prefix) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    fx_log_t *log = fx_log_init(ctx, f, FX_LOG_DEBUG);
    fx_log_warn(log, "something");

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, " WARN "));
    ck_assert_ptr_nonnull(strstr(out, "something"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_error_prefix) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    fx_log_t *log = fx_log_init(ctx, f, FX_LOG_DEBUG);
    fx_log_error(log, "boom");

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, " ERROR "));
    ck_assert_ptr_nonnull(strstr(out, "boom"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_format_args) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    fx_log_t *log = fx_log_init(ctx, f, FX_LOG_DEBUG);
    fx_log_info(log, "count=%" PRId32, (int32_t)42);

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, "count=42"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_level_suppresses_info) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    fx_log_t *log = fx_log_init(ctx, f, FX_LOG_WARN);
    fx_log_info(log, "suppressed");

    const char *out = read_tmpfile(f);
    ck_assert_str_eq(out, "");

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_level_suppresses_warn) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    fx_log_t *log = fx_log_init(ctx, f, FX_LOG_ERROR);
    fx_log_warn(log, "suppressed");

    const char *out = read_tmpfile(f);
    ck_assert_str_eq(out, "");

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_debug_level_allows_info) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    fx_log_t *log = fx_log_init(ctx, f, FX_LOG_DEBUG);
    fx_log_info(log, "visible");

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, "visible"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

#ifdef DEBUG
START_TEST(test_log_debug_prefix) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    fx_log_t *log = fx_log_init(ctx, f, FX_LOG_DEBUG);
    fx_log_debug(log, "trace");

    const char *out = read_tmpfile(f);
    ck_assert_ptr_nonnull(strstr(out, " DEBUG "));
    ck_assert_ptr_nonnull(strstr(out, "trace"));

    fclose(f);
    talloc_free(ctx);
}
END_TEST

START_TEST(test_log_debug_suppressed_by_info_level) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    FILE *f = tmpfile();
    ck_assert_ptr_nonnull(f);

    fx_log_t *log = fx_log_init(ctx, f, FX_LOG_INFO);
    fx_log_debug(log, "suppressed");

    const char *out = read_tmpfile(f);
    ck_assert_str_eq(out, "");

    fclose(f);
    talloc_free(ctx);
}
END_TEST
#endif /* DEBUG */

static Suite *log_suite(void)
{
    Suite *s = suite_create("log");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, test_log_info_prefix);
    tcase_add_test(tc, test_log_warn_prefix);
    tcase_add_test(tc, test_log_error_prefix);
    tcase_add_test(tc, test_log_format_args);
    tcase_add_test(tc, test_log_level_suppresses_info);
    tcase_add_test(tc, test_log_level_suppresses_warn);
    tcase_add_test(tc, test_log_debug_level_allows_info);
#ifdef DEBUG
    tcase_add_test(tc, test_log_debug_prefix);
    tcase_add_test(tc, test_log_debug_suppressed_by_info_level);
#endif

    suite_add_tcase(s, tc);
    return s;
}

int main(void)
{
    Suite *s = log_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, "reports/check/unit/log_test.xml");
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

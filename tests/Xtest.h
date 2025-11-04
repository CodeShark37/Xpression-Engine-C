/**
	XPression Engine C (c)2025
	
	@author: Joshua (Codeshark37)
	@contact: labprogramming7@gmail.com
	
	Xtest.h
*/
#ifndef XTEST_H
#define XTEST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#include <unistd.h>
#endif

// ========================================
// Configuration & Statistics
// ========================================
typedef enum {
    LOG_NONE = 0,
    LOG_ERROR = 1,
    LOG_INFO = 2,
    LOG_DEBUG = 3
} LogLevel;

typedef enum {
    OUTPUT_DEFAULT = 0,
    OUTPUT_TAP = 1,
    OUTPUT_JUNIT = 2,
    OUTPUT_JSON = 3
} OutputFormat;

typedef struct {
    bool enable_timeout;
    size_t timeout_seconds;
    bool enable_benchmark;
    LogLevel log_level;
    const char *log_file;
    FILE *log_fp;
    OutputFormat output_format;
    const char *test_filter; // Regex pattern to filter tests
    bool random_order;
    unsigned int random_seed;
} TestConfig;

typedef struct {
    size_t total;
    size_t passed;
    size_t failed;
    size_t timeout;
    size_t skipped;
    const char *current_suite;
    double total_time_ms;
    clock_t start_time;
} TestStats;

typedef struct {
    size_t test_failed;
	size_t test_passed;
} TestState;

// ========================================
// Globals
// ========================================
extern TestConfig g_test_config;
extern TestStats g_test_stats;
extern TestState g_test_state;
extern jmp_buf g_timeout_jmp;

#ifdef _WIN32
extern HANDLE g_timeout_timer;
extern volatile bool g_timer_active;
extern volatile bool g_timeout_triggered;
#else
extern volatile sig_atomic_t g_timeout_triggered;
#endif

// ========================================
// Configuration API
// ========================================
static inline void test_set_timeout_seconds(int seconds) {
    g_test_config.enable_timeout = (seconds > 0);
    g_test_config.timeout_seconds = seconds;
}

static inline void test_set_benchmark_enabled(bool enable) {
    g_test_config.enable_benchmark = enable;
}
static inline void test_set_log_level(LogLevel level) {
    g_test_config.log_level = level;
}

static inline void test_set_log_file(const char *filename) {
    if (g_test_config.log_fp && g_test_config.log_fp != stdout) {
        fclose(g_test_config.log_fp);
    }
    
    if (filename) {
        g_test_config.log_file = filename;
        g_test_config.log_fp = fopen(filename, "w");
        if (!g_test_config.log_fp) {
            fprintf(stderr, "Failed to open log file: %s\n", filename);
            g_test_config.log_fp = stdout;
        }
    } else {
        g_test_config.log_fp = stdout;
    }
}

static inline void test_set_output_format(OutputFormat format) {
    g_test_config.output_format = format;
}

static inline void test_set_filter(const char *pattern) {
    g_test_config.test_filter = pattern;
}

static inline void test_set_random_order(bool enable, unsigned int seed) {
    g_test_config.random_order = enable;
    g_test_config.random_seed = seed;
}

#define TEST_CONFIG_TIMEOUT(seconds) test_set_timeout_seconds(seconds)
#define TEST_CONFIG_BENCHMARK(enable) test_set_benchmark_enabled(enable)
#define TEST_CONFIG_LOG_LEVEL(level) test_set_log_level(level)
#define TEST_CONFIG_LOG_FILE(file) test_set_log_file(file)
#define TEST_CONFIG_OUTPUT(format) test_set_output_format(format)
#define TEST_CONFIG_FILTER(pattern) test_set_filter(pattern)
#define TEST_CONFIG_RANDOM(enable, seed) test_set_random_order(enable, seed)
#define TEST_CONFIG_TIMEOUT(seconds) test_set_timeout_seconds(seconds)
#define TEST_CONFIG_BENCHMARK(enable) test_set_benchmark_enabled(enable)

// ========================================
// Internal Helpers
// ========================================
static inline void test_reset_state(void) {
    g_test_state.test_failed = 0;
    g_timeout_triggered = false;	
}

static inline void test_reset_stats(void) {
	g_test_stats.total = 0;
    g_test_stats.passed = 0;
    g_test_stats.failed = 0;
    g_test_stats.skipped = 0;
    g_test_stats.timeout = 0;
    g_test_stats.total_time_ms = 0;
}

// Logging helpers
static inline void test_log(LogLevel level, const char *format, ...) {
    if (level > g_test_config.log_level) return;
    
    FILE *out = g_test_config.log_fp ? g_test_config.log_fp : stdout;
    
    const char *level_str[] = {"", "ERROR", "INFO", "DEBUG"};
    
    // Timestamp
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(out, "[%s] [%s] ", timestamp, level_str[level]);
    
    va_list args;
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    
    fprintf(out, "\n");
    fflush(out);
}

// Test filter (simple substring match)
static inline bool test_should_run(const char *test_name) {
    if (!g_test_config.test_filter) return true;
    return strstr(test_name, g_test_config.test_filter) != NULL;
}


// Forward declarations
static void test_setup_timeout(int seconds);
static void test_cancel_timeout(void);
static double test_get_time_ms(void);

// ========================================
// Assertions
// ========================================
#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            printf("  [FAIL] Assertion failed\n"); \
            printf("    File: %s:%d\n", __FILE__, __LINE__); \
            printf("    Expression: %s\n", #cond); \
            g_test_state.test_failed = 1; \
        } \
    } while(0)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))
#define ASSERT_EQUAL(a, b) ASSERT_TRUE((a) == (b))
#define ASSERT_NOT_EQUAL(a, b) ASSERT_TRUE((a) != (b))
#define ASSERT_NULL(ptr) ASSERT_TRUE((ptr) == NULL)
#define ASSERT_NOT_NULL(ptr) ASSERT_TRUE((ptr) != NULL)

#define ASSERT_STR_EQUAL(a, b) \
    do { \
        if (strcmp((a), (b)) != 0) { \
            printf("  [FAIL] String assertion failed\n"); \
            printf("    File: %s:%d\n", __FILE__, __LINE__); \
            printf("    Expected: \"%s\"\n", (a)); \
            printf("    Got:      \"%s\"\n", (b)); \
            g_test_state.test_failed = 1; \
        } \
    } while(0)

#define TEST_CASES(name, type) \
    static type test_cases_##name[] =

#define RUN_TEST_CASES(name, type, test_func) \
    do { \
        g_test_stats.current_suite = #name; \
        test_log(LOG_INFO, "Starting test suite: %s", #name); \
        printf("\n=== Test Suite: %s ===\n", #name); \
        size_t count = sizeof(test_cases_##name) / sizeof(test_cases_##name[0]); \
        for (size_t i = 0; i < count; i++) { \
            type *tc = &test_cases_##name[i]; \
            \
            /* Check filter */ \
            if (!test_should_run(tc->desc)) { \
                test_log(LOG_DEBUG, "Skipping test (filtered): %s", tc->desc); \
                g_test_stats.skipped++; \
                continue; \
            } \
            \
            g_test_stats.total++; \
            test_reset_state(); \
            printf("\n[TEST %u/%u] %s\n", i + 1, count, tc->desc); \
            test_log(LOG_INFO, "Running test: %s", tc->desc); \
            \
            double start_time = 0; \
            if (g_test_config.enable_benchmark) { \
                fflush(stdout); \
                start_time = test_get_time_ms(); \
            } \
            \
            if (g_test_config.enable_timeout) { \
                if (setjmp(g_timeout_jmp) == 0) { \
                    test_setup_timeout(g_test_config.timeout_seconds); \
                    test_func(tc); \
                    test_cancel_timeout(); \
                } else { \
                    printf("  [TIMEOUT] Test exceeded %u seconds\n", g_test_config.timeout_seconds); \
                    test_log(LOG_ERROR, "Test timeout: %s", tc->desc); \
                    g_test_stats.timeout++; \
                    g_test_state.test_failed = true; \
                } \
            } else { \
                test_func(tc); \
            } \
            \
            if (g_test_config.enable_benchmark) { \
                fflush(stdout); \
                double elapsed = test_get_time_ms() - start_time; \
                printf("  [TIME] %.3f ms\n", elapsed); \
                test_log(LOG_DEBUG, "Test time: %.3f ms", elapsed); \
                g_test_stats.total_time_ms += elapsed; \
            } \
            \
            if (!g_test_state.test_failed) { \
                g_test_stats.passed++; \
                printf("  [PASS]\n"); \
                test_log(LOG_INFO, "Test passed: %s", tc->desc); \
            } else { \
                g_test_stats.failed++; \
                test_log(LOG_ERROR, "Test failed: %s", tc->desc); \
            } \
        } \
        test_log(LOG_INFO, "Finished test suite: %s", #name); \
    } while(0)

// ========================================
// Parametrized Tests (Data-Driven style)
// ========================================
#define TEST_PARAM_CASES(name, type, param_type) \
    typedef struct { \
        const char *desc; \
        param_type values[100]; \
        size_t count; \
    } TestParam_##name; \
    static TestParam_##name test_param_##name[] =

#define RUN_PARAM_CASES(name, param_type, test_func) \
    do { \
        g_test_stats.current_suite = #name; \
        printf("\n=== Test Suite (Parametrized): %s ===\n", #name); \
        size_t suite_count = sizeof(test_param_##name) / sizeof(test_param_##name[0]); \
        for (size_t s = 0; s < suite_count; s++) { \
            TestParam_##name *suite = &test_param_##name[s]; \
            printf("\n[TEST PARAM] %s\n", suite->desc); \
            for (size_t i = 0; i < suite->count; i++) { \
                g_test_stats.total++; \
                test_reset_state(); \
                param_type value = suite->values[i]; \
                printf("  [Param %u/%u] ", i + 1, suite->count); \
                \
                double start_time = 0; \
                if (g_test_config.enable_benchmark) { \
                    fflush(stdout); \
                    start_time = test_get_time_ms(); \
                } \
                \
                if (g_test_config.enable_timeout) { \
                    if (setjmp(g_timeout_jmp) == 0) { \
                        test_setup_timeout(g_test_config.timeout_seconds); \
                        test_func(&value); \
                        test_cancel_timeout(); \
                    } else { \
                        printf("[TIMEOUT] "); \
                        g_test_stats.timeout++; \
                        g_test_state.test_failed = true; \
                    } \
                } else { \
                    test_func(&value); \
                } \
                \
                if (g_test_config.enable_benchmark) { \
                    fflush(stdout); \
                    double elapsed = test_get_time_ms() - start_time; \
                    printf("[TIME] %.3f ms ", elapsed); \
                    g_test_stats.total_time_ms += elapsed; \
                } \
                \
                if (!g_test_state.test_failed) { \
                    g_test_stats.passed++; \
                    printf("[PASS]\n"); \
                } else { \
                    g_test_stats.failed++; \
                    printf("[FAIL]\n"); \
                } \
            } \
        } \
    } while(0)

// ========================================
// Repeat Tests (Data-Driven style)
// ========================================
#define TEST_REPEAT_CASES(name, type) \
    typedef struct { \
        const char *desc; \
        size_t repeat_count; \
        type data; \
    } TestRepeat_##name; \
    static TestRepeat_##name test_repeat_##name[] =

#define RUN_REPEAT_CASES(name, type, test_func) \
    do { \
        g_test_stats.current_suite = #name; \
        printf("\n=== Test Suite (Repeat): %s ===\n", #name); \
        size_t suite_count = sizeof(test_repeat_##name) / sizeof(test_repeat_##name[0]); \
        for (size_t s = 0; s < suite_count; s++) { \
            TestRepeat_##name *suite = &test_repeat_##name[s]; \
            printf("\n[TEST REPEAT] %s (%u times)\n", suite->desc, suite->repeat_count); \
            for (size_t i = 0; i < suite->repeat_count; i++) { \
                g_test_stats.total++; \
                test_reset_state(); \
                printf("  [Run %u/%u] ", i + 1, suite->repeat_count); \
                \
                double start_time = 0; \
                if (g_test_config.enable_benchmark) { \
                    fflush(stdout); \
                    start_time = test_get_time_ms(); \
                } \
                \
                if (g_test_config.enable_timeout) { \
                    if (setjmp(g_timeout_jmp) == 0) { \
                        test_setup_timeout(g_test_config.timeout_seconds); \
                        test_func(&suite->data); \
                        test_cancel_timeout(); \
                    } else { \
                        printf("[TIMEOUT] "); \
                        g_test_stats.timeout++; \
                        g_test_state.test_failed = true; \
                    } \
                } else { \
                    test_func(&suite->data); \
                } \
                \
                if (g_test_config.enable_benchmark) { \
                    fflush(stdout); \
                    double elapsed = test_get_time_ms() - start_time; \
                    printf("[TIME] %.3f ms ", elapsed); \
                    g_test_stats.total_time_ms += elapsed; \
                } \
                \
                if (!g_test_state.test_failed) { \
                    g_test_stats.passed++; \
                    printf("[PASS]\n"); \
                } else { \
                    g_test_stats.failed++; \
                    printf("[FAIL]\n"); \
                } \
            } \
        } \
    } while(0)
		

// ========================================
// Test Report
// ========================================
static inline int test_report(void) {
    printf("\n=== Test Report ===\n");
    printf("Total:   %u\n", g_test_stats.total);
    printf("Passed:  %u\n", g_test_stats.passed);
    printf("Failed:  %u\n", g_test_stats.failed);
    if (g_test_config.enable_timeout && g_test_stats.timeout > 0) {
        printf("Timeout: %u\n", g_test_stats.timeout);
    }
    if (g_test_config.enable_benchmark) {
        fflush(stdout);
        printf("Total time: %.3f ms\n", g_test_stats.total_time_ms);
        if (g_test_stats.total > 0) {
            printf("Avg time:   %.3f ms/test\n", 
                   g_test_stats.total_time_ms / g_test_stats.total);
        }
    }
    if (g_test_stats.failed == 0 && g_test_stats.timeout == 0) {
        printf("\nAll tests passed!\n");
    } else {
        printf("\nSome tests failed.\n");
    }
    return (!g_test_stats.failed && !g_test_stats.timeout ? 0 : 1);
}

#define TEST_REPORT() test_report()

// ========================================
// Platform-Specific Implementations
// ========================================
#ifdef _WIN32

static VOID CALLBACK test_timeout_callback(LPVOID lpArg, DWORD dwLowValue, DWORD dwHighValue) {
    (void)lpArg;
    (void)dwLowValue;
    (void)dwHighValue;
    if (g_timer_active) {
        g_timeout_triggered = true;
        longjmp(g_timeout_jmp, 1);
    }
}

static inline void test_setup_timeout(int seconds) {
    g_timeout_triggered = false;
    g_timer_active = true;
    if (!g_timeout_timer) {
        g_timeout_timer = CreateWaitableTimer(NULL, TRUE, NULL);
    }
    LARGE_INTEGER due;
    due.QuadPart = -(LONGLONG)seconds * 10000000LL;
    SetWaitableTimer(g_timeout_timer, &due, 0, test_timeout_callback, NULL, FALSE);
}

static inline void test_cancel_timeout(void) {
    g_timer_active = false;
    if (g_timeout_timer)
        CancelWaitableTimer(g_timeout_timer);
}

static inline double test_get_time_ms(void) {
    static double qpf = 0.0;
    LARGE_INTEGER freq, counter;
    if (qpf == 0.0) {
        QueryPerformanceFrequency(&freq);
        qpf = (double)freq.QuadPart;
    }
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart * 1000.0 / qpf;
}

#else

static void test_timeout_handler(int sig) {
    (void)sig;
    g_timeout_triggered = true;
    longjmp(g_timeout_jmp, 1);
}

static inline void test_setup_timeout(int seconds) {
    struct sigaction sa = {0};
    sa.sa_handler = test_timeout_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);
    alarm(seconds);
}

static inline void test_cancel_timeout(void) {
    alarm(0);
    signal(SIGALRM, SIG_DFL);
}

static inline double test_get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

#endif

#endif // XTEST_H
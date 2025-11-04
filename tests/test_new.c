#include "Xtest.h"

// ========================================
// Example Test Cases
// ========================================
typedef struct {
    const char *desc;
    int a;
    int b;
    int expected;
} MathTest;

static void run_math_test(MathTest *tc) {
    printf("  Testing: %d + %d = %d\n", tc->a, tc->b, tc->expected);
    int result = tc->a + tc->b;
    ASSERT_EQUAL(result, tc->expected);
}

TEST_CASES(math_tests, MathTest) {
    {"Addition: zero plus zero", 0, 0, 0},
    {"Addition: positive numbers", 5, 3, 8},
    {"Addition: negative numbers", -5, -3, -8},
    {"Addition: mixed signs", 10, -3, 7},
    {"Addition: large numbers", 1000, 2000, 3000},
    {"Subtraction test", 10, 5, 15}, // This will fail!
};

// ========================================
// String Test Cases  
// ========================================
typedef struct {
    const char *desc;
    const char *str1;
    const char *str2;
} StringTest;

static void run_string_test(StringTest *tc) {
    printf("  Comparing: \"%s\" vs \"%s\"\n", tc->str1, tc->str2);
    ASSERT_STR_EQUAL(tc->str1, tc->str2);
}

TEST_CASES(string_tests, StringTest) {
    {"String: empty strings", "", ""},
    {"String: same word", "hello", "hello"},
    {"String: same sentence", "hello world", "hello world"},
};

// ========================================
// Main with different configurations
// ========================================
int main(int argc, char **argv) {
    printf("=== Test Framework with Logging Demo ===\n\n");
    
    // ========================================
    // Example 1: Default output with file logging
    // ========================================
    printf("--- Example 1: Default output + File logging ---\n");
    TEST_CONFIG_TIMEOUT(5);
    TEST_CONFIG_BENCHMARK(true);
    TEST_CONFIG_LOG_LEVEL(LOG_DEBUG);
    TEST_CONFIG_LOG_FILE("test_run.log");
    
    RUN_TEST_CASES(math_tests, MathTest, run_math_test);
    RUN_TEST_CASES(string_tests, StringTest, run_string_test);
    
    TEST_REPORT();
    
    // ========================================
    // Example 2: TAP output
    // ========================================
    printf("\n\n--- Example 2: TAP Format Output ---\n");
    
    // Reset stats
    test_reset_stats();
    
    TEST_CONFIG_OUTPUT(OUTPUT_TAP);
    TEST_CONFIG_LOG_FILE(NULL); // Back to stdout
    
    RUN_TEST_CASES(math_tests, MathTest, run_math_test);
    
    TEST_REPORT();
    
    // ========================================
    // Example 3: JSON output
    // ========================================
    printf("\n\n--- Example 3: JSON Format Output ---\n");
    
    // Reset stats
    test_reset_stats();
    
    TEST_CONFIG_OUTPUT(OUTPUT_JSON);
    
    RUN_TEST_CASES(string_tests, StringTest, run_string_test);
    
    TEST_REPORT();
    
    // ========================================
    // Example 4: JUnit XML output
    // ========================================
    printf("\n\n--- Example 4: JUnit XML Format Output ---\n");
    
    // Reset stats
    test_reset_stats();
    
    TEST_CONFIG_OUTPUT(OUTPUT_JUNIT);
    
    RUN_TEST_CASES(math_tests, MathTest, run_math_test);
    
    TEST_REPORT();
    
    // ========================================
    // Example 5: Test filtering
    // ========================================
    printf("\n\n--- Example 5: Filtered Tests (only 'Addition') ---\n");
    
    // Reset stats
    test_reset_stats();
    
    TEST_CONFIG_OUTPUT(OUTPUT_DEFAULT);
    TEST_CONFIG_FILTER("Addition"); // Only run tests with "Addition" in name
    
    RUN_TEST_CASES(math_tests, MathTest, run_math_test);
    
    TEST_REPORT();
    
    printf("\n\n=== All Examples Complete ===\n");
    printf("Check 'test_run.log' for detailed logs from Example 1\n");
    
    (void)argc;
    (void)argv;
 
    
    return 0;
}
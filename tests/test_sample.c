#include "Xtest2.h"

// ========================================
// Example 1: Basic Tests
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

TEST_CASES(math_basic, MathTest) {
    {"Zero plus zero", 0, 0, 0},
    {"Positive numbers", 5, 3, 8},
    {"Negative numbers", -5, -3, -8},
    {"Mixed signs", 10, -3, 7},
    {"Large numbers", 1000, 2000, 3000},
};

// ========================================
// Example 2: String Tests
// ========================================
typedef struct {
    const char *desc;
    const char *input;
    const char *expected;
} StringTest;

static void run_string_test(StringTest *tc) {
    printf("  Testing: \"%s\"\n", tc->input);
    ASSERT_STR_EQUAL(tc->input, tc->expected);
}

TEST_CASES(string_tests, StringTest) {
    {"Empty string", "", ""},
    {"Single char", "a", "a"},
    {"Word", "hello", "hello"},
    {"Sentence", "hello world", "hello world"},
};

// ========================================
// Example 3: Pointer Tests
// ========================================
typedef struct {
    const char *desc;
    int value;
    bool should_be_null;
} PointerTest;

static void run_pointer_test(PointerTest *tc) {
    int *ptr = tc->should_be_null ? NULL : &tc->value;
    
    if (tc->should_be_null) {
        ASSERT_NULL(ptr);
    } else {
        ASSERT_NOT_NULL(ptr);
        ASSERT_EQUAL(*ptr, tc->value);
    }
}

TEST_CASES(pointer_tests, PointerTest) {
    {"Null pointer", 0, true},
    {"Valid pointer with 42", 42, false},
    {"Valid pointer with 0", 0, false},
    {"Valid pointer with negative", -10, false},
};

// ========================================
// Example 4: Parametrized Tests
// ========================================
typedef int IntParam;

static void run_power_of_two_test(IntParam *value) {
    printf("Value: %d ", *value);
    ASSERT_TRUE(*value > 0);
    // Check if power of 2
    ASSERT_TRUE((*value & (*value - 1)) == 0);
}

TEST_PARAM_CASES(powers_of_two, IntParam, IntParam) {
    {"Powers of 2", {1, 2, 4, 8, 16, 32, 64}, 7},
};

// ========================================
// Example 5: Repeat Tests
// ========================================
typedef struct {
    int multiplier;
} RepeatData;

static void run_stable_operation(RepeatData *data) {
    int result = data->multiplier * 10;
    ASSERT_EQUAL(result, data->multiplier * 10);
}

TEST_REPEAT_CASES(stability, RepeatData) {
    {"Stable multiplication by 2", 5, {2}},
    {"Stable multiplication by 10", 3, {10}},
};

// ========================================
// Example 6: Complex Test with Multiple Assertions
// ========================================
typedef struct {
    const char *desc;
    int array[5];
    size_t size;
    int sum_expected;
} ArrayTest;

static void run_array_test(ArrayTest *tc) {
    printf("  Testing array of size %u\n", tc->size);
    
    int sum = 0;
    for (size_t i = 0; i < tc->size; i++) {
        sum += tc->array[i];
    }
    
    ASSERT_EQUAL(sum, tc->sum_expected);
    ASSERT_TRUE(tc->size > 0);
    ASSERT_TRUE(tc->size <= 5);
}

TEST_CASES(array_tests, ArrayTest) {
    {"Single element", {5, 0, 0, 0, 0}, 1, 5},
    {"Two elements", {3, 7, 0, 0, 0}, 2, 10},
    {"All elements", {1, 2, 3, 4, 5}, 5, 15},
    {"With negatives", {10, -5, 3, -2, 0}, 5, 6},
};

// ========================================
// Example 7: Boolean Tests
// ========================================
typedef struct {
    const char *desc;
    bool condition;
    bool expected;
} BoolTest;

static void run_bool_test(BoolTest *tc) {
    if (tc->expected) {
        ASSERT_TRUE(tc->condition);
    } else {
        ASSERT_FALSE(tc->condition);
    }
}

TEST_CASES(bool_tests, BoolTest) {
    {"True is true", true, true},
    {"False is false", false, false},
    {"1 > 0 is true", (1 > 0), true},
    {"0 > 1 is false", (0 > 1), false},
};

// ========================================
// Main Runner
// ========================================
int main(void) {
    printf("=== Comprehensive Test Suite ===\n");
    printf("Testing all data-driven patterns\n");
    
    // Configure
    TEST_CONFIG_TIMEOUT(5);
    TEST_CONFIG_BENCHMARK(true);
    
    // Run all test suites
    RUN_TEST_CASES(math_basic, MathTest, run_math_test);
    RUN_TEST_CASES(string_tests, StringTest, run_string_test);
    RUN_TEST_CASES(pointer_tests, PointerTest, run_pointer_test);
    RUN_TEST_CASES(array_tests, ArrayTest, run_array_test);
    RUN_TEST_CASES(bool_tests, BoolTest, run_bool_test);
    
    // Parametrized tests
    RUN_PARAM_CASES(powers_of_two, IntParam, run_power_of_two_test);
    
    // Repeat tests
    RUN_REPEAT_CASES(stability, RepeatData, run_stable_operation);
    
    return TEST_REPORT();
}
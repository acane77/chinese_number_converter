#ifndef MOCK_GTEST_H
#define MOCK_GTEST_H

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

#define TEST(test_case_name, test_name) \
    void test_case_name##_##test_name(); \
    int main_##test_case_name##_##test_name = (test_case_name##_##test_name(), 0); \
    void test_case_name##_##test_name()

#define ASSERT_EQ(val1, val2) \
    do { \
        auto v1 = (val1); \
        auto v2 = (val2); \
        if (v1 != v2) { \
            std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::cerr << "Expected: " << v1 << std::endl; \
            std::cerr << "Actual:   " << v2 << std::endl; \
            std::exit(1); \
        } \
    } while (0)

#define ASSERT_STREQ(val1, val2) \
    do { \
        const char* v1 = (val1); \
        const char* v2 = (val2); \
        if (strcmp(v1, v2) != 0) { \
            std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::cerr << "Expected: " << v1 << std::endl; \
            std::cerr << "Actual:   " << v2 << std::endl; \
            std::exit(1); \
        } \
    } while (0)

// Dummy main removed, provided by test file


#include <vector>
#include <functional>

struct TestRegistry {
    static std::vector<std::function<void()>>& get_tests() {
        static std::vector<std::function<void()>> tests;
        return tests;
    }
    static void add_test(std::function<void()> test) {
        get_tests().push_back(test);
    }
    static void run_all() {
        for (auto& t : get_tests()) {
            t();
        }
        std::cout << "All tests passed!" << std::endl;
    }
};

#undef TEST
#define TEST(TestCase, TestName) \
    void TestCase##_##TestName(); \
    struct TestCase##_##TestName##_Register { \
        TestCase##_##TestName##_Register() { \
            TestRegistry::add_test(TestCase##_##TestName); \
        } \
    } TestCase##_##TestName##_register_instance; \
    void TestCase##_##TestName()

// Redefine main to run tests
// But `test_cnh_conv.cpp` might be compiled with this header.
// If I define `main` here, it will be in the test executable.
#define RUN_ALL_TESTS() TestRegistry::run_all()


#endif

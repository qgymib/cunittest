/**
 * @file
 */
/**
 * @mainpage CUnitTest
 * CUnitTest is a test framework for C. It's was inspired by GoogleTest originally.
 *
 * CUnitTest has following features:
 * + C89 / C99 / C11 compatible.
 * + GCC / Clang / MSVC compatible.
 * + x86 / x86_64 / arm / arm64 compatible.
 * + No dynamic memory alloc at runtime.
 * + Tests are automatically registered when declared.
 * + Support parameterized tests.
 */
#ifndef __C_UNIT_TEST_H__
#define __C_UNIT_TEST_H__

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Major version.
 */
#define CUTEST_VERSION_MAJOR        1

/**
 * @brief Minor version.
 */
#define CUTEST_VERSION_MINOR        0

/**
 * @brief Patch version.
 */
#define CUTEST_VERSION_PATCH        7

/**
 * @brief Development version.
 */
#define CUTEST_VERSION_PREREL       3

/**
 * @brief Ensure the api is exposed as C function.
 */
#if defined(__cplusplus)
#define TEST_C_API  extern "C"
#else
#define TEST_C_API
#endif

/**
 * @example main.c
 * A example for how to call #cutest_run_tests().
 */
/**
 * @example test_p.c
 * A example for parameterized test #TEST_P().
 */

/**
 * @defgroup TEST_DEFINE Define Test
 *
 * There are three ways to define a test:
 * + by #TEST().
 * + by #TEST_F().
 * + by #TEST_P().
 *
 * #TEST() define a simple test unit, which should be self contained.
 * 
 * ```c
 * TEST(foo, self) {\
 *     ASSERT_EQ_D32(0, 0);
 * }
 * ```
 *
 * Both #TEST_F() and #TEST_P() define a set of shared test unit, which share
 * the same setup and teardown procedure defined by #TEST_FIXTURE_SETUP() and
 * #TEST_FIXTURE_TEAREDOWN().
 *
 * ```c
 * TEST_FIXTURE_SETUP(foo) {
 *     printf("setup of foo.\n");
 * }
 * TEST_FIXTURE_TEAREDOWN(foo) {
 *     printf("teardown of foo.\n");
 * }
 * 
 * TEST_F(foo, normal) {
 *     ASSERT_NE_D32(0, 0);
 * }
 *
 * TEST_PARAMETERIZED_DEFINE(foo, param, int, 0, 1, 2);
 * TEST_P(foo, param) {
 *     printf("param:%d\n", TEST_GET_PARAM());
 * }
 * ```
 *
 * The #TEST_P() define a parameterized test, which require #TEST_PARAMETERIZED_DEFINE() define a set of parameterized data.
 * 
 * @{
 */

/**
 * @brief Setup test fixture
 * @param [in] fixture  The name of fixture
 */
#define TEST_FIXTURE_SETUP(fixture)    \
    TEST_C_API static void s_cutest_fixture_setup_##fixture(void)

/**
 * @brief TearDown test suit
 * @param [in] fixture  The name of fixture
 */
#define TEST_FIXTURE_TEAREDOWN(fixture)    \
    TEST_C_API static void s_cutest_fixture_teardown_##fixture(void)

/**
 * @brief Get parameterized data
 * @snippet test_p.c GET_PARAMETERIZED_DATA
 * @see TEST_PARAMETERIZED_DEFINE
 * @see TEST_P
 * @return  The data you defined
 */
#define TEST_GET_PARAM()    \
    (_test_parameterized_data[_test_parameterized_idx])

/**
 * @brief Define parameterized data for fixture
 * @snippet test_p.c ADD_SIMPLE_PARAMETERIZED_DEFINE
 * @snippet test_p.c ADD_COMPLEX_PARAMETERIZED_DEFINE
 * @param[in] fixture   Which fixture you want to parameterized
 * @param[in] test      Which test you want to parameterized
 * @param[in] TYPE      Data type
 * @param[in] ...       Data values
 */
#define TEST_PARAMETERIZED_DEFINE(fixture, test, TYPE, ...)  \
    TEST_C_API static void cutest_usertest_parameterized_register_##fixture##_##test(void (*cb)(TYPE*, unsigned long)) {\
        static TYPE s_parameterized_userdata[] = { __VA_ARGS__ };\
        static cutest_case_t s_tests[TEST_ARG_COUNT(__VA_ARGS__)];\
        unsigned long number_of_parameterized_data = sizeof(s_parameterized_userdata) / sizeof(s_parameterized_userdata[0]);\
        unsigned long i = 0;\
        for (i = 0; i < number_of_parameterized_data; i++) {\
            s_tests[i].node = (cutest_map_node_t){ NULL, NULL, NULL };\
            s_tests[i].info.fixture_name = #fixture;\
            s_tests[i].info.case_name = #test;\
            s_tests[i].stage.setup = s_cutest_fixture_setup_##fixture;\
            s_tests[i].stage.teardown = s_cutest_fixture_teardown_##fixture;\
            s_tests[i].stage.body = (void(*)(void*, unsigned long))cb;\
            s_tests[i].data.mask = 0;\
            s_tests[i].data.randkey = 0;\
            s_tests[i].parameterized.type_name = #TYPE;\
            s_tests[i].parameterized.test_data_cstr = TEST_STRINGIFY(__VA_ARGS__);\
            s_tests[i].parameterized.param_data = s_parameterized_userdata;\
            s_tests[i].parameterized.param_idx = i;\
            cutest_register_case(&s_tests[i]);\
        }\
    }\
    typedef TYPE u_cutest_parameterized_type_##fixture##_##test\

/**
 * @brief Suppress unused parameter warning if #TEST_GET_PARAM() is not used.
 * @snippet test_p.c SUPPRESS_UNUSED_PARAMETERIZED_DATA
 */
#define TEST_PARAMETERIZED_SUPPRESS_UNUSED  \
    (void)_test_parameterized_data; (void)_test_parameterized_idx

/**
 * @brief Parameterized Test
 *
 * A parameterized test will run many cycles, which was defined by
 * #TEST_PARAMETERIZED_DEFINE().
 *
 * You can get the parameter by #TEST_GET_PARAM(). Each cycle the #TEST_GET_PARAM()
 * will return the matching data defined in #TEST_PARAMETERIZED_DEFINE()
 *
 * @note If you declare a Parameterized Test but do not want to use #TEST_GET_PARAM(),
 *   you may get a compile time warning like `unused parameter _test_parameterized_data`.
 *   To suppress this warning, just place #TEST_PARAMETERIZED_SUPPRESS_UNUSED
 *   in the begin of your test body.
 *
 * @param [in] fixture  The name of fixture
 * @param [in] test     The name of test case
 * @see TEST_GET_PARAM()
 * @see TEST_PARAMETERIZED_DEFINE()
 * @see TEST_PARAMETERIZED_SUPPRESS_UNUSED
 * @snippet test_p.c
 */
#define TEST_P(fixture, test) \
    TEST_C_API void u_cutest_body_##fixture##_##test(\
        u_cutest_parameterized_type_##fixture##_##test*, unsigned long);\
    TEST_INITIALIZER(cutest_usertest_interface_##fixture##_##test) {\
        static unsigned char s_token = 0;\
        if (s_token == 0) {\
            s_token = 1;\
            cutest_usertest_parameterized_register_##fixture##_##test(u_cutest_body_##fixture##_##test);\
        }\
    }\
    TEST_C_API void u_cutest_body_##fixture##_##test(\
        u_cutest_parameterized_type_##fixture##_##test* _test_parameterized_data,\
        unsigned long _test_parameterized_idx)

/**
 * @brief Test Fixture
 * @param [in] fixture  The name of fixture
 * @param [in] test     The name of test case
 */
#define TEST_F(fixture, test) \
    TEST_C_API void cutest_usertest_body_##fixture##_##test(void);\
    TEST_C_API static void s_cutest_proxy_##fixture##_##test(void* _test_parameterized_data,\
        unsigned long _test_parameterized_idx) {\
        TEST_PARAMETERIZED_SUPPRESS_UNUSED;\
        cutest_usertest_body_##fixture##_##test();\
    }\
    TEST_INITIALIZER(cutest_usertest_interface_##fixture##_##test) {\
        static cutest_case_t _case_##fixture##_##test = {\
            {\
                NULL, NULL, NULL,\
            }, /* .node */\
            {\
                #fixture,\
                #test,\
            }, /* .info */\
            {\
                s_cutest_fixture_setup_##fixture,\
                s_cutest_fixture_teardown_##fixture,\
                s_cutest_proxy_##fixture##_##test,\
            }, /* .stage */\
            {\
                0, 0,\
            }, /* .data */\
            {\
                NULL, NULL, NULL, 0,\
            }, /* .parameterized */\
        };\
        static unsigned char s_token = 0;\
        if (s_token == 0) {\
            s_token = 1;\
            cutest_register_case(&_case_##fixture##_##test);\
        }\
    }\
    TEST_C_API void cutest_usertest_body_##fixture##_##test(void)

/**
 * @brief Simple Test
 * @param [in] fixture  suit name
 * @param [in] test     case name
 */
#define TEST(fixture, test)  \
    TEST_C_API void cutest_usertest_body_##fixture##_##test(void);\
    TEST_C_API static void s_cutest_proxy_##fixture##_##test(void* _test_parameterized_data,\
        unsigned long _test_parameterized_idx) {\
        TEST_PARAMETERIZED_SUPPRESS_UNUSED;\
        cutest_usertest_body_##fixture##_##test();\
    }\
    TEST_INITIALIZER(cutest_usertest_interface_##fixture##_##test) {\
        static cutest_case_t _case_##fixture##_##test = {\
            {\
                NULL, NULL, NULL,\
            }, /* .node */\
            {\
                #fixture,\
                #test,\
            }, /* .info */\
            {\
                NULL, NULL,\
                s_cutest_proxy_##fixture##_##test,\
            }, /* .stage */\
            {\
                0, 0,\
            }, /* .data */\
            {\
                NULL, NULL, NULL, 0,\
            }, /* parameterized */\
        };\
        static unsigned char s_token = 0;\
        if (s_token == 0) {\
            s_token = 1;\
            cutest_register_case(&_case_##fixture##_##test);\
        }\
    }\
    TEST_C_API void cutest_usertest_body_##fixture##_##test(void)

/** @cond */

/**
 * @def TEST_INITIALIZER(f)
 * @brief Run the following code before main() invoke.
 */
#ifdef __cplusplus
#   define TEST_INITIALIZER(f) \
        TEST_C_API void f(void); \
        struct f##_t_ { f##_t_(void) { f(); } }; f##_t_ f##_; \
        TEST_C_API void f(void)
#elif defined(_MSC_VER)
#   pragma section(".CRT$XCU",read)
#   define TEST_INITIALIZER2_(f,p) \
        TEST_C_API void f(void); \
        __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
        __pragma(comment(linker,"/include:" p #f "_")) \
        TEST_C_API void f(void)
#   ifdef _WIN64
#       define TEST_INITIALIZER(f) TEST_INITIALIZER2_(f,"")
#   else
#       define TEST_INITIALIZER(f) TEST_INITIALIZER2_(f,"_")
#   endif
#elif defined(__GNUC__) || defined(__clang__) || defined(__llvm__)
#   define TEST_INITIALIZER(f) \
        TEST_C_API void f(void) __attribute__((constructor)); \
        TEST_C_API void f(void)
#else
#   define TEST_INITIALIZER(f) \
        TEST_C_API void f(void)
#   define TEST_NEED_MANUAL_REGISTRATION    1
#endif

/**
 * @def TEST_ARG_COUNT
 * @brief Get the number of arguments
 */
#ifdef _MSC_VER // Microsoft compilers
#   define TEST_ARG_COUNT(...)  \
        TEST_INTERNAL_EXPAND_ARGS_PRIVATE(TEST_INTERNAL_ARGS_AUGMENTER(__VA_ARGS__))
#   define TEST_INTERNAL_ARGS_AUGMENTER(...)    \
        unused, __VA_ARGS__
#   define TEST_INTERNAL_EXPAND_ARGS_PRIVATE(...)   \
        TEST_EXPAND(TEST_INTERNAL_GET_ARG_COUNT_PRIVATE(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#   define TEST_INTERNAL_GET_ARG_COUNT_PRIVATE(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, count, ...) count
#else // Non-Microsoft compilers
#   define TEST_ARG_COUNT(...)  \
        TEST_INTERNAL_GET_ARG_COUNT_PRIVATE(0, ## __VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#   define TEST_INTERNAL_GET_ARG_COUNT_PRIVATE(_0, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, count, ...) count
#endif

#define TEST_STRINGIFY(...)     TEST_STRINGIFY_2(__VA_ARGS__)
#define TEST_STRINGIFY_2(...)   #__VA_ARGS__

#define TEST_EXPAND(x)          x
#define TEST_JOIN(a, b)         TEST_JOIN2(a, b)
#define TEST_JOIN2(a, b)        a##b

typedef struct cutest_map_node
{
    struct cutest_map_node*     __rb_parent_color;      /**< father node | color */
    struct cutest_map_node*     rb_right;               /**< right child node */
    struct cutest_map_node*     rb_left;                /**< left child node */
} cutest_map_node_t;

typedef struct cutest_case
{
    cutest_map_node_t           node;

    struct
    {
        const char*             fixture_name;                   /**< suit name */
        const char*             case_name;                      /**< case name */
    } info;

    struct
    {
        void                    (*setup)(void);                 /**< setup */
        void                    (*teardown)(void);              /**< teardown */
        void                    (*body)(void*, unsigned long);  /**< test body */
    } stage;

    struct
    {
        unsigned long           mask;                           /**< Internal mask */
        unsigned long           randkey;                        /**< Random key */
    } data;

    struct
    {
        const char*             type_name;                      /**< User type name. */
        const char*             test_data_cstr;                 /**< The C string of user test data. */
        void*                   param_data;                     /**< Data passed to #cutest_case_t::stage::body */
        unsigned long           param_idx;                      /**< Index passed to #cutest_case_t::stage::body */
    } parameterized;
} cutest_case_t;

/**
 * @brief Register test case
 * @param[in] test_case     Test case
 */
void cutest_register_case(cutest_case_t* test_case);

/** @endcond */

/**
 * Group: TEST_DEFINE
 * @}
 */

/**
 * @defgroup TEST_MANUAL_REGISTRATION Manually registr test
 *
 * By default all test cases are registered automatically. If not, please check
 * whether the value of #TEST_NEED_MANUAL_REGISTRATION is 1. If so, it means
 * that your compiler does not support run codes before `main()`.
 *
 * To make things works, you need to manually register your tests.
 *
 * Assume you have following tests defined:
 *
 * ```c
 * TEST_FIXTURE_SETUP(manual_registeration){}
 * TEST_FIXTURE_TEAREDOWN(manual_registeration){}
 * TEST_PARAMETERIZED_DEFINE(manual_registeration, third, int, 0);
 *
 * TEST(manual_registeration, first){}
 * TEST_F(manual_registeration, second){}
 * TEST_P(manual_registeration, third){ TEST_PARAMETERIZED_SUPPRESS_UNUSED; }
 * ```
 *
 * To manually register your tests:
 *
 * 1. Defines all your tests in macro.
 *   ```c
 *   #define MY_TEST_TABLE(xx) \
 *       xx(manual_registeration, first) \
 *       xx(manual_registeration, second) \
 *       xx(manual_registeration, third)
 *   ```
 *
 * 2. Declare all tests interface on top of your file.
 *   ```c
 *   MY_TEST_TABLE(TEST_MANUAL_DECLARE_TEST_INTERFACE)
 *   ```
 *
 * 3. Register all tests.
 *   ```c
 *   MY_TEST_TABLE(TEST_MANUAL_REGISTER_TEST_INTERFACE)
 *   ```
 *
 * @note Manual registeration can work well with automatical registeration.
 *
 * @{
 */

/**
 * @brief Whether manual registration is needed.
 *
 * If the value is 0, tests will be automatically registered. If not, you need
 * to register your tests manually.
 */
#if !defined(TEST_NEED_MANUAL_REGISTRATION)
#   define TEST_NEED_MANUAL_REGISTRATION    0
#endif

/**
 * @brief Declare test interface.
 * @param[in] fixture   Fixture name.
 * @param[in] test      Test name.
 */
#define TEST_MANUAL_DECLARE_TEST_INTERFACE(fixture, test) \
    TEST_C_API void cutest_usertest_interface_##fixture##_##test();

/**
 * @brief Call test interface.
 * @param[in] fixture   Fixture name.
 * @param[in] test      Test name.
 */
#define TEST_MANUAL_REGISTER_TEST_INTERFACE(fixture, test) \
    cutest_usertest_interface_##fixture##_##test();

/**
 * Group: TEST_MANUAL_REGISTRATION
 * @}
 */

/**
 * @defgroup TEST_ASSERTION Assertion
 * 
 * [cutest](https://github.com/qgymib/cutest/) support rich set of assertion. An
 * assertion typically have following syntax:
 * 
 * ```c
 * ASSERT_OP_TYPE(a, b)
 * ASSERT_OP_TYPE(a, b, fmt, ...)
 * ```
 * 
 * The `OP` means which compare operation you want to use:
 * + EQ: `a` is equal to `b`.
 * + NE: `a` is not equal to `b`.
 * + LT: `a` is less than `b`.
 * + LE: `a` is equal to `b` or less than `b`.
 * + GT: `a` is greater than `b`.
 * + GE: `a` is equal to `b` or greater than `b`.
 *
 * The `TYPE` means the type of value `a` and `b`.
 * + D32: int32_t
 * + U32: uint32_t
 * + D64: int64_t
 * + U64: uint64_t
 * + STR: const char*
 * + PTR: const void*
 * + SIZE: size_t
 * + FLOAT: float
 * + DOUBLE: double
 *
 * > To support more types, checkout #TEST_REGISTER_TYPE_ONCE().
 *
 * So, an assertion like #ASSERT_EQ_D32() means except `a` and `b` have type of
 * `int32_t` and they are the same value.
 *
 * You may notice all assertions have syntax of `ASSERT_OP_TYPE(a, b, fmt, ...)`,
 * it means custom print is available if assertion fails. For example, the
 * following code
 * 
 * ```c
 * int errcode = ENOENT;
 * ASSERT_EQ_D32(0, errcode, "%s(%d)", strerror(errcode), errcode);
 * ```
 *
 * Will print something like:
 *
 * ```
 * No such file or directory(2)
 * ```
 *
 * You may also want to refer to the actual value of operator, you can use `_L`
 * to refer to left operator and `_R` to refer to right operator:
 * 
 * ```c
 * ASSERT_EQ_D32(0, 1+2, "%d is not %d", _L, _R);
 * ```
 *
 * The output will be something like:
 *
 * ```
 * 0 is not 3
 * ```
 *
 * @{
 */

/**
 * @defgroup TEST_ASSERTION_C89 C89 Assertion
 *
 * Assertion macros for C89 standard, provide native type support:
 * + char
 * + unsigned char
 * + signed char
 * + short
 * + unsigned short
 * + int
 * + unsigned int
 * + long
 * + unsigned long
 * + float
 * + double
 * + const void*
 * + const char*
 *
 * @note About `size_t` and `ptrdiff_t`: Although they are included in C89
 *   standard, the formal print conversion specifier `%%zu` and `%%td` are
 *   inclueded in C99 standard. Besides they need `<stddef.h>`, and we want
 *   the dependency to be minimum.
 *
 * @note About `long double`:  Although it exists in C89 standard, we do not
 * offer support for type `long double`, because actual properties unspecified.
 * The implementation can be either x86 extended-precision floating-point format
 * (80 bits, but typically 96 bits or 128 bits in memory with padding bytes),
 * the non-IEEE "double-double" (128 bits), IEEE 754 quadruple-precision
 * floating-point format (128 bits), or the same as double.
 *
 * @see http://port70.net/~nsz/c/c89/c89-draft.html
 *
 * @{
 */

/**
 * @defgroup TEST_ASSERTION_C89_CHAR char
 * @{
 */
#define ASSERT_EQ_CHAR(a, b, ...)       ASSERT_TEMPLATE_EXT(char, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_CHAR(a, b, ...)       ASSERT_TEMPLATE_EXT(char, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_CHAR(a, b, ...)       ASSERT_TEMPLATE_EXT(char, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_CHAR(a, b, ...)       ASSERT_TEMPLATE_EXT(char, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_CHAR(a, b, ...)       ASSERT_TEMPLATE_EXT(char, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_CHAR(a, b, ...)       ASSERT_TEMPLATE_EXT(char, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_DCHAR signed char
 * @{
 */
#define ASSERT_EQ_DCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(signed char, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_DCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(signed char, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_DCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(signed char, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_DCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(signed char, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_DCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(signed char, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_DCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(signed char, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_UCHAR unsigned char
 * @{
 */
#define ASSERT_EQ_UCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned char, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_UCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned char, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_UCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned char, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_UCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned char, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_UCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned char, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_UCHAR(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned char, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_SHORT short
 * @{
 */
#define ASSERT_EQ_SHORT(a, b, ...)      ASSERT_TEMPLATE_EXT(short, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_SHORT(a, b, ...)      ASSERT_TEMPLATE_EXT(short, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_SHORT(a, b, ...)      ASSERT_TEMPLATE_EXT(short, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_SHORT(a, b, ...)      ASSERT_TEMPLATE_EXT(short, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_SHORT(a, b, ...)      ASSERT_TEMPLATE_EXT(short, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_SHORT(a, b, ...)      ASSERT_TEMPLATE_EXT(short, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_USHORT unsigned short
 * @{
 */
#define ASSERT_EQ_USHORT(a, b, ...)     ASSERT_TEMPLATE_EXT(unsigned short, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_USHORT(a, b, ...)     ASSERT_TEMPLATE_EXT(unsigned short, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_USHORT(a, b, ...)     ASSERT_TEMPLATE_EXT(unsigned short, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_USHORT(a, b, ...)     ASSERT_TEMPLATE_EXT(unsigned short, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_USHORT(a, b, ...)     ASSERT_TEMPLATE_EXT(unsigned short, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_USHORT(a, b, ...)     ASSERT_TEMPLATE_EXT(unsigned short, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_INT int
 * @{
 */
#define ASSERT_EQ_INT(a, b, ...)        ASSERT_TEMPLATE_EXT(int, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_INT(a, b, ...)        ASSERT_TEMPLATE_EXT(int, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_INT(a, b, ...)        ASSERT_TEMPLATE_EXT(int, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_INT(a, b, ...)        ASSERT_TEMPLATE_EXT(int, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_INT(a, b, ...)        ASSERT_TEMPLATE_EXT(int, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_INT(a, b, ...)        ASSERT_TEMPLATE_EXT(int, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_UINT unsigned int
 * @{
 */
#define ASSERT_EQ_UINT(a, b, ...)       ASSERT_TEMPLATE_EXT(unsigned int, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_UINT(a, b, ...)       ASSERT_TEMPLATE_EXT(unsigned int, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_UINT(a, b, ...)       ASSERT_TEMPLATE_EXT(unsigned int, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_UINT(a, b, ...)       ASSERT_TEMPLATE_EXT(unsigned int, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_UINT(a, b, ...)       ASSERT_TEMPLATE_EXT(unsigned int, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_UINT(a, b, ...)       ASSERT_TEMPLATE_EXT(unsigned int, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_LONG long
 * @{
 */
#define ASSERT_EQ_LONG(a, b, ...)       ASSERT_TEMPLATE_EXT(long, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_LONG(a, b, ...)       ASSERT_TEMPLATE_EXT(long, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_LONG(a, b, ...)       ASSERT_TEMPLATE_EXT(long, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_LONG(a, b, ...)       ASSERT_TEMPLATE_EXT(long, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_LONG(a, b, ...)       ASSERT_TEMPLATE_EXT(long, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_LONG(a, b, ...)       ASSERT_TEMPLATE_EXT(long, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_ULONG unsigned long
 * @{
 */
#define ASSERT_EQ_ULONG(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned long, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_ULONG(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned long, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_ULONG(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned long, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_ULONG(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned long, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_ULONG(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned long, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_ULONG(a, b, ...)      ASSERT_TEMPLATE_EXT(unsigned long, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_FLOAT float
 * @{
 */
#define ASSERT_EQ_FLOAT(a, b, ...)      ASSERT_TEMPLATE_EXT(float, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_FLOAT(a, b, ...)      ASSERT_TEMPLATE_EXT(float, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_FLOAT(a, b, ...)      ASSERT_TEMPLATE_EXT(float, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_FLOAT(a, b, ...)      ASSERT_TEMPLATE_EXT(float, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_FLOAT(a, b, ...)      ASSERT_TEMPLATE_EXT(float, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_FLOAT(a, b, ...)      ASSERT_TEMPLATE_EXT(float, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_DOUBLE double
 * @{
 */
#define ASSERT_EQ_DOUBLE(a, b, ...)     ASSERT_TEMPLATE_EXT(double, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_DOUBLE(a, b, ...)     ASSERT_TEMPLATE_EXT(double, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_DOUBLE(a, b, ...)     ASSERT_TEMPLATE_EXT(double, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_DOUBLE(a, b, ...)     ASSERT_TEMPLATE_EXT(double, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_DOUBLE(a, b, ...)     ASSERT_TEMPLATE_EXT(double, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_DOUBLE(a, b, ...)     ASSERT_TEMPLATE_EXT(double, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_PTR const void*
 * @{
 */
#define ASSERT_EQ_PTR(a, b, ...)        ASSERT_TEMPLATE_EXT(const void*, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_PTR(a, b, ...)        ASSERT_TEMPLATE_EXT(const void*, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_PTR(a, b, ...)        ASSERT_TEMPLATE_EXT(const void*, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_PTR(a, b, ...)        ASSERT_TEMPLATE_EXT(const void*, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_PTR(a, b, ...)        ASSERT_TEMPLATE_EXT(const void*, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_PTR(a, b, ...)        ASSERT_TEMPLATE_EXT(const void*, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C89_STR const char*
 * @{
 */
#define ASSERT_EQ_STR(a, b, ...)        ASSERT_TEMPLATE_EXT(const char*, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_STR(a, b, ...)        ASSERT_TEMPLATE_EXT(const char*, !=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99 C99 Assertion
 *
 * Assertion macros for C99 standard, provide support:
 * + long long
 * + unsigned long long
 * + int8_t
 * + uint8_t
 * + int16_t
 * + uint16_t
 * + int32_t
 * + uint32_t
 * + int64_t
 * + uint64_t
 * + size_t
 * + ptrdiff_t
 * + intptr_t
 * + uintptr_t
 *
 * @note To use these assertions, you need `<stdint.h>` and `<inttypes.h>` header
 *   files which are not included.
 *
 * @note These assertions are enabled by default. To disable all of them, add
 * `CUTEST_NO_C99_SUPPORT` (eg. `-DCUTEST_NO_C99_SUPPORT`) during compile cutest.
 *
 * @note These assertions can be disabled separatily:
 * | type               | flag                        |
 * | ------------------ | --------------------------- |
 * | long long          | CUTEST_NO_LONGLONG_SUPPORT  |
 * | unsigned long long | CUTEST_NO_ULONGLONG_SUPPORT |
 * | int8_t             | CUTEST_NO_INT8_SUPPORT      |
 * | uint8_t            | CUTEST_NO_UINT8_SUPPORT     |
 * | int16_t            | CUTEST_NO_INT16_SUPPORT     |
 * | uint16_t           | CUTEST_NO_UINT16_SUPPORT    |
 * | int32_t            | CUTEST_NO_INT32_SUPPORT     |
 * | uint32_t           | CUTEST_NO_UINT32_SUPPORT    |
 * | int64_t            | CUTEST_NO_INT64_SUPPORT     |
 * | uint64_t           | CUTEST_NO_UINT64_SUPPORT    |
 * | size_t             | CUTEST_NO_SIZE_SUPPORT      |
 * | ptrdiff_t          | CUTEST_NO_PTRDIFF_SUPPORT   |
 * | intptr_t           | CUTEST_NO_INTPTR_SUPPORT    |
 * | uintptr_t          | CUTEST_NO_UINTPTR_SUPPORT   |
 * 
 * @{
 */

/**
 * @defgroup TEST_ASSERTION_C99_LONGLONG long long
 * @{
 */
#define ASSERT_EQ_LONGLONG(a, b, ...)   ASSERT_TEMPLATE_EXT(long long, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_LONGLONG(a, b, ...)   ASSERT_TEMPLATE_EXT(long long, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_LONGLONG(a, b, ...)   ASSERT_TEMPLATE_EXT(long long, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_LONGLONG(a, b, ...)   ASSERT_TEMPLATE_EXT(long long, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_LONGLONG(a, b, ...)   ASSERT_TEMPLATE_EXT(long long, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_LONGLONG(a, b, ...)   ASSERT_TEMPLATE_EXT(long long, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_ULONGLONG unsigned long long
 * @{
 */
#define ASSERT_EQ_ULONGLONG(a, b, ...)  ASSERT_TEMPLATE_EXT(unsigned long long, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_ULONGLONG(a, b, ...)  ASSERT_TEMPLATE_EXT(unsigned long long, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_ULONGLONG(a, b, ...)  ASSERT_TEMPLATE_EXT(unsigned long long, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_ULONGLONG(a, b, ...)  ASSERT_TEMPLATE_EXT(unsigned long long, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_ULONGLONG(a, b, ...)  ASSERT_TEMPLATE_EXT(unsigned long long, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_ULONGLONG(a, b, ...)  ASSERT_TEMPLATE_EXT(unsigned long long, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_INT8 int8_t
 * @{
 */
#define ASSERT_EQ_INT8(a, b, ...)       ASSERT_TEMPLATE_EXT(int8_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_INT8(a, b, ...)       ASSERT_TEMPLATE_EXT(int8_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_INT8(a, b, ...)       ASSERT_TEMPLATE_EXT(int8_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_INT8(a, b, ...)       ASSERT_TEMPLATE_EXT(int8_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_INT8(a, b, ...)       ASSERT_TEMPLATE_EXT(int8_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_INT8(a, b, ...)       ASSERT_TEMPLATE_EXT(int8_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_UINT8 uint8_t
 * @{
 */
#define ASSERT_EQ_UINT8(a, b, ...)      ASSERT_TEMPLATE_EXT(uint8_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_UINT8(a, b, ...)      ASSERT_TEMPLATE_EXT(uint8_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_UINT8(a, b, ...)      ASSERT_TEMPLATE_EXT(uint8_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_UINT8(a, b, ...)      ASSERT_TEMPLATE_EXT(uint8_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_UINT8(a, b, ...)      ASSERT_TEMPLATE_EXT(uint8_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_UINT8(a, b, ...)      ASSERT_TEMPLATE_EXT(uint8_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_INT16 int16_t
 * @{
 */
#define ASSERT_EQ_INT16(a, b, ...)      ASSERT_TEMPLATE_EXT(int16_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_INT16(a, b, ...)      ASSERT_TEMPLATE_EXT(int16_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_INT16(a, b, ...)      ASSERT_TEMPLATE_EXT(int16_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_INT16(a, b, ...)      ASSERT_TEMPLATE_EXT(int16_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_INT16(a, b, ...)      ASSERT_TEMPLATE_EXT(int16_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_INT16(a, b, ...)      ASSERT_TEMPLATE_EXT(int16_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */


/**
 * @defgroup TEST_ASSERTION_C99_UINT16 uint16_t
 * @{
 */
#define ASSERT_EQ_UINT16(a, b, ...)     ASSERT_TEMPLATE_EXT(uint16_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_UINT16(a, b, ...)     ASSERT_TEMPLATE_EXT(uint16_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_UINT16(a, b, ...)     ASSERT_TEMPLATE_EXT(uint16_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_UINT16(a, b, ...)     ASSERT_TEMPLATE_EXT(uint16_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_UINT16(a, b, ...)     ASSERT_TEMPLATE_EXT(uint16_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_UINT16(a, b, ...)     ASSERT_TEMPLATE_EXT(uint16_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_INT32 int32_t
 * @{
 */
#define ASSERT_EQ_INT32(a, b, ...)      ASSERT_TEMPLATE_EXT(int32_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_INT32(a, b, ...)      ASSERT_TEMPLATE_EXT(int32_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_INT32(a, b, ...)      ASSERT_TEMPLATE_EXT(int32_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_INT32(a, b, ...)      ASSERT_TEMPLATE_EXT(int32_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_INT32(a, b, ...)      ASSERT_TEMPLATE_EXT(int32_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_INT32(a, b, ...)      ASSERT_TEMPLATE_EXT(int32_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_UINT32 uint32_t
 * @{
 */
#define ASSERT_EQ_UINT32(a, b, ...)     ASSERT_TEMPLATE_EXT(uint32_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_UINT32(a, b, ...)     ASSERT_TEMPLATE_EXT(uint32_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_UINT32(a, b, ...)     ASSERT_TEMPLATE_EXT(uint32_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_UINT32(a, b, ...)     ASSERT_TEMPLATE_EXT(uint32_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_UINT32(a, b, ...)     ASSERT_TEMPLATE_EXT(uint32_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_UINT32(a, b, ...)     ASSERT_TEMPLATE_EXT(uint32_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_INT64 int64_t
 * @{
 */
#define ASSERT_EQ_INT64(a, b, ...)      ASSERT_TEMPLATE_EXT(int64_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_INT64(a, b, ...)      ASSERT_TEMPLATE_EXT(int64_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_INT64(a, b, ...)      ASSERT_TEMPLATE_EXT(int64_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_INT64(a, b, ...)      ASSERT_TEMPLATE_EXT(int64_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_INT64(a, b, ...)      ASSERT_TEMPLATE_EXT(int64_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_INT64(a, b, ...)      ASSERT_TEMPLATE_EXT(int64_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_UINT64 uint64_t
 * @{
 */
#define ASSERT_EQ_UINT64(a, b, ...)     ASSERT_TEMPLATE_EXT(uint64_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_UINT64(a, b, ...)     ASSERT_TEMPLATE_EXT(uint64_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_UINT64(a, b, ...)     ASSERT_TEMPLATE_EXT(uint64_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_UINT64(a, b, ...)     ASSERT_TEMPLATE_EXT(uint64_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_UINT64(a, b, ...)     ASSERT_TEMPLATE_EXT(uint64_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_UINT64(a, b, ...)     ASSERT_TEMPLATE_EXT(uint64_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_SIZE size_t
 * @{
 */
#define ASSERT_EQ_SIZE(a, b, ...)       ASSERT_TEMPLATE_EXT(size_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_SIZE(a, b, ...)       ASSERT_TEMPLATE_EXT(size_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_SIZE(a, b, ...)       ASSERT_TEMPLATE_EXT(size_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_SIZE(a, b, ...)       ASSERT_TEMPLATE_EXT(size_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_SIZE(a, b, ...)       ASSERT_TEMPLATE_EXT(size_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_SIZE(a, b, ...)       ASSERT_TEMPLATE_EXT(size_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_PTRDIFF ptrdiff_t
 * @{
 */
#define ASSERT_EQ_PTRDIFF(a, b, ...)    ASSERT_TEMPLATE_EXT(ptrdiff_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_PTRDIFF(a, b, ...)    ASSERT_TEMPLATE_EXT(ptrdiff_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_PTRDIFF(a, b, ...)    ASSERT_TEMPLATE_EXT(ptrdiff_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_PTRDIFF(a, b, ...)    ASSERT_TEMPLATE_EXT(ptrdiff_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_PTRDIFF(a, b, ...)    ASSERT_TEMPLATE_EXT(ptrdiff_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_PTRDIFF(a, b, ...)    ASSERT_TEMPLATE_EXT(ptrdiff_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_INTPTR inttpr_t
 * @{
 */
#define ASSERT_EQ_INTPTR(a, b, ...)     ASSERT_TEMPLATE_EXT(intptr_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_INTPTR(a, b, ...)     ASSERT_TEMPLATE_EXT(intptr_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_INTPTR(a, b, ...)     ASSERT_TEMPLATE_EXT(intptr_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_INTPTR(a, b, ...)     ASSERT_TEMPLATE_EXT(intptr_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_INTPTR(a, b, ...)     ASSERT_TEMPLATE_EXT(intptr_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_INTPTR(a, b, ...)     ASSERT_TEMPLATE_EXT(intptr_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @defgroup TEST_ASSERTION_C99_UINTPTR uinttpr_t
 * @{
 */
#define ASSERT_EQ_UINTPTR(a, b, ...)    ASSERT_TEMPLATE_EXT(uintptr_t, ==, a, b, __VA_ARGS__)
#define ASSERT_NE_UINTPTR(a, b, ...)    ASSERT_TEMPLATE_EXT(uintptr_t, !=, a, b, __VA_ARGS__)
#define ASSERT_LT_UINTPTR(a, b, ...)    ASSERT_TEMPLATE_EXT(uintptr_t, <,  a, b, __VA_ARGS__)
#define ASSERT_LE_UINTPTR(a, b, ...)    ASSERT_TEMPLATE_EXT(uintptr_t, <=, a, b, __VA_ARGS__)
#define ASSERT_GT_UINTPTR(a, b, ...)    ASSERT_TEMPLATE_EXT(uintptr_t, >,  a, b, __VA_ARGS__)
#define ASSERT_GE_UINTPTR(a, b, ...)    ASSERT_TEMPLATE_EXT(uintptr_t, >=, a, b, __VA_ARGS__)
/**
 * @}
 */

/**
 * @}
 */

/**
 * Group: TEST_ASSERTION
 * @}
 */

/**
 * @defgroup TEST_CUSTOM_TYPE Custom type assertion support
 *
 * Even though cutest have rich set of assertion macros, there might be some
 * cases that need to compare custom type.
 *
 * We have a custom type register system to support such scene.
 *
 * Suppose we have a custom type: `typedef struct { int a; } foo_t`, to add
 * type support:
 *
 * + Register type information by #TEST_REGISTER_TYPE_ONCE()
 *
 *   ```c
 *   static int _on_cmp_foo(const foo_t* addr1, const foo_t* addr2) {
 *       return addr1->a - addr2->a;
 *   }
 *   static int _on_dump_foo(FILE* stream, const foo_t* addr) {
 *       return fprintf(stream, "{ a:%d }", addr->a);
 *   }
 *   int main(int argc, char* argv[]) {
 *       TEST_REGISTER_TYPE_ONCE(foo_t, _on_cmp_foo, _on_dump_foo);
 *       return cutest_run_tests(argc, argv, stdout, NULL);
 *   }
 *   ```
 *
 * + Define assertion macros
 *
 *   ```c
 *   #define ASSERT_EQ_FOO(a, b, ...)   ASSERT_TEMPLATE_EXT(foo_t, ==, a, b, __VA_ARGS__)
 *   #define ASSERT_NE_FOO(a, b, ...)   ASSERT_TEMPLATE_EXT(foo_t, !=, a, b, __VA_ARGS__)
 *   #define ASSERT_LT_FOO(a, b, ...)   ASSERT_TEMPLATE_EXT(foo_t, <,  a, b, __VA_ARGS__)
 *   #define ASSERT_LE_FOO(a, b, ...)   ASSERT_TEMPLATE_EXT(foo_t, <=, a, b, __VA_ARGS__)
 *   #define ASSERT_GT_FOO(a, b, ...)   ASSERT_TEMPLATE_EXT(foo_t, >,  a, b, __VA_ARGS__)
 *   #define ASSERT_GE_FOO(a, b, ...)   ASSERT_TEMPLATE_EXT(foo_t, >=, a, b, __VA_ARGS__)
 *   ```
 *
 * Now you can use `ASSERT_EQ_FOO()` / `ASSERT_NE_FOO()` / etc to do assertion.
 *
 * @{
 */

/**
 * @brief Declare and register custom type.
 *
 * This function does following things:
 * 1. Try best to check function prototype of \p cmp and \p dump. Note this depends on comiler so might not work.
 * 2. Generate information for \p TYPE.
 * 3. Ensure this register code only run once.
 *
 * For example:
 *
 * + If \p TYPE is `unsigned`, the protocol of \p cmp and \p dump should be:
 *   ```c
 *   int (*)(const unsigned* addr1, const unsigned* addr2) {\
 *       unsigned v1 = *addr1, v2 = *addr2;
 *       if (v1 == v2)
 *           return 0;
 *       return v1 < v2 ? -1 : 1;
 *   }
 *   int (*)(FILE* stream, const unsigned* addr) {
 *       fprintf(stream, "%u", *addr);
 *   }
 *   ```
 *
 * + If \p TYPE is `const char*`, the protocol of \p cmp and \p dump should be:
 *   ```c
 *   int (*)(const char** addr1, const char** addr2) {\
 *       return strcmp(*addr1, *addr2);
 *   }
 *   int (*)(FILE* stream, const char** addr) {
 *       fprintf(stream, "%s", *addr);
 *   }
 *   ```
 *
 * @note Although not restricted, it is recommend to register all custom type before run any test.
 * @param[in] TYPE  Data type.
 * @param[in] cmp   Compare function. It must have proto of `int (*)(const TYPE*, const TYPE*)`.
 * @param[in] dump  Dump function. It must have proto of `int (*)(FILE*, const TYPE*)`.
 */
#define TEST_REGISTER_TYPE_ONCE(TYPE, fn_cmp, fn_dump)    \
    do {\
        /* Try our best to check function protocol. */\
        int (*ckeck_type_cmp)(TYPE*,TYPE*) = fn_cmp; (void)ckeck_type_cmp;\
        int (*check_type_dump)(FILE*, TYPE*) = fn_dump; (void)check_type_dump;\
        /* Register type information. */\
        static cutest_type_info_t s_info = {\
            { NULL, NULL, NULL },\
            #TYPE,\
            (cutest_custom_type_cmp_fn)fn_cmp,\
            (cutest_custom_type_dump_fn)fn_dump,\
        };\
        static int s_token = 0;\
        if (s_token == 0) {\
            s_token = 1;\
            cutest_internal_register_type(&s_info);\
        }\
    } TEST_MSVC_WARNNING_GUARD(while (0), 4127)

/**
 * @brief Compare template.
 * @warning It is for internal usage.
 * @param[in] TYPE  Type name.
 * @param[in] OP    Compare operation.
 * @param[in] a     Left operator.
 * @param[in] b     Right operator.
 * @param[in] fmt   Extra print format when assert failure.
 * @param[in] ...   Print arguments.
 */
#define ASSERT_TEMPLATE_EXT(TYPE, OP, a, b, fmt, ...) \
    do {\
        TYPE _L = (a); TYPE _R = (b);\
        if (cutest_internal_compare(#TYPE, (const void*)&_L, (const void*)&_R) OP 0) {\
            break;\
        }\
        cutest_internal_dump(__FILE__, __LINE__, #TYPE, #OP, #a, #b, (const void*)&_L, (const void*)&_R,\
            "" fmt, ##__VA_ARGS__);\
        if (cutest_internal_break_on_failure()) {\
            TEST_DEBUGBREAK;\
        }\
        cutest_internal_assert_failure();\
    } TEST_MSVC_WARNNING_GUARD(while (0), 4127)

/** @cond */

/**
 * @brief Compare function for specific type.
 * @param[in] addr1     Address of value1.
 * @param[in] addr2     Address of value2.
 * @return              0 if equal, <0 if value1 is less than value2, >0 if value1 is more than value2.
 */
typedef int (*cutest_custom_type_cmp_fn)(const void* addr1, const void* addr2);

/**
 * @brief Dump value.
 * @param[in] stream    The file stream to print.
 * @param[in] addr      The address of value.
 * @return              The number of characters printed.
 */
typedef int (*cutest_custom_type_dump_fn)(FILE* stream, const void* addr);

/**
 * @brief Custom type information.
 * @note It is for internal usage.
 */
typedef struct cutest_type_info
{
    cutest_map_node_t           node;       /**< Map node. */
    const char*                 type_name;  /**< The name of type. */
    cutest_custom_type_cmp_fn   cmp;        /**< Compare function. */
    cutest_custom_type_dump_fn  dump;       /**< Dump function. */
} cutest_type_info_t;

/**
 * @brief Register custom type.
 * @warning Use #TEST_REGISTER_TYPE_ONCE().
 * @param[in] info          Type information.
 */
void cutest_internal_register_type(cutest_type_info_t* info);

/**
 * @def TEST_MSVC_WARNNING_GUARD(exp, code)
 * @brief Disable warning for `code'.
 * @note This macro only works for MSVC.
 */
#if defined(_MSC_VER) && _MSC_VER < 1900
#   define TEST_MSVC_WARNNING_GUARD(exp, code)  \
        __pragma(warning(push)) \
        __pragma(warning(disable : code)) \
        exp \
        __pragma(warning(pop))
#else
#   define TEST_MSVC_WARNNING_GUARD(exp, code) \
        exp
#endif

/**
 * @def TEST_DEBUGBREAK
 * @brief Causes a breakpoint in your code, where the user will be prompted to
 *   run the debugger.
 */
#if defined(_MSC_VER)
#   define TEST_DEBUGBREAK      __debugbreak()
#elif (defined(__clang__) || defined(__GNUC__)) && (defined(__x86_64__) || defined(__i386__))
#   define TEST_DEBUGBREAK      __asm__ volatile("int $0x03")
#elif (defined(__clang__) || defined(__GNUC__)) && defined(__thumb__)
#   define TEST_DEBUGBREAK      __asm__ volatile(".inst 0xde01")
#elif (defined(__clang__) || defined(__GNUC__)) && defined(__arm__) && !defined(__thumb__)
#   define TEST_DEBUGBREAK      __asm__ volatile(".inst 0xe7f001f0")
#elif (defined(__clang__) || defined(__GNUC__)) && defined(__aarch64__) && defined(__APPLE__)
#   define TEST_DEBUGBREAK      __builtin_debugtrap()
#elif (defined(__clang__) || defined(__GNUC__)) && defined(__aarch64__)
#   define TEST_DEBUGBREAK      __asm__ volatile(".inst 0xd4200000")
#elif (defined(__clang__) || defined(__GNUC__)) && defined(__powerpc__)
#   define TEST_DEBUGBREAK      __asm__ volatile(".4byte 0x7d821008")
#elif (defined(__clang__) || defined(__GNUC__)) && defined(__riscv)
#   define TEST_DEBUGBREAK      __asm__ volatile(".4byte 0x00100073")
#else
#   define TEST_DEBUGBREAK      *(volatile int*)NULL = 1
#endif

/**
 * @brief Compare value1 and value2 of specific type.
 * @param[in] type_name The name of type.
 * @param[in] addr1     The address of value1.
 * @param[in] addr2     The address of value2.
 * @return              Compare result.
 */
int cutest_internal_compare(const char* type_name, const void* addr1, const void* addr2);

/**
 * @brief Dump compare result.
 * @param[in] file      The file name.
 * @param[in] line      The line number.
 * @param[in] type_name The name of type.
 * @param[in] op        The string of operation.
 * @param[in] op_l      The string of left operator.
 * @param[in] op_r      The string of right operator.
 * @param[in] addr1     The address of value1.
 * @param[in] addr2     The address of value2.
 * @param[in] fmt       User defined print format.
 * @param[in] ...       Print arguments to \p fmt.
 */
void cutest_internal_dump(const char* file, int line, const char* type_name,
    const char* op, const char* op_l, const char* op_r,
    const void* addr1, const void* addr2, const char* fmt, ...);

/**
 * @brief Check if `--test_break_on_failure` is set.
 * @return              Boolean.
 */
int cutest_internal_break_on_failure(void);

/**
 * @brief Set current test as failure
 * @note This function is available in setup stage and test body.
 * @warning Call this function in TearDown stage will cause assert.
 */
void cutest_internal_assert_failure(void);

/** @endcond */

/**
 * Group: TEST_CUSTOM_TYPE
 * @}
 */

/**
 * @defgroup TEST_RUN Run
 * @{
 */

/**
 * @brief CUnitTest hook
 */
typedef struct cutest_hook
{
    /**
     * @brief Hook before run all tests
     * @param[in] argc  The number of arguments
     * @param[in] argv  Argument list
     */
    void(*before_all_test)(int argc, char* argv[]);

    /**
     * @brief Hook after run all tests
     */
    void(*after_all_test)(void);

    /**
     * @brief Hook before #TEST_FIXTURE_SETUP() is called
     * @param[in] fixture   Fixture name
     */
    void(*before_setup)(const char* fixture);

    /**
     * @brief Hook after #TEST_FIXTURE_SETUP() is called
     * @param[in] fixture   Fixture name
     * @param[in] ret       zero: #TEST_FIXTURE_SETUP() success, otherwise failure
     */
    void(*after_setup)(const char* fixture, int ret);

    /**
     * @brief Hook before #TEST_FIXTURE_TEAREDOWN() is called
     * @param[in] fixture   Fixture name
     */
    void(*before_teardown)(const char* fixture);

    /**
     * @brief Hook after #TEST_FIXTURE_TEAREDOWN() is called
     * @param[in] fixture   Fixture name
     * @param[in] ret       zero: #TEST_FIXTURE_TEAREDOWN() success, otherwise failure
     */
    void(*after_teardown)(const char* fixture, int ret);

    /**
     * @brief Hook before #TEST_F() is called
     * @param[in] fixture       Fixture name
     * @param[in] test_name     Test name
     */
    void(*before_test)(const char* fixture, const char* test_name);

    /**
     * @brief Hook after #TEST_F() is called
     * @param[in] fixture       Fixture name
     * @param[in] test_name     Test name
     * @param[in] ret           zero: #TEST_F() success, otherwise failure
     */
    void(*after_test)(const char* fixture, const char* test_name, int ret);
} cutest_hook_t;

/**
 * @brief Run all test cases
 * @snippet main.c ENTRYPOINT
 * @param[in] argc      The number of arguments.
 * @param[in] argv      The argument list.
 * @param[in] out       Output stream, cannot be NULL.
 * @param[in] hook      Test hook.
 * @return              0 if success, otherwise failure.
 */
int cutest_run_tests(int argc, char* argv[], FILE* out, const cutest_hook_t* hook);

/**
 * @brief Get current running suit name
 * @return              The suit name
 */
const char* cutest_get_current_fixture(void);

/**
 * @brief Get current running case name
 * @return              The case name
 */
const char* cutest_get_current_test(void);

/**
 * @brief Skip current test case.
 * @note This function only has affect in setup stage.
 * @see TEST_CLASS_SETUP
 */
void cutest_skip_test(void);

/**
 * Group: TEST_RUN
 * @}
 */

/**
 * @defgroup TEST_PORTING Porting
 *
 * By default [cutest](https://github.com/qgymib/cutest) support Windows/Linux,
 * and if you need to porting it to some other OS, you need to implementation
 * all the interfaces listed in this chapter.
 *
 * Do note that the following API is non-optional and operation system must provide it:
 * + [va_start()](https://linux.die.net/man/3/va_start)
 * + [va_end()](https://linux.die.net/man/3/va_end)
 *
 * @{
 */

/**
 * @defgroup TEST_PORTING_SYSTEM_API System API
 * 
 * To use porting interface, add `CUTEST_USE_PORTING` (eg. `-DCUTEST_USE_PORTING`)
 * when compile [cutest](https://github.com/qgymib/cutest).
 *
 * @{
 */

typedef enum cutest_porting_color
{
    CUTEST_COLOR_DEFAULT,
    CUTEST_COLOR_RED,
    CUTEST_COLOR_GREEN,
    CUTEST_COLOR_YELLOW,
} cutest_porting_color_t;

typedef struct cutest_porting_timespec
{
    long    tv_sec;
    long    tv_nsec;
} cutest_porting_timespec_t;

typedef struct cutest_porting_jmpbuf cutest_porting_jmpbuf_t;

/**
 * 
 * ```c
 * struct cutest_porting_jmpbuf
 * {
 *     jmp_buf buf;
 * };
 * void cutest_porting_setjmp(void (*execute)(cutest_porting_jmpbuf_t* buf, int val, void* data),
 *     void* data)
 * {
 *     cutest_porting_jmpbuf_t jmpbuf;
 *     execute(&jmpbuf, setjmp(jmpbuf.buf), data);
 * }
 * ```
 * 
 * @see https://man7.org/linux/man-pages/man3/setjmp.3.html
 */
void cutest_porting_setjmp(void (*execute)(cutest_porting_jmpbuf_t* buf, int val, void* data), void* data);

/**
 * ```c
 * void cutest_porting_longjmp(cutest_porting_jmpbuf_t* buf, int val)
 * {
 *     longjmp(buf->buf, val);
 * }
 * ```
 * @see https://man7.org/linux/man-pages/man3/longjmp.3p.html
 */
void cutest_porting_longjmp(cutest_porting_jmpbuf_t* buf, int val);

/**
 * @see https://linux.die.net/man/3/clock_gettime
 */
void cutest_porting_clock_gettime(cutest_porting_timespec_t* tp);

/**
 * @see https://man7.org/linux/man-pages/man3/abort.3.html
 * @param[in] fmt   Last words.
 * @param[in] ...   Arguments to last words.
 * @return          This function does not return.
 */
int cutest_porting_abort(const char* fmt, ...);

/**
 * @brief Get current thread ID.
 *
 * This function is used for check whether ASSERTION macros are called from
 * main thread. If your system does not support multithread, just return NULL.
 *
 * @see https://man7.org/linux/man-pages/man3/pthread_self.3.html
 * @see https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentthreadid
 */
void* cutest_porting_gettid(void);

/**
 * @brief Colorful print.
 *
 * Different system have different solutions to do colorful print, so we have a
 * interface to do it.
 *
 * Below is a simple implementation that works just fine except drop colorful
 * support:
 *
 * ```c
 * int cutest_porting_cvfprintf(FILE* stream, int color, const char* fmt, va_list ap)
 * {
 *     (void)color;
 *     return fprintf(stream, fmt, ap);
 * }
 * ```
 *
 * @note The color support is optional, it only affects the vision on console.
 *   If you do not known how to add support for it, just ignore the parameter
 *   \p color.
 *
 * @warning If you want to support colorful print, do remember to distinguish
 *   whether data is print to console. If \p stream is a normal file and the
 *   color control charasters are printed into the file, this file is highly
 *   possible mess up.
 *
 * @param[in] stream    The stream to print.
 * @param[in] color     Print color. Checkout #cutest_porting_color_t.
 * @param[in] fmt       Print format.
 * @param[in] ap        Print arguments.
 */
int cutest_porting_cvfprintf(FILE* stream, int color, const char* fmt, va_list ap);

/**
 * Group: TEST_PORTING_SYSTEM_API
 * @}
 */

/**
 * @defgroup TEST_FLOATING_NUMBER Floating-Point Numbers
 *
 * By default we use `EPSILON` and `ULP` to do floating number compre, which
 * assume all floating numbers are matching IEEE 754 Standard (both `float` and
 * `double`).
 *
 * This works fine on most time, but there are indeed some hardware does
 * not follow this standard, and this technology just broken.
 *
 * To use custom compare algorithm, define `CUTEST_USE_PORTING_FLOATING_COMPARE_ALGORITHM`
 * (eg. `-DCUTEST_USE_PORTING_FLOATING_COMPARE_ALGORITHM`) when compile.
 *
 * @see https://bitbashing.io/comparing-floats.html
 *
 * @{
 */

/**
 * @brief Floating number compare.
 * @param[in] type  The type of \p v1 and \p v2. 0 is float, 1 is double.
 * @param[in] v1    The address to value1.
 * @param[in] v2    The address to value2.
 * @return          <0, =0, >0 if \p v1 is less than, equal to, more than \p v2.
 */
int cutest_porting_compare_floating_number(int type, const void* v1, const void* v2);

/**
 * Group: TEST_FLOATING_NUMBER
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif

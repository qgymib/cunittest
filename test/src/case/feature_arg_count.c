#include "test.h"

DEFINE_TEST(arg, count)
{
    TEST_PORTING_ASSERT(TEST_ARG_COUNT(1, 2) == 2);
    TEST_PORTING_ASSERT(TEST_ARG_COUNT(3, 4, 5) == 3);
    TEST_PORTING_ASSERT(TEST_ARG_COUNT({ 6, 7 }) == 2);
}

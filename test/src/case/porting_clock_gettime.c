#include "test.h"

///////////////////////////////////////////////////////////////////////////////
// Porting
///////////////////////////////////////////////////////////////////////////////

void cutest_porting_clock_gettime(cutest_porting_timespec_t* tp)
{
    tp->tv_nsec = 0;
    tp->tv_sec = 0;
}

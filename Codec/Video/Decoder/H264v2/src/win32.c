#include "global.h"

#if defined(WIN32) && !defined(_WIN32_WCE)

static LARGE_INTEGER freq;

void gettime(TIME_T* time)
{
  QueryPerformanceCounter(time);
}

int64 timediff(TIME_T* start, TIME_T* end)
{
  return (int64)((end->QuadPart - start->QuadPart));
}

int64 timenorm(int64  cur_time)
{
  static int first = 1;

  if(first) 
  {
    QueryPerformanceFrequency(&freq);
    first = 0;
  }

  return (int64)(cur_time * 1000 /(freq.QuadPart));
}

#else
#if !(defined(RVDS) || defined(_WIN32_WCE))
static struct timezone tz;

void gettime(TIME_T* time)
{
  gettimeofday(time, &tz);
}

int64 timediff(TIME_T* start, TIME_T* end)
{
  int t1, t2;

  t1 =  end->tv_sec  - start->tv_sec;
  t2 =  end->tv_usec - start->tv_usec;
  return (int64) t2 + (int64) t1 * (int64) 1000000;
}

int64 timenorm(int64 cur_time)
{
  return (int64)(cur_time / (int64) 1000);
}
#endif
#endif

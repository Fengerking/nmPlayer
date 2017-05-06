
#ifndef _IFUNCTIONS_H_
#define _IFUNCTIONS_H_

# if !(defined(WIN32) || defined(WIN64)) && (__STDC_VERSION__ < 199901L)
 // #define static
 // #define inline
#endif
#include <math.h>
#if defined(RVDS)|| defined(_WIN32_WCE)
#include <limits.h>
#endif
#define VOMIN(a,b) ((a) > (b) ? (b) : (a))
#define VOMIN3(a,b,c) VOMIN(VOMIN(a,b),c)
static inline int imin(int a, int b)
{
  return ((a) < (b)) ? (a) : (b);
}

static inline int imax(int a, int b)
{
  return ((a) > (b)) ? (a) : (b);
}
#if 1
static inline int imedian(int a,int b,int c)
{
  if (a > b) // a > b
  { 
    if (b > c) 
      return(b); // a > b > c
    else if (a > c) 
	    return(c); // a > c > b
    else 
      return(a); // c > a > b
  }
  else // b > a
  { 
    if (a > c) 
      return(a); // b > a > c
    else if (b > c)
	    return(c); // b > c > a
    else
      return(b);  // c > b > a
  }
}
#else
static inline int imedian(int a,int b,int c)
{
  int p = (a-b)&((a-b)>>31);
  a -= p;
  b += p;
  b -= (b-c)&((b-c)>>31);
  b += (a-b)&((a-b)>>31);
  p = b;
  return p;
}
#endif
#if 0
static inline int iabs(int x)
{
  static const int INT_BITS = (sizeof(int) * CHAR_BIT) - 1;
  int y = x >> INT_BITS;
  return (x ^ y) - y;
}
#else
static inline int iabs(int x)
{
  return abs(x);
}

#endif
static inline int iabs2(int x) 
{
  return (x) * (x);
}

static inline int isign(int x)
{
  return ( (x > 0) - (x < 0));
}

static inline int isignab(int a, int b)
{
  return ((b) < 0) ? -iabs(a) : iabs(a);
}

static inline int rshift_rnd(int x, int a)
{
  return (a > 0) ? ((x + (1 << (a-1) )) >> a) : (x << (-a));
}

static inline int rshift_rnd_sign(int x, int a)
{
  return (x > 0) ? ( ( x + (1 << (a-1)) ) >> a ) : (-( ( iabs(x) + (1 << (a-1)) ) >> a ));
}

static inline unsigned int rshift_rnd_us(unsigned int x, unsigned int a)
{
  return (a > 0) ? ((x + (1 << (a-1))) >> a) : x;
}

static inline int rshift_rnd_sf(int x, int a)
{
  return ((x + (1 << (a-1) )) >> a);
}

//static inline int iClip1(int high, int x)
//{
//  x = imax(x, 0);
//  x = imin(x, high);

//  return x;
//}

static inline int iClip3(int low, int high, int x)
{
  x = imax(x, low);
  x = imin(x, high);

  return x;
}

static inline int RSD(int x)
{
 return ((x&2)?(x|1):(x&(~1)));
}


static inline int get_bit(int64 x,int n)
{
  return (int)(((x >> n) & 1));
}



# if !(defined(WIN32) || defined(WIN64)) && (__STDC_VERSION__ < 199901L)
  #undef static
  #undef inline
#endif

#endif


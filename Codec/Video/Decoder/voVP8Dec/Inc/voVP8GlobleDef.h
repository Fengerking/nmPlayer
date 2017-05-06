#ifndef VP8_GLOBLE_H
#define VP8_GLOBLE_H

#include "voVP8DecID.h"

//#define INLINE  __forceinline
#ifdef VOWINCE
#define INLINE __forceinline
#elif RVDS
#define INLINE  
#elif VOWINXP
#define INLINE __forceinline
#elif  _IOS
#define INLINE __inline
#elif VOANDROID 
#define INLINE __inline
#endif


#endif

////Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// // All rights reserved.
// // This file is part of FirteX (www.firtex.org)
// //
//// Use of the FirteX is subject to the terms of the software license set forth in 
// // the LICENSE file included with this software, and also available at
// // http://www.firtex.org/license.html
// //
//
////
// #ifndef __STDINT_H__
// #define __STDINT_H__
//  #include <wchar.h>
//
// #ifndef WIN32
//       #include <inttypes.h>
//
//        typedef uint8_t                 byte;
// 
// #else//for Win32
//        // #include <sys/types.h>
//
//        // #include <wchar.h>
//typedef char int8_t;
//        typedef short                           int16_t;
//         typedef long                            int32_t;
//         typedef __int64                         int64_t;
//        typedef unsigned char           uint8_t;
//         typedef unsigned short          uint16_t;
//         typedef unsigned long           uint32_t;
//        typedef unsigned __int64        uint64_t;
//         typedef unsigned char           byte;
// 
//         typedef __int64                         intmax_t;
//        typedef unsigned __int64        uintmax_t;
//
//#endif
// 
//         typedef int64_t                         fileoffset_t;
// 
//         /* LIMIT MACROS */
//         #ifndef MAX_UINT64
//                 #define MAX_UINT64  ( ~ ((uint64_t) 0) )
//       #endif
//
//        #ifndef MAX_INT64
//               #define MAX_INT64   ( (int64_t) ( MAX_UINT64 >> 1 ) )
//         #endif
//
//        #ifndef MIN_INT64
//                #define MIN_INT64   ( (int64_t) ( MAX_UINT64 ^ ( (uint64_t) MAX_INT64 ) ) )
//        #endif
// 
//         #ifndef MAX_UINT32
//                 #define MAX_UINT32  ( ~ ((uint32_t) 0) )
//         #endif
//
//        #ifndef MAX_INT32
//                 #define MAX_INT32   ( (int32_t) (MAX_UINT32 >> 1) )
//         #endif 
//         #ifndef MIN_INT32
//                 #define MIN_INT32   ( (int32_t) ( MAX_UINT32 ^ ( (uint32_t) MAX_INT32 ) ) )
//        #endif
//
//#undef MAX
// #define MAX(a, b)  (((a) > (b)) ? (a) : (b))
// 
// #undef  MIN
//#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
// #ifndef NULL
//       #define NULL 0
// #endif
//
//#endif /* __STDINT_H__ */

#if defined(WIN32) && !defined(__MINGW32__) && !defined(__CYGWIN__)
#    define CONFIG_WIN32
#endif
#if defined(WIN32) && !defined(__MINGW32__) && !defined(__CYGWIN__) && !defined(EMULATE_INTTYPES)
#    define EMULATE_INTTYPES
#endif
#ifndef EMULATE_INTTYPES
#   include <inttypes.h>
#else
    typedef signed char  int8_t;
    typedef signed short int16_t;
    typedef signed int   int32_t;
    typedef unsigned char  uint8_t;
    typedef unsigned short uint16_t;
    typedef unsigned int   uint32_t;
#   ifdef CONFIG_WIN32
        typedef signed __int64   int64_t;
        typedef unsigned __int64 uint64_t;
#   else /* other OS */
        typedef signed long long   int64_t;
        typedef unsigned long long uint64_t;
#   endif /* other OS */
#endif /* EMULATE_INTTYPES */

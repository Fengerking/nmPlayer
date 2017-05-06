/*
 * NOTE: Porting candidate.
 *
 * Contains macros used to offset an determine differences between different
 * platforms & compilers.
 */

#ifndef __MKBase_Platform_h__
#define __MKBase_Platform_h__

/******************************************************************************/

/*
 * NOTE: Some defines are required before we include anything in some cases.
 */

#if __GNUC__
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE 1
    #endif
#endif

#if __GNUC__ || (__STDC_VERSION__ && __STDC_VERSION__ >= 199901L)
    #ifndef _ISOC99_SOURCE
        #define _ISOC99_SOURCE 1
    #endif
#endif

/******************************************************************************/

#include <stddef.h>
#include <limits.h>

/******************************************************************************/

#ifndef MK_UNUSED
    #define MK_UNUSED(v) ((void)v)
#endif

#ifndef MK_STR
    #define MK_STR(v) #v
#endif

#ifndef MK_XSTR
    #define MK_XSTR(v) MK_STR(v)
#endif

/******************************************************************************/

#ifndef MK_REV
    #define MK_REV "?"
#endif

#ifndef MK_BUILD
    #define MK_BUILD "?"
#endif

#ifndef MK_VERSION
    #define MK_VERSION "1.0.21"
#endif

/******************************************************************************/

#if defined(DEBUG) || defined(_DEBUG)
    #ifndef MK_DEBUG
        #define MK_DEBUG 1
    #elif !MK_DEBUG
        #error Conflicting debug macro definitions. Please fix!
    #endif
#endif

#if defined(NDEBUG)
    #ifndef MK_DEBUG
        #define MK_DEBUG 0
    #elif MK_DEBUG
        #error Conflicting debug macro definitions. Please fix!
    #endif
#endif

#ifndef MK_DEBUG
    #define MK_DEBUG 1
#endif

#ifndef MK_NDEBUG
    #define MK_NDEBUG (!MK_DEBUG)
#endif

/******************************************************************************/

#if __i386__ || __i686__ || _M_IX86
    #define MK_X86_32 1
#endif

#if __x86_64__ || __amd64__ || _M_X64
    #define MK_X86_64 1
#endif

#define MK_X86 (MK_X86_32 || MK_X86_64)

/******************************************************************************/

#if __arm__ || __thumb__ || _M_ARM || _M_ARMT
    #define MK_ARM_32 1
#endif

#define MK_ARM (MK_ARM_32)

#if MK_ARM && !defined(MK_ARM_ARCH)
    #if __ARM_ARCH_5__ || __ARM_ARCH_5T__ || __ARM_ARCH_5E__ || __ARM_ARCH_5TE__ || __ARM_ARCH_5TEJ__
        #define MK_ARM_ARCH 5
    #elif __ARM_ARCH_6__ || __ARM_ARCH_6J__ || __ARM_ARCH_6K__ || __ARM_ARCH_6Z__ || __ARM_ARCH_6ZK__ || __ARM_ARCH_6T2__
        #define MK_ARM_ARCH 6
    #elif __ARM_ARCH_7__ || __ARM_ARCH_7A__ || __ARM_ARCH_7R__ || __ARM_ARCH_7M__
        #define MK_ARM_ARCH 7
    #elif _M_ARM
        #define MK_ARM_ARCH _M_ARM
    #elif _M_ARMT
        #define MK_ARM_ARCH _M_ARMT
    #else
        #define MK_ARM_ARCH 4
    #endif
#endif

#if MK_ARM && !defined(MK_ARM_THUMB)
    #if __thumb2__
        #define MK_ARM_THUMB 2
    #elif __thumb__ || _M_ARMT
        #define MK_ARM_THUMB 1
    #endif
#endif

/******************************************************************************/

#ifndef MK_ARCH
    #if MK_X86
        #if MK_X86_64
            #define MK_ARCH "x86_64"
        #elif MK_X86_32
            #define MK_ARCH "x86"
        #endif
    #elif MK_ARM
        #if MK_ARM_THUMB
            #if 1 < MK_ARM_THUMB
                #define MK_ARCH "ARMv"MK_XSTR(MK_ARM_ARCH)"T"MK_XSTR(MK_ARM_THUMB)
            #else
                #define MK_ARCH "ARMv"MK_XSTR(MK_ARM_ARCH)"T"
            #endif
        #else
            #define MK_ARCH "ARMv"MK_XSTR(MK_ARM_ARCH)
        #endif
    #else
        #define MK_ARCH "Unknown"
    #endif
#endif

/******************************************************************************/

#if __APPLE__
    #ifndef MK_DARWIN
        #define MK_DARWIN 1
    #endif
#endif

/* TODO: Improve MK_IPHONE detection? */
#if MK_DARWIN && MK_ARM
    #ifndef MK_IPHONE
        #define MK_IPHONE 1
    #endif
#endif

/******************************************************************************/

#if __linux || __BIONIC__
    #ifndef MK_LINUX
        #define MK_LINUX 1
    #endif
#endif

#if __BIONIC__
    #define MK_ANDROID 1
#endif

#if MK_LINUX
    #define _FILE_OFFSET_BITS 64
#endif

/******************************************************************************/

#if _WIN32 || WIN32 || _MSC_VER
    #ifndef MK_WIN32
        #define MK_WIN32 1
    #endif
#endif

#if MK_WIN32 && MK_ARM
    #ifndef MK_WINCE
        #define MK_WINCE 1
    #endif
#endif

#if MK_WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
#endif

/******************************************************************************/

#ifndef MK_PLATFORM
    #if MK_DARWIN
        #if MK_IPHONE
            #define MK_PLATFORM "iPhone"
        #else
            #define MK_PLATFORM "Darwin"
        #endif
    #elif MK_LINUX
        #if MK_ANDROID
            #define MK_PLATFORM "Android"
        #else
            #define MK_PLATFORM "Linux"
        #endif
    #elif MK_WIN32
        #if MK_WINCE
            #define MK_PLATFORM "WinCE"
        #else
            #define MK_PLATFORM "Win32"
        #endif
    #else
        #define MK_PLATFORM "Unknown"
    #endif
#endif

/******************************************************************************/

#ifndef FALSE
    #define FALSE 0
#endif
#define MK_FALSE FALSE

#ifndef TRUE
    #define TRUE 1
#endif
#define MK_TRUE TRUE

/******************************************************************************/

#ifndef MK_RESTRICT
    #if __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
        #define MK_RESTRICT restrict
    #elif __GNUC__ >= 3
        #define MK_RESTRICT __restrict__
    #elif _MSC_VER
        #define MK_RESTRICT __restrict
    #else
        #define MK_RESTRICT
    #endif
#endif

#ifndef MK_EXTERN
    #if __cplusplus
        #define MK_EXTERN extern "C"
    #else
        #define MK_EXTERN
    #endif
#endif

#ifndef MK_STATIC
    #define MK_STATIC static
#endif

#ifndef MK_INLINE
    #if __cplusplus
        #define MK_INLINE inline
    #else
        #define MK_INLINE static __inline
    #endif
#endif

/******************************************************************************/

#ifndef MK_PTRSIZE
    #if _WIN64 || __LP64__
        #define MK_PTRSIZE 8
    #else
        #define MK_PTRSIZE 4
    #endif
#endif

#ifndef MK_STRICT_ALIGNMENT
    #if MK_ARM
        #define MK_STRICT_ALIGNMENT 1
    #endif
#endif

/******************************************************************************/

#ifndef MK_BIG_ENDIAN
    #if (__BYTE_ORDER && __BYTE_ORDER == __BIG_ENDIAN) || \
        (BYTE_ORDER && BYTE_ORDER == BIG_ENDIAN) || \
        __ARMEB__ || __sparc__

        #define MK_BIG_ENDIAN 1
    #endif
#endif

#ifndef MK_LITTLE_ENDIAN
    #if (__BYTE_ORDER && __BYTE_ORDER == __LITTLE_ENDIAN) || \
        (BYTE_ORDER && BYTE_ORDER == LITTLE_ENDIAN) || \
        MK_X86 || MK_WIN32 || __ARMEL__ || __ia64__ || _M_IA64

        #define MK_LITTLE_ENDIAN 1
    #endif
#endif

/******************************************************************************/

#ifndef MK_HAVE_BUILTIN_BSWAP32
    #if __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 3 || (__APPLE_CC__ && __GNUC_MINOR__ >= 2))))
        #define MK_HAVE_BUILTIN_BSWAP32 1
    #endif
#endif

#ifndef MK_HAVE_BUILTIN_BSWAP64
    #if MK_HAVE_BUILTIN_BSWAP32 && !MK_ANDROID
        #define MK_HAVE_BUILTIN_BSWAP64 1
    #endif
#endif

#ifndef MK_HAVE_BUILTIN_SYNC
    #if (__GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 4 || (!MK_ARM && __GNUC_MINOR__ >= 1)))) && \
        (!__i386__ || (MK_DARWIN || __i486__ || __i586__ || __i686__)) \
        && !MK_ANDROID

        /* NOTE: Excluding Android for now (latest compiler, gcc 4.4.0, doesn't support atomics) */
        #define MK_HAVE_BUILTIN_SYNC 1
    #endif
#endif

/******************************************************************************/

#ifndef MK_HAVE_MEMRCHR
    /*#define MK_HAVE_MEMRCHR 0*/
#endif

#ifndef MK_HAVE_CALLOC
    #define MK_HAVE_CALLOC 1
#endif

#ifndef MK_HAVE_REALLOC
    #define MK_HAVE_REALLOC 1
#endif

/******************************************************************************/

#ifndef MK_HAVE_STRLEN
    #define MK_HAVE_STRLEN 1
#endif

#ifndef MK_HAVE_STRCMP
    #define MK_HAVE_STRCMP 1
#endif

#ifndef MK_HAVE_STRNCMP
    #define MK_HAVE_STRNCMP 1
#endif

#ifndef MK_HAVE_STRCASECMP
    #if !MK_WIN32
        #define MK_HAVE_STRCASECMP 1
    #endif
#endif

#ifndef MK_HAVE__STRICMP
    #if MK_WIN32
        #define MK_HAVE__STRICMP 1
    #endif
#endif

#ifndef MK_HAVE_STRCHR
    #define MK_HAVE_STRCHR 1
#endif

#ifndef MK_HAVE_STRNCHR
    /*#define MK_HAVE_STRNCHR 0*/
#endif

#ifndef MK_HAVE_STRRCHR
    #define MK_HAVE_STRRCHR 1
#endif

#ifndef MK_HAVE_STRNRCHR
    /*#define MK_HAVE_STRNRCHR 0*/
#endif

#ifndef MK_HAVE_STRSTR
    #define MK_HAVE_STRSTR 1
#endif

#ifndef MK_HAVE_STRNCASECMP
    #if !MK_WIN32
        #define MK_HAVE_STRNCASECMP 1
    #endif
#endif

#ifndef MK_HAVE__STRNICMP
    #if MK_WIN32
        #define MK_HAVE__STRNICMP 1
    #endif
#endif

#ifndef MK_HAVE_STRNSTR
    #if MK_DARWIN
        #define MK_HAVE_STRNSTR 1
    #endif
#endif

#ifndef MK_HAVE_STRCASESTR
    #if !MK_WIN32
        #define MK_HAVE_STRCASESTR 1
    #endif
#endif

#ifndef MK_HAVE_STRNCASESTR
    /*#define MK_HAVE_STRNCASESTR 0*/
#endif

#ifndef MK_HAVE_STRSPN
    #define MK_HAVE_STRSPN 1
#endif

#ifndef MK_HAVE_STRCSPN
    #define MK_HAVE_STRCSPN 1
#endif

#ifndef MK_HAVE_STRPBRK
    #define MK_HAVE_STRPBRK 1
#endif

#ifndef MK_HAVE_STRLCPY
    #if MK_DARWIN
        #define MK_HAVE_STRLCPY 1
    #endif
#endif

#ifndef MK_HAVE_STRLCAT
    #if MK_DARWIN
        #define MK_HAVE_STRLCAT 1
    #endif
#endif

#ifndef MK_HAVE_STRDUP
    #if !MK_WIN32
        #define MK_HAVE_STRDUP 1
    #endif
#endif

#ifndef MK_HAVE__STRDUP
    #if MK_WIN32
        #define MK_HAVE__STRDUP 1
    #endif
#endif

/******************************************************************************/

#ifndef MK_HAVE_SA_STORAGE
    #define MK_HAVE_SA_STORAGE 1
#endif

#ifndef MK_HAVE_SA_LEN
    #if MK_DARWIN 
        #define MK_HAVE_SA_LEN 1
    #endif
#endif

#ifndef MK_HAVE_ADDRINFO
    #define MK_HAVE_ADDRINFO 1
#endif

/******************************************************************************/

#ifndef MK_HAVE_MSG_NOSIGNAL
    #if !MK_DARWIN
        #define MK_HAVE_MSG_NOSIGNAL 1
    #endif
#endif

#ifndef MK_HAVE_SO_NOSIGPIPE
    #if MK_DARWIN
        #define MK_HAVE_SO_NOSIGPIPE 1
    #endif
#endif

#ifndef MK_HAVE_SO_REUSEPORT
    #if MK_DARWIN
        #define MK_HAVE_SO_REUSEPORT 1
    #endif
#endif

#ifndef MK_HAVE_FIONBIO
    #define MK_HAVE_FIONBIO 1
#endif

/******************************************************************************/

#ifndef MK_HAVE_KQUEUE
    #if MK_DARWIN
        #define MK_HAVE_KQUEUE 1
    #endif
#endif

#ifndef MK_HAVE_EPOLL
    #if MK_LINUX
        #define MK_HAVE_EPOLL 1
    #endif
#endif

/******************************************************************************/

#endif

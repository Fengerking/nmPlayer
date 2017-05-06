/*
 * Contains macros used to configure the feature-set of the MKBase library at
 * compile time. 
 */

#ifndef __MKBase_Config_h__
#define __MKBase_Config_h__

#include <MKBase/Platform.h>

/******************************************************************************/

/*
 * Memory debugging configuration.
 *
 * MK_MEMCHECK
 *  - Currently supported on Darwin/iOS and Linux/Android
 *  - Enable sentinels before and after allocations
 *  - Fill space on allocations and frees
 *  - Default: disabled
 */
#ifndef MK_MEMCHECK
    #define MK_MEMCHECK 0
#endif

/*
 * Make sure we use fallbacks based on MK_Mem_Alloc() for functions which make
 * allocations (or some frees may crash).
 */
#if MK_MEMCHECK
    #undef MK_HAVE_CALLOC
    #undef MK_HAVE_REALLOC
    #undef MK_HAVE_STRDUP
    #undef MK_HAVE__STRDUP
#endif

/******************************************************************************/

/*
 * Logging configuration.
 *
 * MK_ENABLE_LOGTRAC
 *  - Enable MK_LOGTRAC macro (compiled out if disabled)
 *  - Default: enabled in debug builds, disabled in release
 *
 * MK_ENABLE_LOGDBG
 *  - Enable MK_LOGDBG macro (compiled out if disabled)
 *  - Default: enabled
 *
 * MK_ENABLE_LOGINFO
 *  - Enable MK_LOGINFO macro (compiled out if disabled)
 *  - Default: enabled
 *
 * MK_ENABLE_LOGWARN
 *  - Enable MK_LOGWARN macro (compiled out if disabled)
 *  - Default: enabled
 *
 * MK_ENABLE_LOGERR
 *  - Enable MK_LOGERR macro (compiled out if disabled)
 *  - Default: enabled
 *
 * MK_LOG_L_DEFAULT
 *  - Set default log level filter
 *  - Default: MK_LOG_L_DBG in debug builds, MK_LOG_L_INFO in release
 */

#ifndef MK_ENABLE_LOGTRAC
    #if MK_DEBUG
        #define MK_ENABLE_LOGTRAC 1
    #else
        #define MK_ENABLE_LOGTRAC 0
    #endif
#endif

#ifndef MK_ENABLE_LOGDBG
    #define MK_ENABLE_LOGDBG 1
#endif

#ifndef MK_ENABLE_LOGINFO
    #define MK_ENABLE_LOGINFO 1
#endif

#ifndef MK_ENABLE_LOGWARN
    #define MK_ENABLE_LOGWARN 1
#endif

#ifndef MK_ENABLE_LOGERR
    #define MK_ENABLE_LOGERR 1
#endif

#ifndef MK_LOG_L_DEFAULT
    #if MK_DEBUG
        #define MK_LOG_L_DEFAULT MK_LOG_L_DBG
    #else
        #define MK_LOG_L_DEFAULT MK_LOG_L_INFO
    #endif
#endif

/******************************************************************************/

/*
 * Thread safety configuration.
 *
 * MK_ENABLE_MPSAFE
 *  - Enable support for SMP systems (e.g. multiple cores/processors)
 *  - Implies MK_ENABLE_MTSAFE
 *  - Default: disabled
 *
 * MK_ENABLE_MTSAFE
 *  - Enable multi-threading support
 *  - Default: enabled
 */

#ifndef MK_ENABLE_MPSAFE
    #define MK_ENABLE_MPSAFE 0
#endif

#ifndef MK_ENABLE_MTSAFE
    #define MK_ENABLE_MTSAFE 1
#elif MK_ENABLE_MPSAFE
    #error Invalid SMP/threading configuration
#endif

/******************************************************************************/

#endif

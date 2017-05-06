#ifndef __MKBase_Log_h__
#define __MKBase_Log_h__

#include <MKBase/Config.h>
#include <MKBase/Type.h>
#include <MKBase/Result.h>

#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

/*
 * Define MK_LOGTRAC macros based on value of MK_ENABLE_LOGTRAC.
 */
#if MK_ENABLE_LOGTRAC
    #define MK_LOGTRAC(aFmt, ...) MK_Log(MK_LOG_L_TRAC, aFmt , ## __VA_ARGS__)
    #define MK_LOGTRACIF(aCond, aFmt, ...) if (aCond) { MK_LOGTRAC(aFmt , ## __VA_ARGS__); } else {}
    #define MK_LOGXTRAC(aBuf, aN, aHdr) MK_LogX(MK_LOG_L_TRAC, aBuf, aN, aHdr)
    #define MK_LOGXTRACIF(aCond, aBuf, aN, aHdr) if (aCond) { MK_LOGXTRAC(aBuf, aN, aHdr); } else {}

    #define MK_LOGTRACHEXBUF(aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator) \
            MK_LogHexBuf(MK_LOG_L_TRAC, aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator)
#else
    #define MK_LOGTRAC(aFmt, ...)
    #define MK_LOGTRACIF(aCond, aFmt, ...)
    #define MK_LOGXTRAC(aBuf, aN, aHdr)
    #define MK_LOGXTRACIF(aCond, aBuf, aN, aHdr)

    #define MK_LOGTRACHEXBUF(aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator)
#endif

#define MK_LOG_L_TRAC 0

/******************************************************************************/

/*
 * Define MK_LOGDBG macros based on value of MK_ENABLE_LOGDBG.
 */
#if MK_ENABLE_LOGDBG
    #define MK_LOGDBG(aFmt, ...) MK_Log(MK_LOG_L_DBG, aFmt , ## __VA_ARGS__)
    #define MK_LOGDBGIF(aCond, aFmt, ...) if (aCond) { MK_LOGDBG(aFmt , ## __VA_ARGS__); } else {}
    #define MK_LOGXDBG(aBuf, aN, aHdr) MK_LogX(MK_LOG_L_DBG, aBuf, aN, aHdr)
    #define MK_LOGXDBGIF(aCond, aBuf, aN, aHdr) if (aCond) { MK_LOGXDBG(aBuf, aN, aHdr); } else {}

    #define MK_LOGDBGHEXBUF(aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator) \
            MK_LogHexBuf(MK_LOG_L_DBG, aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator)
#else
    #define MK_LOGDBG(aFmt, ...)
    #define MK_LOGDBGIF(aCond, aFmt, ...)
    #define MK_LOGXDBG(aBuf, aN, aHdr)
    #define MK_LOGXDBGIF(aCond, aBuf, aN, aHdr)

    #define MK_LOGDBGHEXBUF(aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator)
#endif

#define MK_LOG_L_DBG 1

/******************************************************************************/

/*
 * Define MK_LOGINFO macros based on value of MK_ENABLE_LOGINFO.
 */
#if MK_ENABLE_LOGINFO
    #define MK_LOGINFO(aFmt, ...) MK_Log(MK_LOG_L_INFO, aFmt , ## __VA_ARGS__)
    #define MK_LOGINFOIF(aCond, aFmt, ...) if (aCond) { MK_LOGINFO(aFmt , ## __VA_ARGS__); } else {}
    #define MK_LOGXINFO(aBuf, aN, aHdr) MK_LogX(MK_LOG_L_INFO, aBuf, aN, aHdr)
    #define MK_LOGXINFOIF(aCond, aBuf, aN, aHdr) if (aCond) { MK_LOGXINFO(aBuf, aN, aHdr); } else {}

    #define MK_LOGINFOHEXBUF(aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator) \
            MK_LogHexBuf(MK_LOG_L_INFO, aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator)
#else
    #define MK_LOGINFO(aFmt, ...)
    #define MK_LOGINFOIF(aCond, aFmt, ...)
    #define MK_LOGXINFO(aBuf, aN, aHdr)
    #define MK_LOGXINFOIF(aCond, aBuf, aN, aHdr)

    #define MK_LOGINFOHEXBUF(aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator)
#endif

#define MK_LOG_L_INFO 2

/******************************************************************************/

/*
 * Define MK_LOGWARN macros based on value of MK_ENABLE_LOGWARN.
 */
#if MK_ENABLE_LOGWARN
    #define MK_LOGWARN(aFmt, ...) MK_Log(MK_LOG_L_WARN, aFmt , ## __VA_ARGS__)
    #define MK_LOGWARNIF(aCond, aFmt, ...) if (aCond) { LOGWARN(aFmt , ## __VA_ARGS__); } else {}
    #define MK_LOGXWARN(aBuf, aN, aHdr) MK_LogX(MK_LOG_L_WARN, aBuf, aN, aHdr)
    #define MK_LOGXWARNIF(aCond, aBuf, aN, aHdr) if (aCond) { MK_LOGXWARN(aBuf, aN, aHdr); } else {}

    #define MK_LOGWARNHEXBUF(aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator) \
            MK_LogHexBuf(MK_LOG_L_WARN, aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator)
#else
    #define MK_LOGWARN(aFmt, ...)
    #define MK_LOGWARNIF(aCond, aFmt, ...)
    #define MK_LOGXWARN(aBuf, aN, aHdr)
    #define MK_LOGXWARNIF(aCond, aBuf, aN, aHdr)

    #define MK_LOGWARNHEXBUF(aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator)
#endif

#define MK_LOG_L_WARN 3

/******************************************************************************/

/*
 * Define MK_LOGERR macros based on value of MK_ENABLE_LOGERR.
 */
#if MK_ENABLE_LOGERR
    #define MK_LOGERR(aFmt, ...) MK_Log(MK_LOG_L_ERR, aFmt , ## __VA_ARGS__)
    #define MK_LOGERRIF(aCond, aFmt, ...) if (aCond) { MK_LOGERR(aFmt , ## __VA_ARGS__); } else {}
    #define MK_LOGXERR(aBuf, aN, aHdr) MK_LogX(MK_LOG_L_ERR, aBuf, aN, aHdr)
    #define MK_LOGXERRIF(aCond, aBuf, aN) if (aCond) { MK_LOGXERR(aBuf, aN, aHdr); } else {}

    #define MK_LOGERRHEXBUF(aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator) \
            MK_LogHexBuf(MK_LOG_L_ERR, aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator)
#else
    #define MK_LOGERR(aFmt, ...)
    #define MK_LOGERRIF(aCond, aFmt, ...)
    #define MK_LOGXERR(aBuf, aN, aHdr)
    #define MK_LOGXERRIF(aCond, aBuf, aN, aHdr)

    #define MK_LOGERRHEXBUF(aBuf, aLen, aNbrPerLine, aLinePrefix, aIndexBase, aFirstValPrefix, aValSeparator)
#endif

#define MK_LOG_L_ERR 4

/******************************************************************************/

typedef enum
{
    kNo  = 0,
    kDec = 10,
    kHex = 0x10,
} eLineIndexType;

/******************************************************************************/

/*
 * Function pointers used to supply message and end notifications to logger
 * clients (see MK_Log_ToFunc() below). The arguments provided to the functions
 * are:
 *
 * aObj - A user defined object
 * aTm  - A time (in milliseconds) since the active logger was initialized
 * aLev - The log-level of the message
 * aLn  - The message lines (may be more than one)
 * aLnN - The length of each message line
 * aN   - The number of message lines
 *
 * NOTE: Logging through the same logger from one of the function callbacks will
 *       cause undefined behaivour.
 *
 * NOTE: Currently there is only one single global logger.
 */
typedef void (*MK_Log_MsgFunc)(void* aObj, MK_U32 aTm, MK_U32 aLev, const MK_Char* aLn[], MK_U32 aLnN[], MK_U32 aN);
typedef void (*MK_Log_EndFunc)(void* aObj);

/******************************************************************************/

/*
 * Log messages with a level of aMinLev or higher to the function aMsgFunc. The
 * value of aObj is provided as a user-defined value. The function specified by
 * aEndFunc is called when the log-function changes to ease proper destruction
 * of the user-defined object aObj.
 */
void MK_Log_ToFunc(MK_Log_MsgFunc aMsgFunc, MK_Log_EndFunc aEndFunc, void* aObj, MK_U32 aMinLev);

/*
 * Redirects logs with a level of aMinLev or higher to a console (stderr).
 */
void MK_Log_ToConsole(MK_U32 aMinLev);

/*
 * Log messages with a level of aMinLev or higher directly to file.
 */
MK_Result MK_Log_ToFile(const MK_Char* aPath, MK_U32 aMinLev);

/*
 * Discard all log messages (i.e. disable logging).
 */
void MK_Log_Discard(void);

/******************************************************************************/

MK_INLINE const MK_Char* MK_Log_L_ToCStr(MK_U32 aLev);

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

MK_INLINE const MK_Char* MK_Log_L_ToCStr(MK_U32 aLev)
{
    switch (aLev)
    {
        case MK_LOG_L_TRAC: return "TRAC";
        case MK_LOG_L_DBG: return "DBG";
        case MK_LOG_L_INFO: return "INFO";
        case MK_LOG_L_WARN: return "WARN";
        case MK_LOG_L_ERR: return "ERR";
        default: return "UNKN";
    }
}

/******************************************************************************/

/*
 * Don't use these functions directly, use logging macros above.
 */

void MK_Log(MK_U32 aLev, const MK_Char* aFmt, ...);
void MK_LogV(MK_U32 aLev, const MK_Char* aFmt, va_list aArgs);

/* NOTE: aHdr last to allow simple API transition to formatted header support */
void MK_LogX(MK_U32 aLev, const void* aBuf, MK_U32 aN, const MK_Char* aHdr);

void MK_LogHexBuf(MK_U32 aLev, const MK_U8* aBuf, MK_U32 aLen, MK_U32 aNbrPerLine, MK_Char* aLinePrefix,
                  eLineIndexType aIndexBase, MK_Char* aFirstValPrefix, MK_Char* aValSeparator);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

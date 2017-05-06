#ifndef __MKBase_Time_h__
#define __MKBase_Time_h__

#include <MKBase/Platform.h>
#include <MKBase/Type.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

/*
 * Represents a time (in seconds) as 64-bit signed fixed point value with the
 * decimal point in the middle (much like the NTP time format, but signed).
 */
typedef MK_S64 MK_Time;

/******************************************************************************/

/*
 * Convenience macro for casting to MK_Time type.
 */
#define MK_ToTime(aVal) MK_Cast(MK_Time, aVal)

/******************************************************************************/

/*
 * Min/max values.
 */

#define MK_TIME_MIN MK_S64_MIN
#define MK_TIME_MINU MK_S64_MINU
#define MK_TIME_MAX MK_S64_MAX
#define MK_TIME_MAXU MK_S64_MAXU

/******************************************************************************/

/*
 * Convenience macro for converting a MK_Time to floating-point (double).
 *
 * NOTE: On some embedded platforms use of floating-point should be avoided.
 */
#define MK_Time_ToFP(aTime) ((aTime) / MK_Cast(double, MK_S64C(1) << 32))

/******************************************************************************/

/*
 * Initialize a MK_Time value by converting the time in aTime with the
 * resolution in aRes into the fixed-point MK_Time format (with optional
 * rounding of the result).
 *
 * For example: MK_Time_Init(1500, 1000, MK_FALSE) will return a MK_Time
 * approximation of the value 1.5 seconds.
 */
MK_INLINE MK_Time MK_Time_Init(MK_S64 aTime, MK_U32 aRes, MK_Bool aRnd);

/*
 * Scale the MK_Time value represented by aTime by multiplying with the provided
 * fraction aNum/aDen (with optional rounding of the result).
 *
 * For example: MK_Time_Scale(aTime, 1000, 1, MK_FALSE) will return a
 * millisecond approximation of the value represented by aTime.
 */
MK_INLINE MK_S64 MK_Time_Scale(MK_Time aTime, MK_S64 aNum, MK_U32 aDen, MK_Bool aRnd);

/******************************************************************************/

/*
 * Return a MK_Time representation of the current wall-clock time relative to
 * a fixed point in time. Suitable for wall-clock relative calculations.
 */
MK_INLINE MK_Time MK_Time_Now(void);

/******************************************************************************/
/* CStr INTERFACE EXTENSIONS                                                  */
/******************************************************************************/

/*
 * Interpret aStr as a decimal MK_Time value and return it. If aNext is
 * non-NULL a pointer to the first invalid character is stored there.
 *
 * Accepts leading spaces and sign characters (+-). Overflows in either
 * direction will return the corresponding MIN/MAX value for the type in
 * question.
 */
MK_Time MK_CStr_DecimalToTime(const MK_Char* aStr, MK_Bool aRnd, MK_Char** aNext);

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

#if MK_WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

/******************************************************************************/

MK_INLINE MK_Time MK_Time_Init(MK_S64 aTime, MK_U32 aRes, MK_Bool aRnd)
{
    /*
     * NOTE: Do some trickery to avoid unnecessary overflow and keep
     *       precision when the resolution is high. The original code
     *       looked like this:
     *
     *       (((aTime << 32) + (aRnd ? (aRes >> 1) : 0)) / aRes);
     */
    return (((aTime / aRes) << 32) + ((((aTime % aRes) << 32) + (aRnd ? (aRes >> 1) : 0)) / aRes));
}

MK_INLINE MK_S64 MK_Time_Scale(MK_Time aTime, MK_S64 aNum, MK_U32 aDen, MK_Bool aRnd)
{
    /*
     * NOTE: Do some trickery to avoid unnecessary overflow and keep
     *       precision when the multiplier is high. The original code
     *       looked like this:
     *
     *       (((aTime * aNum) + (aRnd ? (aDen << 31) : 0)) / (aDen << 32));
     */
    return (((aTime >> 32) * aNum) + ((((aTime & 0xffffffff) * aNum) + (aRnd ? ((MK_U64)(aDen) << 31) : 0)) >> 32)) / aDen;
}

/******************************************************************************/

MK_INLINE MK_Time MK_Time_Now(void)
{
    /*
     * TODO: Should make sure we wrap correctly, both when the actual time
     *       counter we are polling wraps and when the MK_Time value wraps.
     */
    #if MK_WIN32
        return MK_Time_Init(GetTickCount(), 1000, MK_FALSE);
    #else
        struct timeval lTNow = {0, 0};
        gettimeofday(&lTNow, 0);
        return MK_ToTime((MK_ToS64(lTNow.tv_sec) << 32) + ((MK_ToS64(lTNow.tv_usec) << 32) / 1000000));
    #endif
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

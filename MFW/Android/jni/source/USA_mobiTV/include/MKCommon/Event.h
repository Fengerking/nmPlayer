#ifndef __MKCommon_Event_h__
#define __MKCommon_Event_h__

#include <MKBase/Platform.h>
#include <MKBase/Assert.h>
#include <MKBase/Type.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

/*
 * Event codes.
 *
 * TODO: Add documentation for specific events.
 */
typedef enum
{
    MK_kEvent_Reset,  /* aArg = NULL */
    MK_kEvent_Flush,  /* aArg = NULL */
    MK_kEvent_Start,  /* aArg = & of a MK_Time */
    MK_kEvent_End,    /* aArg = NULL */
} MK_Event;

/*
 * Get a human-readable CStr for an event code.
 */
MK_INLINE const MK_Char* MK_Event_ToCStr(MK_Event aEvt);

/******************************************************************************/
/* CALLBACKS                                                                  */
/******************************************************************************/

/*
 * Callback function for events. The provided arguments are:
 *
 *     aObj   - A user defined object (set with the callback)
 *     aEvt   - An event code
 *     aArg   - An event-specific argument pointer
 */
typedef void (*MK_EventFunc)(void* aObj, MK_Event aEvt, const void* aArg);

/******************************************************************************/

/*
 * Callback function for buffering state and fullness. The provided arguments
 * are:
 *
 *     aObj   - A user defined object (set with the callback)
 *     aOn    - A flag telling the current buffering on/off state
 *     aFill  - A value in percent specifying buffer fullness (may exceed 100
 *              depending on buffer configuration)
 */
typedef void (*MK_BufferFunc)(void* aObj, MK_Bool aOn, MK_U32 aFill);

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

MK_INLINE const MK_Char* MK_Event_ToCStr(MK_Event aEvt)
{
    switch (aEvt)
    {
        case MK_kEvent_Reset: return "Reset";
        case MK_kEvent_Flush: return "Flush";
        case MK_kEvent_Start: return "Start";
        case MK_kEvent_End: return "End";
        default: return "Unknown";
    }
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

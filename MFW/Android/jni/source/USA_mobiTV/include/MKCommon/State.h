#ifndef __MKCommon_State_h__
#define __MKCommon_State_h__

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
 * TODO: Add documentation for specific states.
 */
typedef enum
{
    MK_kState_Idle,
    MK_kState_Stopped,
    MK_kState_Playing,
} MK_State;

/*
 * TODO: Remove this function? Seems unnecessary now...
 *
 * Test if the state is an "active" state. 
 */
MK_INLINE MK_Bool MK_State_IsActive(MK_State aStat);

/*
 * Get a human-readable CStr for an event code.
 */
MK_INLINE const MK_Char* MK_State_ToCStr(MK_State aStat);

/******************************************************************************/
/* CALLBACKS                                                                  */
/******************************************************************************/

/*
 * Callback function for states. The provided arguments are:
 *
 *     aObj   - A user defined object (set with the callback)
 *     aWas   - A code for the previous state
 *     aIs    - A code for the new state
 */
typedef void (*MK_StateFunc)(void* aObj, MK_State aWas, MK_State aIs);

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

MK_INLINE MK_Bool MK_State_IsActive(MK_State aStat)
{
    return (MK_kState_Playing == aStat);
}

MK_INLINE const MK_Char* MK_State_ToCStr(MK_State aStat)
{
    switch (aStat)
    {
        case MK_kState_Idle: return "Idle";
        case MK_kState_Stopped: return "Stopped";
        case MK_kState_Playing: return "Playing";
        default: return "Unknown";
    }
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

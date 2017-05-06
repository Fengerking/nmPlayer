/*
 * Extends result codes with MKPlayer specific errors.
 */

#ifndef __MKPlayer_Result_h__
#define __MKPlayer_Result_h__

/* TODO: Include all direct base libraries as they are added */
#include <FileMgr/Result.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* MKPlayer ERRORS                                                            */
/******************************************************************************/

/* Category code & helper macros for code creation */

#define MK_C_PLAYER         MK_C(0x800)
#define MK_S_PLAYER(aCode)  MK_S(MK_C_PLAYER, aCode) 
#define MK_E_PLAYER(aCode)  MK_E(MK_C_PLAYER, aCode)

/* Success codes */

/* Error codes */

#define MK_E_PLAYER_FAIL    MK_E_PLAYER(0x000) /* Unspecified failure */
#define MK_E_PLAYER_CRIT    MK_E_PLAYER(0x001) /* Unspecified critical/abortive failure */

/* TODO: Add more errors.. */

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

/*
 * Resolves MKPlayer (and sub-library) result-codes. Returns non-NULL if the
 * result-code was resolved and NULL otherwise.ific code matched).
 */
const MK_Char* MK_Player_GetCategoryDesc(MK_Result aRes, const MK_Char** aName);
const MK_Char* MK_Player_GetResultDesc(MK_Result aRes, const MK_Char** aName);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXMemory.h

	Contains:	memory operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voOMXMemory_H__
#define __voOMXMemory_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <OMX_Types.h>
#include "voCOMXThreadMutex.h"

/**
 * Alloc the memory
 * \param uID [in] The module ID
 * \param uSize [in] The size of memory
 * \return value is alloced memory address. 0 is failed.
 */
OMX_PTR	voOMXMemAlloc (OMX_U32 uSize);

/**
 * free the alloced memory
 * \param uID [in] The module ID
 * \param pMem [in] The address of memory
 * \return value 0 is succeded.
 */
OMX_U32	voOMXMemFree (OMX_PTR pBuffer);

/**
 * free the alloced memory
 * \param uID [in] The module ID
 * \param pMem [in] The address of memory
 * \return value 0 is succeded.
 */
OMX_U32	voOMXMemSet (OMX_PTR pBuff, OMX_U8 uValue, OMX_U32 uSize);

/**
 * free the alloced memory
 * \param uID [in] The module ID
 * \param pMem [in] The address of memory
 * \return value 0 is succeded.
 */
OMX_U32	voOMXMemCopy (OMX_PTR pDest, OMX_PTR pSource, OMX_U32 uSize);

/**
 * free the alloced memory
 * \param uID [in] The module ID
 * \param pMem [in] The address of memory
 * \return value 0 is succeded.
 */
OMX_U32	voOMXMemCheck (OMX_PTR pBuffer, OMX_U32 uSize);

/**
* free the alloced memory
* \param uID [in] The module ID
* \param pMem [in] The address of memory
* \return value 0 is succeded.
*/
OMX_S32	voOMXMemCompare (OMX_PTR pBuffer1, OMX_PTR pBuffer2, OMX_U32 uSize);


OMX_S32 voOMXMemShowStatus (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voOMXMemory_H__

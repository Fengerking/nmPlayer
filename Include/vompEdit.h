/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/


#ifndef __vompEdit_H__
#define __vompEdit_H__

#include "vompType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VOMP_PID_EDIT_BASE					0x700000								/*!< the base param ID for edit features */

/**
 * Media Editor Item 
 */
typedef struct
{
	int		nFlag;				/*!< the source flag */
	void *	pSource;			/*!< the source info */
	int		nAVFlag;			/*!< the Audio and Video enable or not. 0X03 for AV both, 0X02 for auido only, 0x01 video only*/
	int		nStartPos;			/*!< start position */
	int		nStopPos;			/*!< stop position */
	void *	pVideoEffect;		/*!< the video effect */
	void *	pAudioEffect;		/*!< the audio effect */
	int		nReserve;			/*!< the reverse value */
} VOMP_EDITOR_ITEM;

/**
 * Media Editor Source 
 */
typedef struct
{
	int					nCount;		/*!< The item number */
	VOMP_EDITOR_ITEM **	ppItems;	/*!< the items info */
}VOMP_EDITOR_SOURCE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __vompEdit_H__

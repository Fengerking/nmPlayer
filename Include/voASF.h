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

#ifndef __voASF_H__
#define __voASF_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voFile.h>
#include "voSource2.h"

typedef VO_U32 (VO_API * VOASFPACKETCALLBACK)(VO_PBYTE pPacketData, VO_U32 dwPacketSize, VO_PTR pUserData, VO_U32 nReserved);
typedef struct
{
	VOASFPACKETCALLBACK	fCallback;
	VO_PTR				pUserData;
}VO_ASF_PACKETDATA_CALLBACK;
/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_ASF_BASE							0x44100000						/*!< the base param ID for ASF file parser */
#define	VO_PID_ASF_PACKETDATA_CALLBACK			(VO_PID_ASF_BASE | 0x0001)		/*!< <S>set the packet data callback, VO_ASF_PACKETDATA_CALLBACK* */

/**
 * Get File Read API interface
 * \param pReadHandle [out] Return the ASF file read handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetASFReadAPI (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag);

VO_S32 VO_API voGetASF2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voASF_H__

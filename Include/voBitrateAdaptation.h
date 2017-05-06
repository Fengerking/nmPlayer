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

#ifndef __VOBITRATEADAPTATION_H__

#include "voType.h"
#include "voSource2.h"

#define VO_RET_SOURCE2_BA_BITRATECHANGED	( VO_RET_SOURCE2_BA_BASE | 0x0001 )
#define VO_RET_SOURCE2_BA_BITRATEUNCHANGED	( VO_RET_SOURCE2_BA_BASE | 0x0002 )

#define VO_PID_SOURCE2_BA_PARAMBASE				0x29000000
#define VO_PID_SOURCE2_BA_SEEK			( VO_PID_SOURCE2_BA_PARAMBASE | 0x0001 )
#define VO_PID_SOURCE2_BA_CAP					( VO_PID_SOURCE2_BA_PARAMBASE | 0x0002 )
#define VO_PID_SOURCE2_BA_MAXDOWNLOADBITRATE	( VO_PID_SOURCE2_BA_PARAMBASE | 0x0003 )
#define VO_PID_SOURCE2_BA_START_CAP				( VO_PID_SOURCE2_BA_PARAMBASE | 0x0004 )
#define VO_PID_SOURCE2_BA_DISABLECPUBA			( VO_PID_SOURCE2_BA_PARAMBASE | 0x0005 )
#define VO_PID_SOURCE2_BA_MIN_BITRATE_PLAY	( VO_PID_SOURCE2_BA_PARAMBASE | 0x0006 )
#define VO_PID_SOURCE2_BA_SET_CURRENT_BITRATE	(VO_PID_SOURCE2_BA_PARAMBASE | 0x0007)
#define VO_PID_SOURCE2_BA_CPUUSAGEFORTEST		( VO_PID_SOURCE2_BA_PARAMBASE | 0x0013 )
#define VO_PID_SOURCE2_BA_PLAYBACK_SPEED			(VO_PID_SOURCE2_BA_PARAMBASE | 0x0014)
#define VO_PID_SOURCE2_BA_THRESHOLD  			(VO_PID_SOURCE2_BA_PARAMBASE | 0x0015)
#define VO_PID_SOURCE2_BA_BUFFER_DURATION		(VO_PID_SOURCE2_BA_PARAMBASE | 0x0016)

struct VO_SOURCE2_BITRATE_INFOITEM
{
	VO_U32 uBitrate;
	VO_U32 uSelInfo;				//defined in voSource2.h with VO_SOURCE2_SELECT_ prefix
};

struct VO_SOURCE2_BITRATE_INFO 
{
	VO_U32 uItemCount;
	VO_SOURCE2_BITRATE_INFOITEM * pItemList;
};

struct VO_BITRATE_ADAPTATION_API
{
	VO_HANDLE hHandle;	// the handle of the BA instance

/**
 * Create a BA instance
 * \param pHandle [out] BA handle
 */
	VO_U32 (VO_API *Init)(VO_HANDLE* pHandle);
/**
 * Destroy a BA instance
 * \param pHandle [out] BA handle
 */
	VO_U32 (VO_API *Uninit)(VO_HANDLE hHandle);
/**
 * open a BA instance
 * \param pHandle [in] BA handle
 * \param pProgramInfo [in] The program info of the adaptive stream
 */
	VO_U32 ( VO_API *Open)( VO_HANDLE hHandle , VO_SOURCE2_BITRATE_INFO * pBitrateInfo );

/**
 * Close a BA instance
 * \param pHandle [in] BA handle
 */
	VO_VOID ( VO_API *Close)( VO_HANDLE hHandle );

/**
 * Send the info to BA engine
 * \param pHandle [in] BA handle
 * \param uBitrate [in] the bitrate of this stream, the unit should be bits per second
 * \param ullStartTimeStamp [in] the smallest timestamp of this chunk
 */
	VO_VOID ( VO_API *StreamChunkBegin)( VO_HANDLE hHandle , VO_U32 uBitrate , VO_U64 ullStartTimeStamp , VO_BOOL isFormatChanged );

/**
 * Send the info to BA engine
 * \param pHandle [in] BA handle
 * \param uDownloadTime [in] how much time cost for download this chunk
 * \param uDownloadSize [in] the size of the chunk
 * \param ullEndTimeStamp [in] the biggest timestamp of this chunk
 */
	VO_VOID ( VO_API *StreamChunkEnd)( VO_HANDLE hHandle , VO_U32 uDownloadTime , VO_U32 uDownloadSize , VO_U64 ullEndTimeStamp );

/**
 * Send the video delay info to BA engine
 * \param pHandle [in] BA handle
 * \param uDelayTime [in] how much time delayed now, the unit should be ms
 * \param ullPlayTime [in] current ref-clock, the unit should be ms
 */
	VO_VOID ( VO_API *AddVideoDelay)( VO_HANDLE hHandle , VO_S32 uDelayTime , VO_U64 ullPlayTime );

/**
 * Get next proper bitrate from BA engine
 * \param pHandle [in] BA handle
 * \param pBitrate [out] the bitrate should choose for next chunk
 */
	VO_U32 ( VO_API *GetNextBitrate)( VO_HANDLE hHandle , VO_U32 * pBitrate );

	VO_U32 ( VO_API *CheckDownloadSlow)( VO_HANDLE hHandle , VO_U32  nBitrate );

/**
 * Get param from this BA instance
 * \param hHandle [in] BA handle
 * \param uParamID [in] The param ID
 * \param pParam [out] The get value depend on the param ID.
 */
	VO_U32 ( VO_API *GetParam)( VO_HANDLE hHandle , VO_U32 uParamID , VO_PTR pParam );

/**
 * Set param for this BA instance
 * \param hHandle [in] BA handle
 * \param uParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
	VO_U32 ( VO_API *SetParam)( VO_HANDLE hHandle , VO_U32 uParamID , VO_PTR pParam );
};

#endif	//__VOBITRATEADAPTATION_H__
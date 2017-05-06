/********************************************************************
* Copyright 2003 ~ 2012 by VisualOn Software, Inc.
* All modifications are confidential and proprietary information
* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
*********************************************************************
* File Name: 
*            voBufferCommander.h
*
* Project:
* contents/description: all buffer's boss
*            
***************************** Change History**************************
* 
*    DD/MMM/YYYY     Code Ver     Description             Author
*    -----------     --------     -----------             ------
*    13-08-2013        1.0							     Leon Huang
*                                             
**********************************************************************/

#ifndef __voBufferCommander_H__
#define __voBufferCommander_H__

#include <list>

#include "voToolUtility.h"
#include "voLog.h"
#include "voCMutex.h"
#include "voDSType.h"
#include "voDownloadBufferMgr.h"
#include "voSourceBufferManager_AI.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif



/* FB == Final Buffer
   DB == Download Buffer

	---------           ---------
	|  DB   |   ------> |  OB   |
	---------           ---------  
*/	

class voBufferCommander
{
public: 
	voBufferCommander();
	virtual ~voBufferCommander();

public:
	virtual VO_U32 UnInit();
	virtual VO_U32 Init();

public:
	virtual VO_U32 PushData(VO_DATASOURCE_BUFFER_TYPE nBufferType, VO_SOURCE2_TRACK_TYPE nTrackType, VO_PTR pData);
	virtual VO_U32	GetSample( VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample );
	virtual VO_VOID SetPosition( VO_U64 pos );
	virtual VO_U32	GetDuration(VO_BUFFER_PID_TYPE nType, VO_U64 *pDuration);
	virtual VO_VOID Stop();
	virtual VO_VOID Flush();
	virtual VO_U32 ChunkAgentInit(VO_VOID **ppAgent, VO_U32 uASID, VO_BOOL bSwitch);
	virtual VO_U32 ChunkAgentUninit(VO_VOID *pAgent, VO_U32 uASID, CHUNK_STATUS Switch);

	virtual VO_U32 GetParam( VO_U32 nParamID, VO_PTR pParam);
	virtual VO_U32 SetParam( VO_U32 nParamID, VO_PTR pParam); 

private:
	VO_U32 SetFBBufferingStyle(VO_SOURCE2_PROGRAM_TYPE nType);
	VO_U32 SetFBBufferTime(VO_BUFFER_SETTINGS_TYPE type, VO_U32 nTime);
	
private:
	voDownloadBufferMgr m_DownloadBufferMgr;

private:
	voSourceBufferManager_AI * m_pSamplebuffer;
	VO_U32 m_nBufferingTime;
	VO_U32 m_nStartBufferTime;
	VO_U32 m_nMaxBufferTime;
};


#ifdef _VONAMESPACE
}
#endif
#endif  //__voBufferCommander_H__









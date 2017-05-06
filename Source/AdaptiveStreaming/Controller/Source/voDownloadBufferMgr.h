/********************************************************************
* Copyright 2003 ~ 2012 by VisualOn Software, Inc.
* All modifications are confidential and proprietary information
* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
*********************************************************************
* File Name: 
*            voDownloadBufferMgr.h
*
* Project:
* contents/description: a serious of operation func for DownloadBuffer Manger(like I frame align for BA)
*            
***************************** Change History**************************
* 
*    DD/MMM/YYYY     Code Ver     Description             Author
*    -----------     --------     -----------             ------
*    13-08-2013        1.0							     Leon Huang
*                                             
**********************************************************************/

#ifndef __VODOWNLOADBUFFERMGR_H__
#define __VODOWNLOADBUFFERMGR_H__

#include <list>
#include <algorithm>

#include "voToolUtility.h"
#include "voLog.h"
#include "voCMutex.h"
#include "voDSType.h"
#include "CacheManager.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


struct _CHUNKS
{
	VO_BOOL					bBAing; /*!< identify this chunk should be do BA operation, the value will be set at CHUNK_END | CHUNK_SWITCH */
	_CHUNKS					*pBAPairChunk; /*!< identify which previous chunk should be do BA with this chunk, the value will be set at CHUNK_BEGIN | CHUNK_SWITCH */

	VO_U32					uASID;
	VO_PTR					pHandle;	/*each chunk has a handle to save the samples.*/
};

typedef enum{
	SWITCH_FALSE = 0,
	SWITCH_TRUE = 1,
	SWITCH_IGNORE = 2,
	SWITCH_MAX = 0x7fffffff
}CHUNK_STATUS;

class voDownloadBufferMgr
{
public: 
	voDownloadBufferMgr();
	virtual ~voDownloadBufferMgr();

public:
	VO_VOID setCapacity(VO_U32 uDuration) { m_CacheManager.setCapacity(uDuration); };

	VO_U32 PushData(VO_DATASOURCE_BUFFER_TYPE nBufferType, VO_SOURCE2_TRACK_TYPE nTrackType, VO_PTR pData);
	VO_VOID SetFollowingComponent(VO_PTR pComponent);
	VO_U32 ChunkAgentInit(VO_VOID **ppAgent, VO_U32 uASID, VO_BOOL bSwitch);
	VO_U32 ChunkAgentUninit(VO_VOID *pAgent, VO_U32 uASID, CHUNK_STATUS Switch);

	VO_VOID Start();
	VO_VOID Stop();
	VO_VOID Flush();
	VO_VOID Init();
	VO_VOID UnInit();
private:
	VO_U32 PushSampleToChunkGroup(VO_SOURCE2_TRACK_TYPE nType, _SAMPLE  *pSample);

	VO_U32 DoSample(VO_SOURCE2_TRACK_TYPE nType, _SAMPLE *pSample);
	VO_U32 DoTrackTypeUnavailable(VO_SOURCE2_TRACK_TYPE nType, _SAMPLE *pSample);
	VO_U32 DoEOS();
	VO_U32 ClearList();
private:
	_CHUNKS *SaveChunkHandle(VO_U32 uASID, VO_PTR pHandle, _CHUNKS *pBAPairChunk);
	VO_U32 RemoveChunkHandle(_CHUNKS* pChunk);
	_CHUNKS *SearchChunk(_CHUNKS *pChunk);
	_CHUNKS *FindBAChunk();
private:

	std::list<_CHUNKS* > m_ChunksList;
	voCMutex m_LockList;


	CacheManager m_CacheManager;

	VO_BOOL m_bExit;
};


#ifdef _VONAMESPACE
}
#endif
#endif  //__VODOWNLOADBUFFERMGR_H__









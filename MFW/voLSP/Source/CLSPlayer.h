/*
 *  CLSPlayer.h
 *  voCTS
 *
 *  Created by Lin Jun on 4/21/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _CTS_PLAYER_H_
#define _CTS_PLAYER_H_

#include "volspAPI.h"
#include "vompCEngine.h"

class CLSPlayer
{
public:
	CLSPlayer();
	virtual ~CLSPlayer(void);
	
public:
	virtual VO_U32 Open(VO_PLAYER_OPEN_PARAM* pOpenParam);
	virtual VO_U32 Close();
	virtual VO_U32 SetDrawArea(VO_U16 nLeft, VO_U16 nTop, VO_U16 nRight, VO_U16 nBottom);
	virtual VO_U32 SetDataSource(VO_PTR pSource);
	virtual VO_U32 SendBuffer(VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer);
	virtual VO_U32 Run();
	virtual VO_U32 Pause();
	virtual VO_U32 Stop();
	virtual VO_U32 GetStatus(VOMP_STATUS* pStatus);
	virtual VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam);
	virtual VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);
	
protected:
	VO_BOOL	IsPlaying();
	VO_BOOL	IsStopped();
	VO_BOOL	IsPaused();

private:
	vompCEngine*			m_pVome;
	bool					m_bStopped;
};


#endif //_CTS_PLAYER_H_
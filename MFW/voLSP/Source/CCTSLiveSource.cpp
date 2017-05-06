/*
 *  CCTSLiveSource.cpp
 *  voCTS
 *
 *  Created by Lin Jun on 4/22/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CCTSLiveSource.h"

CCTSLiveSource::CCTSLiveSource(VO_U16 type)
:CBaseLiveSource(type)
{
	CCTSLiveSource::LoadLib(NULL);
}

CCTSLiveSource::~CCTSLiveSource(void)
{
	
}

VO_U32 CCTSLiveSource::Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	if(m_LiveSrcFunc.Open)
		return m_LiveSrcFunc.Open(&m_hLiveSrc, pUserData, fStatus, fSendData);
	
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CCTSLiveSource::LoadLib (VO_HANDLE hInst)
{
#ifdef _IOS
	voGetTSLiveSrcAPI(&m_LiveSrcFunc);
	return 0;
#else
	return CBaseLiveSource::LoadLib(hInst);
#endif
}
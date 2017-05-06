/*
 *  CCTSLiveSource.h
 *  voCTS
 *
 *  Created by Lin Jun on 4/22/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */
#ifndef _CTS_LIVE_SRC_H_
#define _CTS_LIVE_SRC_H_

#include "CBaseLiveSource.h"

class CCTSLiveSource : public CBaseLiveSource
{
public:
	CCTSLiveSource(VO_U16 type);
	virtual ~CCTSLiveSource(void);
	
public:
	virtual VO_U32 Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData);
public:
	virtual VO_U32	LoadLib (VO_HANDLE hInst);
};
#endif //_CTS_LIVE_SRC_H_
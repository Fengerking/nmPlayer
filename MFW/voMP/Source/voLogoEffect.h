//
//  voLogoEffect.h
//  vompEngn
//
//  Created by Lin Jun on 12/14/10.
//  Copyright 2010 VisualOn. All rights reserved.
//
#ifndef _VO_LOGO_EFFECT_H_
#define _VO_LOGO_EFFECT_H_

#include "voType.h"
#include "voIVCommon.h"
#include "voVideo.h"

#define MAX_VIDEO_COUNT 8

class voLogoEffect
{
public:
	voLogoEffect();
	virtual ~voLogoEffect();
	
public:
	virtual VO_U32 CheckVideo (VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_FORMAT * pFormat);
	virtual VO_U32 ResetVideo (VO_VIDEO_BUFFER * pOutBuffer);
	
	virtual VO_U32 Flush();

    virtual VO_U32 SetKey(VO_TCHAR* pszKey){return -1;};
    virtual VO_U32 SetLicenseFilePath(VO_TCHAR* pszFile){return -1;};
	virtual VO_U32 SetLicenseContent(VO_TCHAR* pData){return -1;};
    
private:
	int		m_nTimes;
	int		m_nVideoWidth;
	int		m_nVideoHeight;
	
	int		m_xPos;
	int		m_yPos;
	
	int				m_nVideoCount;
	unsigned char*	m_aResetBuffer[MAX_VIDEO_COUNT];
	VO_VIDEO_BUFFER m_aVideoBuffer[MAX_VIDEO_COUNT];
};

class voLogoEffectExt : public voLogoEffect
{
public:
	voLogoEffectExt();
	virtual ~voLogoEffectExt();
	
public:
	virtual VO_U32 CheckVideo (VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_FORMAT * pFormat);
	virtual VO_U32 ResetVideo (VO_VIDEO_BUFFER * pOutBuffer);
	
	virtual VO_U32 Flush();
    
    virtual VO_U32 SetKey(VO_TCHAR* pszKey);
    virtual VO_U32 SetLicenseFilePath(VO_TCHAR* pszFile);
	
    virtual VO_U32 SetLicenseContent(VO_TCHAR* pData);
private:
    VO_TCHAR	m_szLicensePath[1024];
    
    VO_PTR      m_hLicense;
};



#endif
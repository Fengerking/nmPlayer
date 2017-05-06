/************************************************************************
VisualOn Proprietary
Copyright (c) 2003, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
/************************************************************************
* @file CSubtitleHtmlMgr.h
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#ifndef __CSubtitleHtmlMgr_H__
#define __CSubtitleHtmlMgr_H__

#include "CSubtitleMgr.h"
#include "voIVCommon.h"
#include "CCharsetDecoder.h"
#include "voSubtitleParser.h"

class	CSubtitleHtmlMgr : public CSubtitleMgr
{
public:
	CSubtitleHtmlMgr(void);
	virtual ~CSubtitleHtmlMgr(void);

protected:
	int m_nCurrStartTime;
	int m_nCurrEndTime;
	bool m_bEnd;

	//for interface of subtitle
	voSubtitleInfo				m_subtitleInfo;
	voSubtitleInfo				m_subNULL;
	voSubtitleInfoEntry			m_subtitleInfoEntry;
	voSubtitleInfoEntry		m_subNULLEntry;

	VO_CHAR m_szBreak[4];

	int  m_nStartTimeLastTime;
	int  m_nEndTimeLastTime;

	int  m_nCount;


protected:
	bool IsInRange(int nPos, int nStartTime, int nEndTime);
	void FilterSpecialCharacter(VO_CHAR* pText, int* pnLen);
	bool IsNumberChar(VO_CHAR cChar);

	void CleareSubtitleInfo();
	voSubtitleTextRowInfo* AddNewRow();
	voSubtitleTextInfoEntry* AddNewTextInfoEntry(voSubtitleTextRowInfo* row);
	TT_Style*				GetTTStyle(char* styleName,bool bExact = false);
	voSubtitleInfo* GetCurrentTtmlText( CBaseSubtitleTrack* pTrack, CTextItem * pTextItem,VO_S64 uPos = -1);
	voSubtitleInfo* GetCurrentWebVTTText( CBaseSubtitleTrack* pTrack, CTextItem * pTextItem);
	voSubtitleInfo* GetCurrentNormalText(CBaseSubtitleTrack* pTrack, CTextItem * pTextItem);
	voSubtitleInfo* GetCurrentSMPTETtmlText( CBaseSubtitleTrack* pTrack, CTextItem * pTextItem,VO_S64 uPos = -1);
public:
	virtual int SetMediaFile(VO_TCHAR* pFilePath);
	bool IsChanged(int nPos);

	void Init(int nWidth = -1, int nHeight = -1);

	void SetJniEnv(void* env){m_jniEnv = env;};

	voSubtitleInfo* GetCurrentText(int nPos);
	virtual bool GetTrackInfo( VOOSMP_SUBTITLE_LANGUAGE * pTrackInfo, int nIndex);
	VO_S32 TranslateFontSizeTTML(VO_S32 uFontSize,VO_S32 ParentSize = -1);
};

#endif // __CSubtitleHtmlMgr_H__

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
* @file CSubtitleMgr.h
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#ifndef __CSubtitleMgr_H__
#define __CSubtitleMgr_H__

#define	MAX_TRACK_COUNT	64
#include "CBaseSubtitleParser.h"

#include "voType.h"
#include "voString.h"
#include "voSubtitleType.h"
#include "CPtrList.h"


class	CSubtitleMgr
{
public:
	CSubtitleMgr(void);
	virtual ~CSubtitleMgr(void);

protected:
	int			 m_nTrackCount;
	unsigned int m_TrackList[MAX_TRACK_COUNT];
	void*						m_jniEnv;

protected:
	void		GetTracksFromParser(CBaseSubtitleParser* pParser);

	bool		IsFileExist(VO_TCHAR* pFile, VO_TCHAR* pExt, bool* pbLangPreFile);

public:
	virtual	bool Init() {return true;}
	virtual bool Uninit() {return true;}
	void     	 Clear();

	virtual int  SetMediaFile(VO_TCHAR* pFilePath);
	virtual int  SetInputData ( VO_CHAR * pData, int len,int nType, VO_S64 llBaseTime);
	virtual bool AddSubtitleFile(VO_TCHAR* pFilePath);

	virtual int	 GetTrackCount() { return m_nTrackCount;}
	virtual void	 SelectTrackByIndex(int nTrackIndex){n_nTrackIndex = nTrackIndex;};
	virtual bool GetTrackInfo( VOOSMP_SUBTITLE_LANGUAGE * pTrackInfo, int nIndex){return false;};

	virtual	CBaseSubtitleTrack* GetTrack(int nTrackIndex);

	virtual CObjectList<TT_Style>* GetStyleListOfTTML(){return &m_styleList;};

	virtual Subtitle_Track_Type	GetTrackType(int nTrackIndex);
	virtual voSubtitleCodingType GetParserType(){return m_Parser_Type;};

protected://interface
	virtual	bool		SetPos(int nPos, int nTrackIndex = -1);
	virtual VO_CHAR*	GetCurrSubtitleItem(int nTrackIndex, VO_CHAR* o_szTextLine1, VO_CHAR* o_szTextLine2, VO_CHAR* o_szTextLine3, VO_CHAR*  o_szTextLine4,VO_CHAR* o_szTextLine5,VO_CHAR*  o_szTextLine6,  VO_CHAR*  o_szTextLine7,VO_CHAR* o_szTextLine8,VO_CHAR*  o_szTextLine9,int* o_StartTime, int* o_EndTime, int* o_Count, bool* o_IsEnd);	
	virtual VO_CHAR*	GetSubtitleItem(int nTrackIndex, VO_CHAR* o_szTextLine1, VO_CHAR* o_szTextLine2, VO_CHAR* o_szTextLine3, VO_CHAR* o_szTextLine4,VO_CHAR* o_szTextLine5,VO_CHAR*  o_szTextLine6, VO_CHAR* o_szTextLine7,VO_CHAR* o_szTextLine8,VO_CHAR*  o_szTextLine9, int* o_StartTime, int* o_EndTime, int* o_Count, bool* o_IsEnd);	

protected:
	CObjectList<TT_Style>  m_styleList;
	char		m_strBodyStyle[64];
	char		m_strDivStyle[64];
	int			n_nTrackIndex;
	voSubtitleCodingType m_Parser_Type;
	bool			m_bIsFounded;
	VO_S64			m_llBaseTime;

};

#endif // __CSubtitleMgr_H__

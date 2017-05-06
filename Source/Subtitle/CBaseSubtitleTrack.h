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
* @file CBaseSubtitleTrack.h
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#ifndef __CBaseSubtitleTrack_H__
#define __CBaseSubtitleTrack_H__

#define	TEXT_ITEM_COUNT	9

#include "voSubtitleParser.h"

#include "voType.h"
#include "voString.h"

typedef struct
{
	VO_CHAR		uCodeingType[10];
	VO_U32		uImaWidth;///<image width
	VO_U32		uImaHeight;///<image Height
	VO_U32		uXco;///<image x coodinate
	VO_U32		uYco;///<image y coodinate
	VO_PCHAR	pBuf;///<image data buffer
	VO_U32		uBufSize;///<image data buffer size
}ImageInfo;

class CBufferItem
{
public:
	VO_PBYTE	m_pBuffer;
	CBufferItem *	m_pNext;
};

const VO_U32 MAX_LANGUAGE_STRING_BUFFER_SIZE = 100;
class CTextItem
{
public:
	CTextItem(void)
	{
		for (int i = 0; i < TEXT_ITEM_COUNT; i++)
			m_aText[i] = NULL;
		m_nColor = 0XFFFFFF;
		m_pImage = NULL;
	}
	virtual ~CTextItem(void)
	{
		for (int i = 0; i < TEXT_ITEM_COUNT; i++)
		{
			if (m_aText[i] != NULL)
			{
				delete m_aText[i];
				m_aText[i] = NULL;
			}
		}
		if (m_pImage)
		{
			if (m_pImage->pBuf)
			{
				delete m_pImage->pBuf;
			}
			delete m_pImage;
		}
		
	}

public:
	int			m_nStartTime;
	int         m_nEndTime;
	int			m_nColor;
	//add for webvtt tag
	int				m_nVertical;		//0:horizontal	1:vertical
	int				m_nLine;		//percent of Line, now ignore line number,only focuc on line percent
	int				m_nPosition;	//start percent of a line
	int				m_nSize;		//percent of the frame to show text
	int				m_nAlign;		//0:start 1:end 2:middle 3:left 4:right

	char *		m_aText[TEXT_ITEM_COUNT];
	ImageInfo*		m_pImage;
	CTextItem *	m_pNext;
};





class	CBaseSubtitleTrack
{
public:
	CBaseSubtitleTrack(void);
	virtual ~CBaseSubtitleTrack(void);

	virtual	Subtitle_Track_Type	GetTrackType() {return m_nTrackType;}
	virtual	void				SetTrackType( Subtitle_Track_Type nTrackType) { m_nTrackType = nTrackType;}

	
	virtual	VO_CHAR*			GetCurrSubtitleItem(VO_CHAR* o_szTextLine1, VO_CHAR* o_szTextLine2, VO_CHAR* o_szTextLine3,  VO_CHAR* o_szTextLine4,VO_CHAR* o_szTextLine5,  VO_CHAR* o_szTextLine6, VO_CHAR* o_szTextLine7,VO_CHAR* o_szTextLine8,  VO_CHAR* o_szTextLine9,int* o_StartTime, int* o_EndTime, int* o_Count,  bool* o_IsEnd) ;
	virtual	VO_CHAR*			GetSubtitleItem(VO_CHAR* o_szTextLine1, VO_CHAR* o_szTextLine2, VO_CHAR* o_szTextLine3,  VO_CHAR* o_szTextLine4, VO_CHAR* o_szTextLine5,  VO_CHAR* o_szTextLine6,
		 VO_CHAR* o_szTextLine7, VO_CHAR* o_szTextLine8,  VO_CHAR* o_szTextLine9, int* o_StartTime, int* o_EndTime, int* o_Count,  bool* o_IsEnd) ;
	virtual	VO_CHAR* GetCurSubtitleItem();
	virtual Subtitle_Language	GetLanguage();
	virtual VO_PCHAR			GetLanguageStr(){return m_chLang;}
	virtual bool				SetLanguage(Subtitle_Language nLanguage, bool bForce = false);
	virtual VO_BOOL				SetLanguageString(VO_PCHAR chLang);
	virtual Subtitle_File_Format	GetFileFormat();
	virtual bool					SetFileFormat(Subtitle_File_Format nFileFormat);

	virtual	CTextItem*			GetNewTextItem();
	virtual	bool				DropLastTextItem();
	virtual	bool				FinishAddNewTextItem();

	// operate current text item
	virtual	VO_CHAR*			AddCurrTextItemText(char* szText);
	virtual	VO_BOOL				AddCurrTextItemImage(ImageInfo* pImageInfo, VO_BOOL bBase64En);
	virtual	bool				SetCurrTextItemStartTime(int nStartTime);
	virtual	bool				SetCurrTextItemEndTime(int nEndTime);
	virtual	bool				SetCurrTextItemColor(int nColor);
	//add for webvtt tag
	virtual bool				SetCurrTextItemVertical(int vertical);
	virtual bool				SetCurrTextItemLine(int line);
	virtual bool				SetCurrTextItemPosition(int position);
	virtual bool				SetCurrTextItemSize(int size);
	virtual bool				SetCurrTextItemAlign(int align);

	virtual CTextItem*			GetCurrTextItem() { return m_pNewTextItem; }
	virtual	VO_CHAR*			GetCurrTextItemLastText();
	virtual	bool				ClearCurrTextItem();
	virtual	bool				CloneCurrTextItem(CTextItem* io_pTextItem);
	virtual	bool				CloneAndCommitCurrTextItem(CTextItem* io_pTextItem);

	virtual	bool				SetPos(int nPos);
	virtual bool				FillEndTime();
	virtual bool				SmartLangCheck();

	virtual int					IsSameTag(char* szTag);
	virtual void				SetTag(char* szTag);
	virtual char*				GetTag(){return m_szTag;};
	void						AppendTrackData(CBaseSubtitleTrack* pTrack);
	virtual int					GetDuration(){return m_nDuration;}
	virtual int					GetLastBegin(){return m_nLastBegin;}
	
protected:
	virtual	CBufferItem*		CreateNewBuffer();
	virtual	void				Release();

	inline	bool				IsEnglishChar(VO_CHAR cChar);
	inline	bool				IsNumberChar(VO_CHAR cChar);
	inline	void				FilterSpecialChar(VO_CHAR* pText);

	virtual	void				FixEndTimeForWrongTime(CTextItem* pWrongItem, int nTime, CTextItem* pLastItem);
	
protected:
	Subtitle_Track_Type	m_nTrackType;
	Subtitle_Language	m_nLanguage;
	Subtitle_File_Format m_nFileFormat;

	CBufferItem*		m_pCurItem;
	int					m_nCurrIndex;

	CBufferItem*		m_pFirstItem;

	CTextItem*			m_pNewTextItem;
	int					m_nNewTextItemIndex;

	bool				m_bNewItemDropped;

	char				m_szTag[32];
	int				m_nDuration;
	int				m_nLastBegin;	//used for subtitle which has no endtime
	VO_CHAR			m_chLang[MAX_LANGUAGE_STRING_BUFFER_SIZE];
	
};

#endif // __CBaseSubtitleTrack_H__

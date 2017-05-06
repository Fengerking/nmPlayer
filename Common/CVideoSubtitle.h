	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoSubtitle.h

	Contains:	CVideoSubtitle header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-11-30		JBF			Create file

*******************************************************************************/

#ifndef __CVideoSubtitle_H__
#define __CVideoSubtitle_H__

#include "CDllLoad.h"
#include "voSourceSubtitleDataBuffer.h"
#include "voCaptionParser.h"
#include "voSubtitleFunc.h"
#include "subtitle3API.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef enum
{
	VOSUB_FLAG_CLOSECAPTION				= 0X00000001,		/*!<Indicate the buffer is Close Caption */
	VOSUB_FLAG_EXTERNIMAGE				= 0X00000002,		/*!<Indicate the buffer is extern Image */
	VOSUB_FLAG_EXTERNTEXT				= 0X00000004,		/*!<Indicate the buffer is extern Text */
	VOSUB_FLAG_STREAM_EOS				= 0X00010000,		/*!<Indicate the buffer is End of Stream */
	VOSUB_FLAG_NEW_STREAM				= 0X00020000,		/*!<Indicate the buffer is New of Stream */
}VOSUB_BUFFER_FLAG;

#define VOSUB_GETIMAGE_NUMBER			0x000000001

class CVideoSubtitle : public CDllLoad
{
public:
	CVideoSubtitle ();
	virtual ~CVideoSubtitle (void);

	virtual VO_U32		Init (unsigned char* pBuffer, int nLength, int nType);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		SetInputData (unsigned char* pBuffer, int nLength, int nTime, int nFlag);
	virtual VO_U32		GetOutputData ( voSubtitleInfo *pSample);
	virtual VO_U32		GetOutputImage ( voSubtitleInfo *pSample);
	virtual VO_U32		Flush (void);
	virtual VO_U32		Enable (VO_BOOL bEnable);

	virtual VO_U32		ReSet();

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		GetLanguageNum(VO_U32 *pNum);
	virtual VO_U32		GetLanguageItem(VO_U32 nNum, VO_CAPTION_LANGUAGE** pLangage);
	virtual VO_U32		SelectLanguage(VO_U32 nLangNum);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:	
	VO_U32 AddSubtitle( voSubtitleInfo * pSample );
	VO_U32 AddImage( voSubtitleInfo * pSample );
	VO_U32 HandleCC(unsigned char* pBuffer, int nLength, int nTime, int nFlag);
	VO_U32 HandleImage(unsigned char* pBuffer, int nLength, int nTime, int nFlag);
	VO_U32 HandleText(unsigned char* pBuffer, int nLength, int nTime, int nFlag);

	VO_U32	UpdateLanguage();
	VO_U32	UnintLanguage();

protected:

	VO_U32					m_nType;
	vo_allocator< VO_BYTE > m_alloc;

	voSubtitleInfo*			m_pSubInfo;

	voCMutex				m_Lock;

	voSourceSubtitleDataBuffer m_Imagetitle;

	voSourceSubtitleDataBuffer m_subtitle;
	VO_CAPTION_PARSER_API	m_ccApi;
	VO_PTR					m_hCC;
	VO_BOOL					m_bIsccloaded;
	VO_BOOL					m_bCCParser;

	VO_SUBTITLE_3_API		m_sSubTiltleAPI;
	void*					m_hSubhandle;
	int						m_nSubTitleNum;
	int						m_nSubTitleIndex;

	int						m_nLanuageGot;
	VO_CAPTION_LANGUAGE_INFO m_sLanguageInfo;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CVideoEffect_H__

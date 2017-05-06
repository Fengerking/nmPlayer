#ifndef __COSCloseCaption_H__
#define __COSCloseCaption_H__

#include "voOnStreamType.h"
#include "voOnStreamSource.h"
#include "voSubtitleType.h"
#include "voSubtitleParser.h"
#include "voSource2.h"
#include "COSBaseSubtitle.h"
#include "voCaptionParser.h"
#include "voSourceSubtitleDataBuffer.h"
#include "voSubtitleFunc.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef VO_S32 (VO_API* pvoGetCaptionParserAPI)(VO_CAPTION_PARSER_API * pParser);

typedef enum
{
	VOCC_FLAG_STREAM_EOS				= 0X00010000,		/*!<Indicate the buffer is End of Stream */
	VOCC_FLAG_NEW_STREAM				= 0X00020000,		/*!<Indicate the buffer is New of Stream */
}VOCLOSECAPTION_BUFFER_FLAG;


class COSCloseCaption : public COSBaseSubtitle
{
public:
	COSCloseCaption (VO_SOURCE2_LIB_FUNC *pLibop, VO_TCHAR* pWorkPath = NULL);
	virtual ~COSCloseCaption (void);

	virtual int				Init(unsigned char* pBuffer, int nLength, int nType);
	virtual int				Uninit(void);
	virtual int				SetInputData(VO_CHAR * pData, int len, int nTime,int nType);
	virtual int				GetSubtitleInfo(long nTimeCurrent, voSubtitleInfo ** subtitleInfo);
	virtual int				GetTrackCount ();
	virtual int				GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** ppSubLangInfo);
	virtual int				GetTrackInfo(int nIndex, VOOSMP_SUBTITLE_LANGUAGE ** ppTrackInfo);
	virtual int				SelectTrackByIndex(int nIndex);

	virtual int 			SetCurPos (long long *pCurPos);
	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);

protected:
	virtual int				LoadDll();
	virtual int				unLoadDll();
	virtual int				UpdateLanguage();

protected:
	vo_allocator< VO_BYTE >		m_alloc;

	VO_CAPTION_PARSER_API		m_szCloseCaptionAPI;
	voSourceSubtitleDataBuffer	m_subtitle;

	voSubtitleInfo *			m_pSubInfo;
	voSubtitleInfo				m_sCCInfo;

	voCMutex					m_mtLanguageInfo;
};

#ifdef _VONAMESPACE
}
#endif

#endif // __COSCloseCaption_H__

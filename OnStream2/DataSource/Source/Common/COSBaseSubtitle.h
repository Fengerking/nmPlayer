#ifndef __COSBaseSubTitle_H__
#define __COSBaseSubTitle_H__

#include "voOnStreamType.h"
#include "voOnStreamSource.h"
#include "voSubtitleType.h"
#include "voSubtitleParser.h"
#include "voSource2.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef enum
{
	VOOS_SUBTITLE_NONE = 0,
	VOOS_SUBTITLE_TIMETEXT		=  0x00000001,
	VOOS_SUBTITLE_DVBT			=  0x00000002,
	VOOS_SUBTITLE_CLOSECAPTION	=  0x00000004,

	VOOS_SUBTITLE_MAX = 0X7FFFFFFF   /*!<Max value definition */
}VOOS_SUBTITLE_TYPE;

class COSBaseSubtitle
{
public:
	COSBaseSubtitle (VO_SOURCE2_LIB_FUNC *pLibop, VO_TCHAR* pWorkPath = NULL);
	virtual ~COSBaseSubtitle (void);

	virtual int				Init(unsigned char* pBuffer, int nLength, int nType);
	virtual int				Uninit(void);
	virtual int				SetTimedTextFilePath(VO_TCHAR * pPath, int nType);
	virtual int				SetInputData(VO_CHAR * pData, int len, int nTime, int nType);
	virtual int				GetSubtitleInfo(long nTimeCurrent, voSubtitleInfo ** subtitleInfo);
	virtual int				GetTrackCount ();
	virtual int				GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** ppSubLangInfo);
	virtual int				GetTrackInfo(int nIndex, VOOSMP_SUBTITLE_LANGUAGE ** ppTrackInfo);
	virtual int				SelectTrackByIndex(int nIndex);

	virtual int 			SetCurPos (long long *pCurPos);
	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);

	virtual int 			GetSubTitleNum();

	virtual int				GetSubtitleType() { return m_nSubtitlteType;}

protected:
	virtual int				LoadDll();
	virtual int				unLoadDll();
	virtual int				UpdateLanguage();
	virtual int				ClearLanguage();

	VO_SOURCE2_LIB_FUNC*	m_pLibop;
	VO_LOG_PRINT_CB*		m_pbVOLOG;

	voCMutex				m_Lock;

	void*					m_hDll;
	void*					m_hSubtitle;

	int							m_nSubLangGot;
	VOOSMP_SUBTITLE_LANGUAGE_INFO m_sSubLangInfo;

	int						m_nSubTitleNum;
	int						m_nTrackNum;
	int						m_nTrackIndex;

	int						m_nSubtitlteType;
	VO_TCHAR				m_szWorkPath[512];
};

#ifdef _VONAMESPACE
}
#endif

#endif // __COSBaseSubTitle_H__

#ifndef __COSTimeText_H__
#define __COSTimeText_H__

#include "voOnStreamType.h"
#include "voOnStreamSource.h"
#include "voSubtitleType.h"
#include "voSubtitleParser.h"
#include "voSource2.h"
#include "COSBaseSubtitle.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class COSTimeText : public COSBaseSubtitle
{
public:
	COSTimeText (VO_SOURCE2_LIB_FUNC *pLibop, VO_TCHAR* pWorkPath = NULL);
	virtual ~COSTimeText (void);

	virtual int				Init(unsigned char* pBuffer, int nLength, int nType);
	virtual int				Uninit(void);
	virtual int				SetTimedTextFilePath(VO_TCHAR * pPath, int nType);
	virtual int				SetInputData(VO_CHAR * pData, int len, int nTime,int nType);
	virtual int				GetSubtitleInfo(long nTimeCurrent, voSubtitleInfo ** subtitleInfo);
	virtual int				GetTrackCount ();
	virtual int				GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** ppSubLangInfo);
	virtual int				GetTrackInfo(int nIndex, VOOSMP_SUBTITLE_LANGUAGE ** ppTrackInfo);
	virtual int				SelectTrackByIndex(int nIndex);

	virtual int 			SetCurPos (long long *pCurPos);
	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);

	virtual int 			GetSubTitleNum();

protected:
	virtual int				LoadDll();
	virtual int				unLoadDll();
	virtual int				ClearLanguage();

	VO_SUBTITLE_PARSER		m_szSubParserAPI;
};

#ifdef _VONAMESPACE
}
#endif

#endif // __COSTimeText_H__

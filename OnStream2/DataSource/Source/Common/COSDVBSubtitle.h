#ifndef __COSDVBSubtitle_H__
#define __COSDVBSubtitle_H__

#include "voOnStreamType.h"
#include "voOnStreamSource.h"
#include "voSubtitleType.h"
#include "voSubtitleParser.h"
#include "voSource2.h"
#include "COSBaseSubtitle.h"
#include "voDVBSubtitleParser.h"
#include "voSourceSubtitleDataBuffer.h"
#include "voSubtitleFunc.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef VO_S32 (VO_API* pvoGetDVBSubtitleAPI)(VO_DVB_PARSER_API * pParser);

class COSDVBSubtitle : public COSBaseSubtitle
{
public:
	COSDVBSubtitle (VO_SOURCE2_LIB_FUNC *pLibop, VO_TCHAR* pWorkPath = NULL);
	virtual ~COSDVBSubtitle (void);

	virtual int				Init(unsigned char* pBuffer, int nLength, int nType);
	virtual int				Uninit(void);
	virtual int				SetInputData(VO_CHAR * pData, int len, int nTime,int nType);
	virtual int				GetSubtitleInfo(long nTimeCurrent, voSubtitleInfo ** subtitleInfo);

	virtual int 			SetCurPos (long long *pCurPos);
	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);

protected:
	virtual int				LoadDll();
	virtual int				unLoadDll();

	VO_DVB_PARSER_API		m_szDVBSubtitleAPI;

	voSubtitleInfo			m_sDVBSubInfo;

	vo_allocator< VO_BYTE > m_alloc;
	voSourceSubtitleDataBuffer m_subtitle;
};

#ifdef _VONAMESPACE
}
#endif

#endif // __COSDVBSubtitle_H__

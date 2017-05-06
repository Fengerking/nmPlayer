#pragma once

#include "CBaseStreamFileTrack.h"
#include "CGFileChunk.h"
#include "CRealParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define read_pointer2(p, l)\
{\
	MemCopy(p, pSrc, l);\
	pSrc += l;\
}

#define skip2(l)\
{\
	pSrc += l;\
}

#define read_byte2(b)\
{\
	read_pointer2(&b, 1);\
}

#define read_word2(w)\
{\
	tp = (VO_PBYTE)&w;\
	read_pointer2(tp + 1, 1);\
	read_pointer2(tp, 1);\
}

#define read_dword2(dw)\
{\
	tp = (VO_PBYTE)&dw;\
	read_pointer2(tp + 3, 1);\
	read_pointer2(tp + 2, 1);\
	read_pointer2(tp + 1, 1);\
	read_pointer2(tp, 1);\
}

#define read_fcc2(dw)\
{\
	read_pointer2(&dw, 4);\
}

class CRealReader;
class CRealTrack : public CBaseStreamFileTrack
{
	friend class CRealReader;
public:
	CRealTrack(VO_TRACKTYPE nType, PRealTrackInfo pTrackInfo, CRealReader* pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CRealTrack();

public:
	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetInfo(VO_SOURCE_TRACKINFO* pTrackInfo);
	virtual VO_U32		FileIndexGetFilePosByTime(VO_S64 llTimeStamp);
	virtual VO_U32		FileIndexGetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp);
	virtual VO_U64		FileIndexGetFilePosByIndex(VO_U32 dwFileIndex);

	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);

	virtual VO_U32		GetDataOffset() {return m_dwDataOffset;}

	virtual VO_VOID		SetThumbnailFlag(VO_BOOL bThumbnail);
protected:
	virtual VO_U32		SetPosN(VO_S64* pPos);

	virtual VO_BOOL		OnPacket(CGFileChunk* pFileChunk, VO_U8 btFlags, VO_U16 wLen, VO_U32 dwTimeStamp) = 0;
	virtual VO_BOOL		ReadIndexFromFile(CGFileChunk* pFileChunk, PRealFileIndexInfo pIndexInfo);

	virtual VO_U32		Unprepare();

protected:
	CRealReader*		m_pReader;

	VO_PBYTE			m_pInitParam;	//VORV_FORMAT_INFO* for video or VORA_INIT_PARAM* for audio
	VO_U32				m_dwInitParamSize;

	VO_U32				m_dwBitrate;
	VO_U32				m_dwDataOffset;

	//index
	VO_U32				m_dwSeekPoints;
	PRealSeekPoint		m_pSeekPoints;

	VO_BOOL				m_IsThumbNail;
};

#ifdef _VONAMESPACE
}
#endif
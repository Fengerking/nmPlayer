#pragma once
#include "voYYDef_filcmn.h"
#include "CBaseReader.h"
#include "CBaseStreamFileTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CBaseStreamFileReader :
	public CBaseReader
{
public:
	CBaseStreamFileReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CBaseStreamFileReader();

public:
	virtual VO_VOID		FileGenerateIndex();

	//if at least one track buffer full, return VO_TRUE
	VO_BOOL				IsTrackBufferFull();

	virtual VO_VOID		OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser);
	virtual VO_VOID		OnDelDataParser(CBaseTrack* pRqstTrack);

	virtual CvoFileDataParser*	GetFileDataParserPtr() {return m_pFileDataParser;}
	virtual CMemPool*			GetMemPoolPtr() {return &m_memPool;}

	virtual VO_VOID		Flush();

protected:
	virtual VO_BOOL		IsCanGenerateIndex();

protected:
	CvoFileDataParser*			m_pFileDataParser;

	CGFileChunk					m_chunkUseForDataParser;
	CMemPool					m_memPool;
};

#ifdef _VONAMESPACE
}
#endif

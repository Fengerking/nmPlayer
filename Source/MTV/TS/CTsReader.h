#pragma once

#include "voYYDef_TS.h"
#include "voParser.h"
#include "CMTVReader.h"
#include "CTsFileDataParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class CTsTrack;
class CTsReader : public CMTVReader
{
public:
	CTsReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CTsReader(void);

	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Close();
	virtual VO_U32		GetInfo(VO_SOURCE_INFO* pSourceInfo);
	virtual VO_U32		MoveTo(VO_S64 llTimeStamp);
    virtual VO_VOID		Flush();
	virtual VO_U32		SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);
    virtual VO_U32              SetSelectTrack(VO_U32 nIndex, VO_BOOL bInUsed, VO_U64 llTs);
	CTsTrack* GetTrackByStreamNum(VO_U32 btStreamNum);

public:
	virtual VO_VOID		OnData(VO_PTR pData);
	virtual VO_VOID		OnBlock(VO_PTR pBlock);


private:
	CTsFileDataParser	m_FileDataParser;
	VO_BOOL				m_bTest;
};

#ifdef _VONAMESPACE
}
#endif


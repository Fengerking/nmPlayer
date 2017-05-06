#ifndef __CMp4Reader_H__
#define __CMp4Reader_H__

#include "CBaseReader.h"
#include "CMp4Track.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define g_dwFRModuleID	VO_INDEX_SRC_MP4

class CMp4Reader :
	public CBaseReader
{
public:
	CMp4Reader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CMp4Reader();

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Close();
};
#ifdef _VONAMESPACE
}
#endif
#endif	//__CMp4Reader_H__

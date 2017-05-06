#include "CMp4Reader.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
CMp4Reader::CMp4Reader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseReader(pFileOp, pMemOp, pLibOP, pDrmCB)
{
}

CMp4Reader::~CMp4Reader()
{
	Close();
}

VO_U32 CMp4Reader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	Close();

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CMp4Reader::Close()
{
	UnprepareTracks();
	UninitTracks();

	return VO_ERR_NOT_IMPLEMENT;
}
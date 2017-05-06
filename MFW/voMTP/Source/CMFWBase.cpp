#include "CMFWBase.h"
#include "cmnMemory.h"


CMFWBase::CMFWBase(void)
{
	cmnMemSet(0, &m_OpenParam, 0, sizeof(MFW_OPEN_PARAM));
}

CMFWBase::~CMFWBase(void)
{
}

VO_U32	CMFWBase::OutputData(MFW_OUTPUT_BUFFER* pBuf)
{
	if (m_OpenParam.fOutputData)
	{
		m_OpenParam.fOutputData(m_OpenParam.pUserData, pBuf);
	}

	return VO_ERR_MTP_OK;
}

VO_U32 CMFWBase::OnTrackInfo(VO_LIVESRC_TRACK_INFO* pTrackInfo)
{
	return VO_ERR_MTP_NOT_IMPLEMENT;
}

VO_VOID	CMFWBase::Flush()
{

}



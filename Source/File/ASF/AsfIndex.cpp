#include "AsfIndex.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

CAsfIndexEntries::CAsfIndexEntries(VO_MEM_OPERATOR* pMemOp)
: CvoBaseMemOpr(pMemOp)
, m_ullTimeInterval(0)
, m_dwEntries(0) 
{
}

VO_U32 CAsfIndexEntries::GetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp)
{
	return bForward ? static_cast<VO_U32>( (llTimeStamp + m_ullTimeInterval - 1) / m_ullTimeInterval ) : static_cast<VO_U32>(llTimeStamp / m_ullTimeInterval);
}

CAsfIndexObjectEntries::CAsfIndexObjectEntries(VO_MEM_OPERATOR* pMemOp)
: CAsfIndexEntries(pMemOp)
, m_pEntries(NULL)
{
}

CAsfIndexObjectEntries::~CAsfIndexObjectEntries()
{
	if (m_pEntries)
		SAFE_MEM_FREE(m_pEntries);
}

VO_U64 CAsfIndexObjectEntries::GetPacketPosByTime(VO_S64 llTimeStamp)
{
	VO_U32 dwIndex = static_cast<VO_U32>(llTimeStamp / m_ullTimeInterval);
	return (dwIndex < m_dwEntries) ? m_pEntries[dwIndex] : m_pEntries[m_dwEntries - 1];
}

VO_U64 CAsfIndexObjectEntries::GetPacketPosByIndex(VO_U32 dwIndex)
{
	return (dwIndex < m_dwEntries) ? m_pEntries[dwIndex] : -1;
}

VO_S64 CAsfIndexObjectEntries::GetPrevKeyFrameTime(VO_S64 llTimeStamp)
{
	VO_U32 dwStart = static_cast<VO_U32>(llTimeStamp / m_ullTimeInterval);
	if (dwStart >= m_dwEntries)
		dwStart = m_dwEntries - 1;

	VO_U32 dwIndex = dwStart;
	for(VO_U32 i = dwStart; i > 0; i--)
	{
		if(m_pEntries[i - 1] != m_pEntries[i])
		{
			dwIndex = i;
			break;
		}
	}

	return dwIndex * m_ullTimeInterval;
}

VO_S64 CAsfIndexObjectEntries::GetNextKeyFrameTime(VO_S64 llTimeStamp)
{
	VO_U32 dwStart = static_cast<VO_U32>(llTimeStamp / m_ullTimeInterval);
	VO_U32 dwIndex = VO_MAXU32;
	for(VO_U32 i = dwStart; i < m_dwEntries - 1; i++)
	{
		if(m_pEntries[i + 1] != m_pEntries[i])
		{
			dwIndex = i + 1;
			break;
		}
	}

	return (VO_MAXU32 == dwIndex) ? VO_MAXS64 : (dwIndex * m_ullTimeInterval);
}

VO_U32 CAsfIndexObjectEntries::SetEntriesNum(VO_U32 dwEntries)
{
	m_dwEntries = dwEntries;

	m_pEntries = NEW_OBJS(VO_U32, m_dwEntries);
	if(!m_pEntries)
		return VO_ERR_OUTOF_MEMORY;

	return VO_ERR_NONE;
}




CAsfSampleIndexObjectEntries::CAsfSampleIndexObjectEntries(VO_MEM_OPERATOR* pMemOp, VO_U32 dwPacketSize)
: CAsfIndexEntries(pMemOp)
, m_pEntries(NULL)
, m_dwPacketSize(dwPacketSize)
{
}

CAsfSampleIndexObjectEntries::~CAsfSampleIndexObjectEntries()
{
	if (m_pEntries)
		SAFE_MEM_FREE(m_pEntries);
}

VO_U64 CAsfSampleIndexObjectEntries::GetPacketPosByTime(VO_S64 llTimeStamp)
{
	VO_U32 dwIndex = static_cast<VO_U32>(llTimeStamp / m_ullTimeInterval);
	VO_U32 dwPacketNum = (dwIndex < m_dwEntries) ? m_pEntries[dwIndex].packet_number : m_pEntries[m_dwEntries - 1].packet_number;

	return m_dwPacketSize * dwPacketNum;
}

VO_U64 CAsfSampleIndexObjectEntries::GetPacketPosByIndex(VO_U32 dwIndex)
{
	return (dwIndex < m_dwEntries) ? (m_dwPacketSize * m_pEntries[dwIndex].packet_number) : VO_MAXU64;
}

VO_S64 CAsfSampleIndexObjectEntries::GetPrevKeyFrameTime(VO_S64 llTimeStamp)
{
	VO_U32 dwStart = static_cast<VO_U32>(llTimeStamp / m_ullTimeInterval);
	if (dwStart >= m_dwEntries)
		dwStart = m_dwEntries - 1;

	VO_U32 dwIndex = dwStart;
	for(VO_U32 i = dwStart; i > 0; i--)
	{
		if(m_pEntries[i - 1].packet_number != m_pEntries[i].packet_number)
		{
			dwIndex = i;
			break;
		}
	}

	return dwIndex * m_ullTimeInterval;
}

VO_S64 CAsfSampleIndexObjectEntries::GetNextKeyFrameTime(VO_S64 llTimeStamp)
{
	VO_U32 dwStart = static_cast<VO_U32>(llTimeStamp / m_ullTimeInterval);
	VO_U32 dwIndex = VO_MAXU32;
	for(VO_U32 i = dwStart; i < m_dwEntries - 1; i++)
	{
		if(m_pEntries[i + 1].packet_number != m_pEntries[i].packet_number)
		{
			dwIndex = i + 1;
			break;
		}
	}

	return (VO_MAXU32== dwIndex) ? VO_MAXS64 : (dwIndex * m_ullTimeInterval);
}

VO_U32 CAsfSampleIndexObjectEntries::SetEntriesNum(VO_U32 dwEntries)
{
	m_dwEntries = dwEntries;

	m_pEntries = NEW_OBJS(AsfIndexEntry, m_dwEntries);
	if(!m_pEntries)
		return VO_ERR_OUTOF_MEMORY;

	return VO_ERR_NONE;
}

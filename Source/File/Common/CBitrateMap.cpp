

#include "CBitrateMap.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CBitrateMap::CBitrateMap()
:m_nCurChunk(-1)
,m_nBitrate(0)
,m_bSmoothPlayed(VO_TRUE)
,m_nDelayTimes(0)
,m_nMaxBitrate(0x7FFFFFFF)
,m_pBACAP(NULL)
{
	memset(&m_timeMap, 0x00, sizeof(TIME_MAP) * MAP_SIZE);
}
CBitrateMap::~CBitrateMap()
{

}
void CBitrateMap::FlushMap()
{
	memset(&m_timeMap, 0x00, sizeof(TIME_MAP) * MAP_SIZE);
	m_nDelayTimes = 0;
}

int CBitrateMap::CheckDelayTimeStamp(int delayTime, VO_U64 timeStamp)
{
// 	if(!m_bSmoothPlayed)
// 		return -1;
	//if(m_nMaxBitrate > m_nBitrate)
		
	int i = 0;
	for(i = 0; i < MAP_SIZE; i++ )
	{
		if(m_timeMap[i].uStartTime <= timeStamp && m_timeMap[i].uEndTime > timeStamp)
		{
			VOLOGR("DelayTime Match[%d][%d]. DelayTime:%d",m_nBitrate,m_nDelayTimes,delayTime);
			if(delayTime >= 100)
				VOLOGW("BAInfo***. VideoDelayTime:%d > 100, This is Birate(%d) will be Caped", delayTime,m_nBitrate);

			if(delayTime >= 140)
				m_nDelayTimes += 100;
			if(delayTime >= 100)
				m_nDelayTimes += 100;
			if(delayTime >= 50)
				m_nDelayTimes += 1;
			if(delayTime< -30)
				m_nDelayTimes -= 1;
			if(delayTime< -40)
			{
				m_nDelayTimes -= 2;
				if(m_nDelayTimes > 70)
					m_nDelayTimes -= 10;
			}
			if(delayTime< -50)
			{
				m_nDelayTimes -= 3;
				if(m_nDelayTimes > 80)
					m_nDelayTimes -= 20;
			}
		
			break;
		}
	}

	if(i >= MAP_SIZE)
	{
		VOLOGR("BA. TimeStamp(%lld) is not belong to Bitrate[%d].", timeStamp,m_nBitrate);
		return -1;
	}

	if(m_nDelayTimes< 0)
		m_nDelayTimes = 0;

	if(m_nMaxBitrate > 0)
	{
		m_bSmoothPlayed = (m_nMaxBitrate > m_nBitrate) ?  VO_TRUE : VO_FALSE;
	}

	if(m_bSmoothPlayed && m_nDelayTimes >100  )
	{
		VOLOGW("BAInfo***. RunTime Speed Check. This bitrate is too Height[%d].(VideoDelayTime:%d)", m_nBitrate,delayTime);
		m_bSmoothPlayed = VO_FALSE;
	}	
	return 0;
}
void CBitrateMap::AddtimeStamp2Map(VO_U64 timeStamp, int nFlag /*= 0*/)//;/*nFlag = 1, this is start of chunk*/
{
	if(nFlag == 1)
	{
		m_nCurChunk ++;
		if(m_nCurChunk >= MAP_SIZE)
			m_nCurChunk = 0; 
		m_timeMap[m_nCurChunk].uStartTime = timeStamp;
	}
	else
		m_timeMap[m_nCurChunk].uEndTime = timeStamp;
}

VO_U32 CBitrateMap::SetParam(VO_U32 uID, VO_PTR pParam)
{
	switch(uID)
	{
 	case 1:
 		break;
	default:
		return VO_RET_SOURCE2_NOIMPLEMENT;
	}
	return VO_RET_SOURCE2_OK;
}


void CBitrateMap::SetBitrate(int index,int bitrate,int w, int h)
{
	m_nBitrate = bitrate;
	VOLOGI("BAInfo*** Index:%d, Bitrate:%d [%d*%d]",index, bitrate, w, h);
}

void CBitrateMap::SetBACAP(VO_SOURCE2_CAP_DATA* pCap)
{
	m_nMaxBitrate = pCap->nBitRate;
	m_pBACAP = pCap;
	if(m_nBitrate > m_nMaxBitrate  )
		m_bSmoothPlayed = VO_FALSE;

}

VO_BOOL CBitrateMap::GetIsSmoothPlayed()
{ 
	VOLOGR("BAInfo***  Bitrate:%d CAP:%d", m_nBitrate, m_nMaxBitrate);
	if(m_nBitrate > m_nMaxBitrate  )
		m_bSmoothPlayed = VO_FALSE;
	return m_bSmoothPlayed;
}
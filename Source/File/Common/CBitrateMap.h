#ifndef __BITRATE_MAP__
#define __BITRATE_MAP__
#include "voYYDef_filcmn.h"
#include "voType.h"
#include "voLog.h"
#include "voSource2.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

struct TIME_MAP
{
	VO_U64 uStartTime;
	VO_U64 uEndTime;
};

#define MAP_SIZE 20
class CBitrateMap
{
public:
	CBitrateMap();
	~CBitrateMap();

	int CheckDelayTimeStamp(int delayTime, VO_U64 timeStamp);

	void AddtimeStamp2Map(VO_U64 timeStamp, int nFlag = 0);//;/*nFlag = 1, this is start of chunk*/
	void FlushMap();

	VO_BOOL GetIsSmoothPlayed();
	void SetBitrate(int index,int bitrate,int w, int h);
	int GetBirate(){return m_nBitrate;}
	VO_U32 SetParam(VO_U32 uID, VO_PTR pParam);
	void SetBACAP(VO_SOURCE2_CAP_DATA* pCap);
private:
	int m_nMaxBitrate;
	int m_nBitrate;
	int m_nCurChunk;
	int m_nDelayTimes;
	VO_BOOL m_bSmoothPlayed;
	TIME_MAP m_timeMap[MAP_SIZE];
	VO_SOURCE2_CAP_DATA* m_pBACAP;
};

#ifdef _VONAMESPACE
}
#endif

#endif

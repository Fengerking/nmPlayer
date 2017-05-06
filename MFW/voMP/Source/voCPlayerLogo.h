	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCPlayerLogo.h

	Contains:	voCPlayerLogo header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-12_17		JBF			Create file

*******************************************************************************/
#ifndef __voCPlayerLogo_H__
#define __voCPlayerLogo_H__

#include "voType.h"
#include "vompType.h"

#if defined (_IOS) || defined (_MAC_OS)
#include <mach/processor_info.h>
#endif

#define LOG_MAX_CATCH_TIME (5*60)

#define		LOGO_CPU_USER_LOAD_VALUE	0x44000001
#define		LOGO_CPU_SYS_LOAD_VALUE		0x44000002
#define		LOGO_CPU_APP_LOAD_VALUE		0x44000003

#define SOURCE_DROP_FLAG		0x01
#define DECODER_DROP_FLAG		0x02
#define RENDER_DROP_FLAG		0x04

#define TIME_NUMBER_MAX		128

typedef struct 
{
	VO_S32	nTimeStamp;
	VO_S32	nPlayingTime;				
	VO_U32	nSystemTime;
	VO_U32	nInSystemTime;				
	VO_U32	nGetSrcSize;
	VO_U32	nDecoderRet;
	VO_U16	nGetSrcTime;
	VO_U16	nDecSrcTime;
	VO_U16	nRenderSrcTime;
	VO_U16	CpuLoading;
	VO_U8	nRndBufCount;
	VO_U8	nBufFlag;
	VO_U16	nReserved16;
	VO_U32	nReserved;			
} VOMP_LOGOINFO;

class CCPULoadInfo;
class voCMediaPlayer;
class voCPlayerLogoInfo
{
public:
	voCPlayerLogoInfo(int nSourceType);
	~voCPlayerLogoInfo();

	virtual int			GetSourceStart (void* pSourceSample);
	virtual int			GetSourceEnd (void* pSourceSample, int nRC);

	virtual int			DecSourceStart (void* pSourceDecInBuf, int nDrop);
	virtual int			DecSourceEnd (void* pSourceDecOutBuf, int nRC);

	virtual int			RenderSourceStart (void * pSourceRenderInBuf, int nBuffer);
	virtual int			RenderSourceEnd (void * pSourceRenderOutBuf, int nPlayTime, int nRC);

	virtual int 		GetParam (int nID, void * pValue);
	virtual int 		SetParam (int nID, void * pValue);

	virtual int 		Flush ();
	virtual int			RestStasticPerfoData();

	virtual int			Start();
	virtual int			Stop();

	virtual void		SetWorkPath( VO_PTCHAR pWorkPath);

protected:
	VOMP_LOGOINFO*		FindItemFromTimeStamp(int nTimeStamp);
	int					GetCurrPerformance(VOMP_PERFORMANCEDATA *pPerfData);
	int					ReadFile(const char*  pathname, char*  buffer, size_t  buffsize);
	int					LicenceCheck();
	void				UpdataPerformanceData(bool bForceUpdate = false);
	int					GetPerformance(VOMP_PERFORMANCEDATA *pPerfData);

	void				UpdateSrcTimeNumber(int nTime);
	void				UpdateDecTimeNumber(int nTime);
	void				UpdateRendTimeNumber(int nTime);
	void				UpdateJitterTimeNumber(int nTime);

protected:	
	int					m_nSourceType;

	int					m_nMaxCount;
	int					m_nCPULoading;
	int					m_nSysLoading;
	int					m_nAPLoading;

	int					m_nReadCount;
	int					m_nReadIndex;
	int					m_nGetStartTime;
	int					m_nGetEndTime;
	int					m_nOutTimeStamp;
	int					m_nGetSrcSize;	
	int					m_nGetSrcTime;
	int					m_nGetSuccess;
	int					m_nDecStartTime;
	int					m_nDecSuccess;
	int					m_nDecEndTime;
	int					m_nRdBufferNum;
	int					m_nRdStartTime;
	int					m_nRdEndTime;	
	int					m_nStartSystemTime;
	int					m_nLastRenderTime;

	int*				m_pIntDecError;
	int					m_nErrorLen;

	voCMutex			m_mtStatus;
	voCMutex			m_mtPerfUpdata;

	VOMP_LOGOINFO*		m_pLogoInfo;
	char*				m_pLogBuffer;
	int					m_nLogEnable;

	int					m_nMaxDecoderTime;
	int					m_nMaxRenderTime;

	int					m_nSumDecoderTime;
	int					m_nSumRenderTime;

	int					m_nSourceDropSample;
	int					m_nDecoderDropFrame;
	int					m_nRnnderDropFrame;
	int					m_nRenderFrame;
	int					m_nDecoderFrameNumber;

	int					m_nStatisPlaybackStartTime;
	int					m_nStatisPlaybackEndTime;
	double				m_nStatisPlaybackDurationTime;
	int					m_nStatisTotalSourceDropSample;
	int					m_nStatisTotalDecoderDropFrame;
	int					m_nStatisTotalRenderDropFrame;
	int					m_nStatisTotalRenderFrame;
	int					m_nStatisTotalDecoderFrame;

	int					m_nFrameRateCount;
	int					m_nFrameRateStartTime;
	int					m_nFrameRateValue;

	VOMP_PERFORMANCEDATA m_sPerformData;

	int					m_nDecoderTotalTime;
	int					m_nDecoderNumber;

	int					m_nVideoRenderStartTime;
	int					m_nVideoRenderEndTime;

	CCPULoadInfo*		m_pCPULoadInfo;
	int					m_nUserCPUload;
	int					m_nTotalCPUload;
	int					m_nCPUThreadStart;

	int					m_nStatus;

	VO_PTCHAR			m_pWorkPath;
	VO_PTR				m_pLicenceCheckHandle;

	typedef struct _sSourceTimeNumber
	{
		int nDuration;
		int TimeUsed;
	} sSourceTimeNumber;
	typedef struct _sDecoderTimeNumber
	{
		int nDuration;
		int TimeUsed;
	} sDecoderTimeNumber;
	typedef struct _sRenderTimeNumber
	{
		int nDuration;
		int TimeUsed;
	} sRenderTimeNumber;
	typedef struct _sJitterTimeNumber
	{
		int nDuration;
		int TimeUsed;
	} sJitterTimeNumber;

	int m_nSrcTimeNumCount;
	int m_nDecTimeNumCount;
	int m_nRendTimeNumCount;
	int m_nJitterTimeNumCount;
	sSourceTimeNumber m_szSrcTimeNumSet[TIME_NUMBER_MAX];
	sDecoderTimeNumber m_szDecTimeNumSet[TIME_NUMBER_MAX];
	sRenderTimeNumber m_szRndTimeNumSet[TIME_NUMBER_MAX];
	sJitterTimeNumber m_szJitterTimeNumSet[TIME_NUMBER_MAX];

	typedef struct _sPerfDataNode
	{
		int nDuration;
		VOMP_PERFORMANCEDATA* pPerfData;
		int nSrcTimeNumCount;
		int nDecTimeNumCount;
		int nRendTimeNumCount;
		int nJitterTimeNumCount;
		sSourceTimeNumber szSrcTimeNumSet[TIME_NUMBER_MAX];
		sDecoderTimeNumber szDecTimeNumSet[TIME_NUMBER_MAX];
		sRenderTimeNumber szRndTimeNumSet[TIME_NUMBER_MAX];
		sJitterTimeNumber szJitterTimeNumSet[TIME_NUMBER_MAX];
		_sPerfDataNode* pNext;
	} sPerfDataNode;

	int	m_nCachCount;
	int	m_nMaxCachSize;
	sPerfDataNode* m_pPerfDataHeader;
	int	m_nLastUpdatePerfTime;
friend class voCMediaPlayer;
};

struct CpuInfo {
    long unsigned utime, ntime, stime, itime;
    long unsigned iowtime, irqtime, sirqtime;
};

struct ProcInfo {
    char state;
    long unsigned utime;
    long unsigned stime;
    long unsigned delta_utime;
    long unsigned delta_stime;
    long unsigned delta_time;
    long vss;
    long rss;
    int prs;
};

class CCPULoadInfo
{
public:
	CCPULoadInfo();
	~CCPULoadInfo();

	virtual int		InitCPUProc();
	virtual int		UpdateCPUProc();
	virtual int		GetCPULoad(int* pOutSys, int* pOutUsr, int* pOutAP);

protected:
	int		GetCPUInfo(CpuInfo* pCPUInfo);
	int		GetAPProcInfo(ProcInfo* pProcInfo);

protected:	
	unsigned int		m_nCount;
	
	CpuInfo m_szCPUInfoOld;
	CpuInfo m_szCPUInfoCur;

	ProcInfo m_szProcAPInfoOld;
	ProcInfo m_szProcAPInfoCur;
    
#if defined (_IOS) || defined (_MAC_OS)
    processor_info_array_t prevCpuInfo;
    mach_msg_type_number_t prevCpuInfoNum;
    
    processor_info_array_t curCpuInfo;
    mach_msg_type_number_t curCpuInfoNum;
#endif
};

#endif //__voCPlayerLogo_H__

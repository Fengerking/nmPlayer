#ifndef _CRECORDER_ENGINE_H
#define _CRECORDER_ENGINE_H

#include "voMMRecord.h"

class CSrc;
class CAVEncoder;
class CSourceSink;
class CColorConvert;

class CRecorderEngine
{
public:
	CRecorderEngine(void);
public:
	virtual ~CRecorderEngine(void);

public:
	bool Open();
	bool Close();
	void SetFileName(TCHAR* pFileDir);
	
	bool StartRecord();
	bool StopRecord();
	bool PauseRecord();
	bool RestartRecord();

	bool SetParameter(long paramID, void* pValue);
	bool GetParameter(long paramID, void* pValue);

	void SetNotifyCallback(VOMMRecordCallBack pProc);

	void RecvAudioSrc(VO_MMR_ASBUFFER* pBuffer);
	void RecvVideoSrc(VO_MMR_VSBUFFER* pBuffer);

	CColorConvert* CreateColorConvert(VO_MMR_VSFORMAT* pFmt, VO_IV_COLORTYPE nOutputVideoType);

private:
	void DeleteTmpFile(TCHAR* pFileDir);

private:
	CSrc* m_pAudioSrc;
	CSrc* m_pVideoSrcForEncode;
	CSrc* m_pVideoSrcForPreview;

	CSourceSink* m_pSrcSink;
	CAVEncoder* m_pAVEncoder;

	//
	bool m_bRecording;
	unsigned long m_dwLastRecordTime;

	//
	void*				m_pUserData;
	VOMMRecordCallBack m_pNotifyCB;

	VO_MMR_VSFORMAT m_fmtRec;
};

#endif // _CRECORDER_ENGINE_H

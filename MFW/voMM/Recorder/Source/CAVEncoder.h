#ifndef _CAV_ENCODER_H
#define _CAV_ENCODER_H

#include "voMMRecord.h"

class CDump;
class CEncode;
class CSampleData;

class CAVEncoder
{
public:
	CAVEncoder(void);
public:
	~CAVEncoder(void);

public:
	void Init();
	unsigned long StartEncode();
	void StopEncode();

	unsigned long  RecvVideoSample(CSampleData* pSample, VO_IV_COLORTYPE& videoType);
	unsigned long  RecvAudioSample(CSampleData* pSample);

	bool SetVideoFileType(long fType);
	bool GetVideoFileType(long* pType);
	bool SetVideoEncodeType(long vType);
	bool GetVideoEncodeType(long* pType);
	bool SetAudioEncodeType(long aType);
	bool GetAudioEncodeType(long* pType);
	bool GetFrameRate(float* pFrameRate);

	void SetVideoSize(int nWidth, int nHeight);

	void SetImgFileDirectory(TCHAR* pFileDir);
	void SetVideoFileDirectory(TCHAR* pFileDir);

	void SetEncodeQuality(int nQuality);
	void SetImageQuality(int nQuality);

	unsigned long GetMpeg4EncodeBitrate();
	void SetMpeg4EncodeBitrate(unsigned long dwBitrate);
	unsigned long GetH263EncodeBitrate();
	void SetH263EncodeBitrate(unsigned long dwBitrate);

	void SetVideoEncodeFrameRate(int nFrmRate);

	void SetContentType(int nType);
	int	 GetContentType();
	bool IsRecordAudio();
	bool IsRecordVideo();

	long long GetDumpSize();
	void SetNotifyCallback(VOMMRecordCallBack pProc);

private:
	void Release();

	CDump* GetCurrDump();
	CEncode* GetCurrVideoEncode();
	CEncode* GetCurrAudioEncode();
	void SetRealFrameRate();
private:
	CDump* m_pCurrDump;
	CEncode* m_pCurrAudioEncode;
	CEncode* m_pCurrVideoEncode;

	long m_lCurrVideoFileType;
	long m_lCurrVideoEncodeType;
	long m_lCurrAudioEncodeType;

	int m_nVideoWidth;
	int m_nVideoHeight;

	TCHAR m_szImgFileDir[256];
	TCHAR m_szVideoFileDir[256];

	unsigned long m_dwTotalEncodeVideoCount;
	unsigned long m_dwLastVideoEncodeTime;
	unsigned long m_dwTotalAudioRawDataLen;

	int m_nEncodeQuality;
	unsigned long m_dwMpeg4EncodeBitrate;
	unsigned long m_dwH263EncodeBitrate;

	int m_nContentType;
	int m_nImageQuality;
	int m_nVideoFrmRate;

	void*				m_pUserData;
	VOMMRecordCallBack m_pNotifyCB;
};

#endif // _CAV_ENCODER_H

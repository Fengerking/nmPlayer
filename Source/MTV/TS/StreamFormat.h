#ifndef _STREAM_FORMAT_H
#define _STREAM_FORMAT_H

#include "voYYDef_TS.h"
#include "TsParserBaseDef.h"
#include "framechk.h"
#include "voVideo.h"
#include "voAAC.h"
#include "voH264.h"



#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CStreamFormat
{
public:
	CStreamFormat(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	~CStreamFormat(void);

public:

	int GetCodec() const { return m_codec; }
	int GetMaxFrameSize() const { return m_nMaxFrameSize; }	
	bool IsVideo(){return m_bVideo;};
public:

	// general
	VO_U32 GetHeadData(VO_PTR pVal);

	//for audio
	VO_U32 GetAudioFormat(VO_PTR pVal);
	VO_U32 GetAudioChannels(VO_PTR pVal);
	VO_U32 GetAudioSampleRate(VO_PTR pVal);
	
	// for video
	VO_U32 GetVideoFormat(VO_PTR pVal);
	VO_U32 GetVideoWidth(VO_PTR pVal);
	VO_U32 GetVideoHeight(VO_PTR pVal);
	VO_U32 GetVideoProgressive(VO_PTR pVal);


	// for H264
	VO_U32 GetH264StreamFormat(VO_PTR pVal);

	// for AAC
	VO_U32 GetAACObjectType(VO_PTR pVal);
	VO_U32 GetAACFrameType(VO_PTR pVal);


private:
	void InitAAC(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	void InitADTSAAC(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	void InitLATMAAC(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	void InitAVC(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	void InitH264(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	void InitHEVC(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	void InitMP3(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	void InitMPEG2(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	void InitMPEG4(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	void InitRMX(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	void InitAC3(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	void InitM2TSLPCM(VOSTREAMPARSESTREAMINFO* pStreamInfo);

protected:
	int m_codec;
	union
	{
		VOVIDEOFORMAT m_video;
		VOAUDIOFORMAT m_audio;
	};

	union
	{
		struct
		{
			int object_type;
			int frame_type;
		} 
		m_aac;

		struct
		{
			int nalu_length_size;
			VO_H264FILEFORMAT stream_format;
		} 
		m_avc;

	};

	bool m_videoProgressive;

	VO_BYTE* m_pHeadData;
	int m_nHeadSize;

	int m_nMaxFrameSize;
    VO_TS_STREAM_TYPE         m_eStreamMediaType;

protected:
	void CheckSaveHeadData(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	bool ParseMP3Head(const void *pbData, int nSize, int& samplerate, int& channels, int& bitrate, int& framelen);

protected:
	FrameChecker* m_pFrameChecker;
	bool			m_bVideo;
};
#endif //.h


#ifdef _VONAMESPACE
}
#endif


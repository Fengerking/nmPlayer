	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		DTSReader.h

	Contains:	DTS file parser class header file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2012-03-05		Rodney		Create file

*******************************************************************************/

#pragma once
#include "CBaseAudioReader.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


// Audio channel arrangement.
typedef enum {
	AMODE_MONO_A = 0,
	AMODE_DUAL_MONO_A_AND_B,
	AMODE_LEFT_RIGHT,
	AMODE_SUM_DIFFERENCE,
	AMODE_LT_AND_RT,
	AMODE_C_L_R,
	AMODE_L_R_S,
	AMODE_C_L_R_S,
	AMODE_L_R_SL_SR,
	AMODE_C_L_R_SL_SR,
	AMODE_CL_CR_L_R_SL_SR,
	AMODE_C_L_R_LR_RR_OV,
	AMODE_CF_CR_LF_RF_LR_RR,
	AMODE_CL_C_CR_L_R_SL_SR,
	AMODE_CL_CR_L_R_SL1_SL2_SR1_SR2,
	AMODE_CL_C_CR_L_R_SL_S_SR,
	AMODE_USER_DEFINED
} AMODE;

// DTS bit rate constants(Kbit/s).
const static VO_U32 dtsBitRate[] = {
	32000, 56000, 64000, 96000, 112000, 128000, 192000, 224000,
	256000, 320000, 384000, 448000, 512000, 576000, 640000, 768000, 
	960000, 1024000, 1152000, 1280000, 1344000, 1408000, 1411200, 1472000,
	1536000, 1920000, 2048000, 3072000, 3840000, 0, 0, 0
};

// DTS frame header(Total: 88 bits)
// Corresponding value of each field.
typedef struct {
	VO_BYTE frameType;				// 1 bit
	VO_BYTE deficitSampleCount;		//* 5 bits
	VO_BYTE	crcFlag;				// 1 bit
	VO_BYTE	nblks;					//* 7 bits
	VO_U16	frameByteSize;			//* 14 bits
	VO_BYTE	audioMode;				// 6 bits
	VO_BYTE	sampleFrequency;		//* 4 bits
	VO_BYTE	rate;					//* 5 bits
	VO_BYTE	mixFlag;				// 1 bit
	VO_BYTE	dynfFlag;				// 1 bit
	VO_BYTE timeStampFlag;			//* 1 bit
	VO_BYTE auxFlag;				// 1 bit
	VO_BYTE hdcd;					// 1 bit
	VO_BYTE extAudioID;				// 3 bits
	VO_BYTE	extAudioFlag;			// 1 bit
	VO_BYTE aspfFlag;				// 1 bit
	VO_BYTE lfeFlag;				//* 2 bits
	VO_BYTE hFlag;					// 1 bit
	VO_U16	hCRC;					// 16 bits
	VO_BYTE filts;					// 1 bit
	VO_BYTE verNum;					// 4 bits
	VO_BYTE christ;					// 2 bits
	VO_BYTE pcmSampleBits;			//* 3 bits
	VO_BYTE sumFlag;				// 1 bit
	VO_BYTE sumsFlag;				// 1 bit
	VO_BYTE dialNorm;				// 4 bits
} DTSFRAMEHEADERVALUE, *PDTSFRAMEHEADERVALUE;

typedef struct {
	VO_U32 dwSyncWord;
	VO_BYTE frameHeader[11];
	DTSFRAMEHEADERVALUE value;
} DTSFRAMEHEADER, *PDTSFRAMEHEADER;


class CDTSReader :
	public CBaseAudioReader
{
public:
	CDTSReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CDTSReader(void);
	virtual VO_U32 GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32 GetBitrate(VO_U32* pdwBitrate);
	virtual VO_U32 GetCodecCC(VO_U32* pCC);
	virtual VO_U32 GetDuration() { return m_dwDuration; }
	virtual	VO_U32 GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32 GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32 SetPos(VO_S64* pPos);
	virtual VO_U32 Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);

protected:
	VO_BOOL ReadDTSHeader();
private:
	VO_U32			m_dwBitRate;
	VO_U32			m_dwFrames;		// number of frames
	VO_U32			m_dwSamples;	// number of samples
	VO_U32			m_dwDurationPerFrame;	// duration of each frame
	VO_U32			m_dwFrameIndex;	// index to a frame
	VO_AUDIO_FORMAT dtsFormat;
	DTSFRAMEHEADER	dtsFrameHeader;
};

#ifdef _VONAMESPACE
}
#endif

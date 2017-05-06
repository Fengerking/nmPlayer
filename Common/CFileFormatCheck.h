	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFileFormatCheck.h

	Contains:	CFileFormatCheck header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-10-29		JBF			Create file

*******************************************************************************/
#ifndef __CFileFormatCheck_H__
#define __CFileFormatCheck_H__

#include "voYYDef_Common.h"
#include "voFile.h"
#include "CvoBaseObject.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define FLAG_CHECK_AAC			0x00000001
#define FLAG_CHECK_AMR			0x00000002
#define FLAG_CHECK_AWB			0x00000004
#define FLAG_CHECK_MP3			0x00000008
#define FLAG_CHECK_QCP			0x00000010
#define FLAG_CHECK_WAV			0x00000020
#define FLAG_CHECK_FLAC			0x00000040

#define FLAG_CHECK_MIDI			0x00000080
#define FLAG_CHECK_OGG			0x00000100

#define FLAG_CHECK_MP4			0x00000200
#define FLAG_CHECK_MOV			0x00000400
#define FLAG_CHECK_ASF			0x00000800
#define FLAG_CHECK_AVI			0x00001000
#define FLAG_CHECK_REAL			0x00002000
#define FLAG_CHECK_MPG			0x00004000
#define FLAG_CHECK_SDP			0x00008000
#define FLAG_CHECK_FLV			0x00010000

#define FLAG_CHECK_AU			0x00020000
#define FLAG_CHECK_MKV			0x00040000

#define FLAG_CHECK_TS			0x00080000
#define FLAG_CHECK_DV			0x00100000
#define FLAG_CHECK_H263			0x00200000
#define FLAG_CHECK_H264			0x00400000
#define FLAG_CHECK_MPEG4		0x00800000
#define FLAG_CHECK_CMMB			0x01000000

#define FLAG_CHECK_APE			0x02000000
#define FLAG_CHECK_ALAC			0x04000000
#define FLAG_CHECK_AC3			0x08000000
#define FLAG_CHECK_DTS			0x10000000
#define FLAG_CHECK_H265			0x20000000
#define FLAG_CHECK_WEBVTT			0x40000000
#define FLAG_CHECK_RAWDATA		0x80000000

#define FLAG_CHECK_NONE			0x0
#define FLAG_CHECK_ALL			0xFFFFFFFF
//AAC, AMR, AWB, MP3, QCP, WAV, FLAC, AU, APE, ALAC, AC3
#define FLAG_CHECK_AUDIOREADER	(FLAG_CHECK_AAC | FLAG_CHECK_AMR | FLAG_CHECK_AWB | FLAG_CHECK_MP3 | FLAG_CHECK_QCP | FLAG_CHECK_WAV | FLAG_CHECK_FLAC | FLAG_CHECK_AU | FLAG_CHECK_APE | FLAG_CHECK_ALAC | FLAG_CHECK_AC3 | FLAG_CHECK_DTS)
//Difficult to judge, AAC, MP3
#define FLAG_CHECK_AUDIOREADER2	(FLAG_CHECK_AAC | FLAG_CHECK_MP3)
//AAC, MP3, MP4, ASF, AC3
#define FLAG_CHECK_METADATA		(FLAG_CHECK_AAC | FLAG_CHECK_MP3 | FLAG_CHECK_MP4 | FLAG_CHECK_ASF | FLAG_CHECK_AC3)
//SDP, MPG, TS, CMMB, AC3
#define FLAG_CHECK_EXTNAME		(FLAG_CHECK_SDP | FLAG_CHECK_MPG | FLAG_CHECK_TS | FLAG_CHECK_CMMB | FLAG_CHECK_AC3 | FLAG_CHECK_DTS)

class CFileFormatCheck : public CvoBaseObject
{
public:
	CFileFormatCheck(VO_FILE_OPERATOR* pFileOP, VO_MEM_OPERATOR* pMemOP);
	virtual ~CFileFormatCheck();

public:
	VO_FILE_FORMAT			GetFileFormat(VO_FILE_SOURCE* pSource, VO_U32 dwCheckFlag, VO_U32 dwSgstFlag = FLAG_CHECK_NONE);
	static VO_FILE_FORMAT	GetFileFormat(VO_PTCHAR pFilePath, VO_U32 dwCheckFlag);

protected:
	VO_BOOL		IsMP4 (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsMOV (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsASF (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsAVI (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsREAL (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsMPG (VO_PBYTE pHeadData, VO_S32 nHeadSize);

	VO_BOOL		IsAAC (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsAC3(VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsAMR (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsAWB (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsDTS(VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsMP3 (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsQCP (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsWAV (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsMID (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsFLAC (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsAU (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsOGG (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsMKV (VO_PBYTE pHeadData, VO_S32 nHeadSize);

	VO_BOOL		IsSDP (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsFLV (VO_PBYTE pHeadData, VO_S32 nHeadSize);

	VO_BOOL		IsAPE (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsTS (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsTTML(VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsWEBVTT(VO_PBYTE pHeadData, VO_S32 nHeadSize);

protected:
	VO_BOOL		SkipID3V2Header(VO_PTR pFile, VO_PBYTE pHeadData, VO_S32& nHeadSize, VO_S64& llFileHeaderSize);

	VO_PBYTE	MP3FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen);
	VO_BOOL		MP3ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize);
	VO_PBYTE	AACFindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen);
	VO_BOOL		AACParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize);

	VO_PBYTE	FindTSPacketHeader(VO_PBYTE pData, VO_S32 nDataSize, VO_S32 nPacketSize);
	VO_BOOL		FindTSPacketHeader2(VO_PBYTE pData, VO_S32 cbData, VO_S32 packetSize);

	VO_PBYTE	MPGFindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen);
	VO_BOOL		MPGParseHeader(VO_PBYTE pHeader, VO_U32 dwLen);

public:
	static VO_FILE_FORMAT	Flag2FileFormat(VO_U32 dwFlag);
	static VO_U32			FileFormat2Flag(VO_FILE_FORMAT ff);

protected:
	VO_FILE_OPERATOR*	m_pFileOP;
	VO_MEM_OPERATOR*	m_pMemOP;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif //__CFileFormatCheck_H__


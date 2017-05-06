	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFileSource.h

	Contains:	CFileSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CFileSource_H__
#define __CFileSource_H__

#include "CBaseSource.h"
#include "voFile.h"
#include "voMetaData.h"

enum {
	METADATA_KEY_CD_TRACK_NUMBER = 0,
	METADATA_KEY_ALBUM           = 1,
	METADATA_KEY_ARTIST          = 2,
	METADATA_KEY_AUTHOR          = 3,
	METADATA_KEY_COMPOSER        = 4,
	METADATA_KEY_DATE            = 5,
	METADATA_KEY_GENRE           = 6,
	METADATA_KEY_TITLE           = 7,
	METADATA_KEY_YEAR            = 8,
	METADATA_KEY_DURATION        = 9,
	METADATA_KEY_NUM_TRACKS      = 10,
	METADATA_KEY_IS_DRM_CRIPPLED = 11,
	METADATA_KEY_CODEC           = 12,
	METADATA_KEY_RATING          = 13,
	METADATA_KEY_COMMENT         = 14,
	METADATA_KEY_COPYRIGHT       = 15,
	METADATA_KEY_BIT_RATE        = 16,
	METADATA_KEY_FRAME_RATE      = 17,
	METADATA_KEY_VIDEO_FORMAT    = 18,
	METADATA_KEY_VIDEO_HEIGHT    = 19,
	METADATA_KEY_VIDEO_WIDTH     = 20,
	METADATA_KEY_WRITER          = 21,
	// Add more here...
};

class CFileSource : public CBaseSource
{
public:
	CFileSource (VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB);
	virtual ~CFileSource (void);

	virtual VO_U32		LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	virtual VO_U32		CloseSource (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		GetSourceInfo (VO_PTR pSourceInfo);
	virtual VO_U32		GetTrackInfo (VO_U32 nTrack, VO_PTR pTrackInfo);

	virtual VO_U32		GetAudioFormat (VO_U32 nTrack, VO_AUDIO_FORMAT * pAudioFormat);
	virtual VO_U32		GetVideoFormat (VO_U32 nTrack, VO_VIDEO_FORMAT * pVideoFormat);

	virtual VO_U32		GetFrameType (VO_U32 nTrack);

	virtual VO_U32		GetTrackData (VO_U32 nTrack, VO_PTR pTrackData);

	virtual VO_U32		SetTrackPos (VO_U32 nTrack, VO_S64 * pPos);

	virtual VO_U32		GetSourceParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32		SetSourceParam (VO_U32 nID, VO_PTR pValue);

	virtual VO_U32		GetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue);
	virtual VO_U32		SetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue);

	virtual VO_U32		GetNearKeyframe (VO_SOURCE_NEARKEYFRAME* pNearKeyframe);

	virtual VO_BOOL		IsOutputConfigData(VO_U32 nTrack);
	virtual VO_U32		SetOutputConfigData(VO_U32 nTrack, VO_BOOL bOutputConfigData);
	virtual void		SetPlayClearestVideo(VO_BOOL bPlayClearestVideo) {m_bPlayClearestVideo = bPlayClearestVideo;}
	virtual void		SetWVC1Decoder(VO_U32 nDecoder) {m_nWVC1Decoder = nDecoder;}
	virtual void		SetLiveSrcType(VO_LIVESRC_FORMATTYPE nType) {m_LiveSrcType = nType;};
	virtual VO_U32		LoadLib (VO_HANDLE hInst);

	virtual	char*		GetMetaDataText (int nIndex);

protected:
	virtual VO_BOOL		ConvertHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize);
	virtual VO_BOOL		ConvertData (VO_PBYTE pData, VO_U32 nSize);
	virtual int			Config2ADTSHeader(/*in*/	unsigned char *config, 
		    							  /*in*/	unsigned int  conlen,
	  									  /*in*/	unsigned int  framelen,
	  									  /*in&out*/unsigned char *adtsbuf,
	  									  /*in&out*/unsigned int  *adtslen);
	virtual VO_U32		CheckCodecLegal();
	virtual VO_BOOL     ConvertHEVCHeadData(VO_PBYTE pHeadData, VO_U32 nHeadSize);

	virtual VO_BOOL		AllocOutputConfigDatas();

protected:
	VO_SOURCE_READAPI	m_funFileRead;
	VO_SOURCE_OPENPARAM	m_paramOpen;
	VO_FILE_SOURCE		m_Source;
	VO_PTR				m_hFile;
	VO_BOOL				m_bForceClosed;

	VO_SOURCE_INFO		m_filInfo;
	VO_BOOL *			m_pbOutputConfigData;
	VO_BOOL				m_bPlayClearestVideo;
	VO_PBYTE			m_pClearestVideoData;
	VO_U32				m_nClearestVideoSize;
	VO_U32				m_nWVC1Decoder;  /* 0: software, 1: qcm hardware, 2: TI harder ware */

	VO_BOOL				m_bAVC2NAL;
	VO_BOOL				m_bIsAVC;
	VO_BOOL				m_bIsHEVC;
	VO_S32				m_nVideoTrack;
	VO_S32				m_nAudioTrack;

	VO_PBYTE			m_pHeadData;
	VO_U32				m_nHeadSize;
	VO_U32				m_nNalLen;
	VO_U32				m_nNalWord;
	VO_PBYTE			m_pVideoData;
	VO_U32				m_nVideoSize;
	VO_U32				m_nFrameSize;

	VO_BOOL				m_bAudioAACAVC;
	VO_PBYTE			m_pAudioData;
	VO_U32				m_nAudioSize;
	VO_SOURCE_TRACKINFO	m_sAudioTrackInfo;

	VO_U32				m_nThumbFrameNum;

	char				m_szMetaData[1024];
	MetaDataString			m_strMetaText;
};

#endif // __CFileSource_H__

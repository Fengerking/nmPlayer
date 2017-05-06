	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMEMetadata.h

	Contains:	CVOMEMetadata header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CVOMEMetadata_H__
#define __CVOMEMetadata_H__

#include "voaEngine.h"

#include "voOMXThread.h"
#include "voCOMXThreadMutex.h"

#include "voFile.h"
#include "voMetaData.h"

#include "vomeFilePipe.h"
#include "CDllLoad.h"

#include "CDrmEngine.h"

#define METADATA_CUSTOMIZED_OFFSET (1000)
#define METADATA_LG_DRM_MATEINFO_OFFSET (0x6F000000)

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
#ifndef _GingerBread
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
	METADATA_KEY_MIMETYPE        = 22,
	METADATA_KEY_DISC_NUMBER	 = 23,
	METADATA_KEY_ALBUMARTIST     = 24,
#else	// _GingerBread
	METADATA_KEY_WRITER          = 11,
	METADATA_KEY_MIMETYPE        = 12,
	METADATA_KEY_ALBUMARTIST     = 13,
	METADATA_KEY_DISC_NUMBER     = 14,
	METADATA_KEY_COMPILATION     = 15,
	//////////////////////////////////////////////////////////////////////////
	// follow KEYs are added by ICS
	//////////////////////////////////////////////////////////////////////////
	METADATA_KEY_HAS_AUDIO		 = 16,
	METADATA_KEY_HAS_VIDEO		 = 17,
	METADATA_KEY_VIDEOWIDTH		 = 18,
	METADATA_KEY_VIDEOHEIGHT	 = 19,
	METADATA_KEY_BITRATE		 = 20,
	METADATA_KEY_TIMED_TEXT_LANGUAGES	  = 21,
	METADATA_KEY_IS_DRM			 = 22,
	METADATA_KEY_LOCATION			 = 23,

	METADATA_KEY_LGE_VIDEO_CONTENT        = 100,
	METADATA_KEY_IS_DRM_CRIPPLED          = 101,
	METADATA_KEY_CODEC                    = 102,
	METADATA_KEY_RATING                   = 103,
	METADATA_KEY_COMMENT                  = 104,
	METADATA_KEY_COPYRIGHT                = 105,
	METADATA_KEY_BIT_RATE                 = 106,
	METADATA_KEY_FRAME_RATE               = 107,
	METADATA_KEY_VIDEO_FORMAT             = 108,
	METADATA_KEY_VIDEO_HEIGHT             = 109,
	METADATA_KEY_VIDEO_WIDTH              = 110,
	METADATA_KEY_DRM_NUM_COUNTS           = 111,
	METADATA_KEY_DRM_IS_PROTECTED         = 112,
	METADATA_KEY_DRM_IS_LICENSE_AVAILABLE = 113,
	METADATA_KEY_DLA_LICENSE_ISSUER       = 114,
	METADATA_KEY_DLA_NUM_REDIRECT         = 115,
	METADATA_KEY_DRM_LICENSE_TYPE         = 116,
	METADATA_KEY_DRM_LICENSE_START        = 117,
	METADATA_KEY_DRM_LICENSE_EXPIRY       = 118,
	METADATA_KEY_DRM_DURATION             = 119,
	METADATA_KEY_DRM_CONTENT_HEADER       = 120,
#endif	// _GingerBread

    // Add more here...
    METADATA_KEY_DRM_IS_RENTAL         = METADATA_CUSTOMIZED_OFFSET + 1, // String : "true" / "false" 
    METADATA_KEY_DRM_USE_COUNTER       = METADATA_CUSTOMIZED_OFFSET + 2, // Int    : useCount of rental DRM file 
    METADATA_KEY_DRM_USE_LIMIT         = METADATA_CUSTOMIZED_OFFSET + 3, // Int    : useLimit of rental DRM file 
    METADATA_KEY_IS_SUPPORTED_AUDIO    = METADATA_CUSTOMIZED_OFFSET + 4, // String : supported / notsupported / notexist
    METADATA_KEY_IS_SUPPORTED_VIDEO    = METADATA_CUSTOMIZED_OFFSET + 5, // String : supported / notsuppo
    METADATA_KEY_IS_3D                 = METADATA_CUSTOMIZED_OFFSET + 6, //Int :  notsupported / SEI type format / for LG 3D Phone

	KEY_PARAMETER_LGEDIVX_RENTAL_COUNT = METADATA_LG_DRM_MATEINFO_OFFSET + 0, // int string
	KEY_PARAMETER_LGEDIVX_TOTAL_COUNT = METADATA_LG_DRM_MATEINFO_OFFSET + 1, // int string 
	KEY_PARAMETER_LGEDIVX_RENTAL_MSG = METADATA_LG_DRM_MATEINFO_OFFSET + 2, // int string "1", "0" 
	KEY_PARAMETER_LGEDIVX_DIVX_DRM_FILE = METADATA_LG_DRM_MATEINFO_OFFSET + 3, // TODO define
	KEY_PARAMETER_LGEDIVX_LICENSE_TYPE = METADATA_LG_DRM_MATEINFO_OFFSET + 4, // Enum int
	KEY_PARAMETER_LGEDIVX_LGVIDEO_PLAYER = METADATA_LG_DRM_MATEINFO_OFFSET + 5, // TODO define
};

typedef enum
{
	IS_PURCHARSE,
	IS_COUNT,
	IS_EXPIRED,
	IS_NOTAUTHORIZE
}DRM_LICENSE_TYPE ;

class CVOMEMetadata : public CDllLoad
{
public:
	CVOMEMetadata();
    virtual ~CVOMEMetadata();

	virtual int				SetFilePipe (void * pFilePipe);
	virtual int				SetFileBasedDrm(void* pFileOpt, void* pDrmAPI);

	virtual int				setDataSource(const char *url);
	virtual int				setDataSource(int fd, int64_t offset, int64_t length);

	virtual VO_FILE_FORMAT	GetFileFormat ();

	virtual	const char *	GetMetaDataText (int nIndex);
	virtual	int				GetAlbumArt (unsigned char ** ppBuffer, int * pBuffSize);

	virtual void			CloseFile (void);

protected:
	virtual VO_BOOL			LoadSource (VO_FILE_FORMAT nFormat);

	virtual VO_FILE_FORMAT	GetFileFormatByName (void);
	virtual VO_FILE_FORMAT	GetFileFormatByData (void);

	VO_BOOL		IsMP4 (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsMOV (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsASF (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsAVI (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsREAL (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsMPG (VO_PBYTE pHeadData, VO_S32 nHeadSize);

	VO_BOOL		IsAMR (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsAWB (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsQCP (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsWAV (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsMID (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsFLAC (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsOGG (VO_PBYTE pHeadData, VO_S32 nHeadSize);

	VO_BOOL		IsFLV (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsMKV (VO_PBYTE pHeadData, VO_S32 nHeadSize);

	VO_BOOL		IsMP3 (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsAAC (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsAC3 (VO_PBYTE pHeadData, VO_S32 nHeadSize);

	VO_PBYTE	MP3FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen);
	VO_BOOL		MP3ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize);
	VO_PBYTE	AACFindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen);
	VO_BOOL		AACParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize);
	VO_PBYTE	MPGFindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen);
	VO_BOOL		MPGParseHeader(VO_PBYTE pHeader, VO_U32 dwLen);

	VO_BOOL		SkipID3V2Header(OMX_PTR hFile, VO_PBYTE pHeadData, VO_S32& nHeadSize, VO_S64& llFileHeaderSize);

	VO_BOOL		IsTS (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_PBYTE    FindTSPacketHeader(VO_PBYTE pData, VO_S32 nDataSize, VO_S32 nPacketSize);
	VO_BOOL		FindTSPacketHeader2(VO_PBYTE pData, VO_S32 cbData, VO_S32 packetSize);

	virtual     VO_U32		vomeOnSourceDrm(OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved);

public:
	static      VO_U32		voSourceDrmCallBack (VO_PTR pUserData, VO_U32 nFlag, VO_PTR pParam, VO_U32 nReserved);

protected:
	voCOMXThreadMutex  		mMutex;
	OMX_VO_FILE_OPERATOR *	m_pFilePipe;

	char					m_szFile[1024];
	int64_t					m_nOffset;
	int64_t					m_nLength;

	OMX_VO_FILE_SOURCE		m_Source;

	VO_SOURCE_READAPI		m_funFileRead;
	VO_SOURCE_OPENPARAM		m_paramOpen;
	VO_PTR					m_hSourceFile;

	VO_BOOL                 m_bDrmCrippled;
	VO_SOURCEDRM_CALLBACK	m_drmCB;
	CDrmEngine *			m_pDrmEngine;
	VO_DRM_FORMAT * 		m_pDrmformat;

	MetaDataString			m_strMetaText;
	char					m_szMetaData[1024];

	VO_FILE_FORMAT			m_nFileFormat;
	VO_DRM_MODE				m_nDrmMode;
};

#endif // __CVOMEMetadata_H__

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

class CVOMEMetadata
{
public:
	CVOMEMetadata();
    virtual ~CVOMEMetadata();

	virtual int				SetFilePipe (void * pFilePipe);

	virtual int				setDataSource(const char *url);
	virtual int				setDataSource(int fd, int64_t offset, int64_t length);

	virtual VO_FILE_FORMAT	GetFileFormat (bool bExt);

	virtual	const char *	GetMetaDataText (int nIndex);
	virtual	int				GetAlbumArt (unsigned char ** ppBuffer, int * pBuffSize);

	virtual void			CloseFile (void);
	
	const char *      GetVideoFormat ();
	const char *      GetAudioFormat ();

protected:
	virtual VO_BOOL			LoadSource (VO_FILE_FORMAT nFormat);

	virtual VO_FILE_FORMAT	GetFileFormatByName (void);
	virtual VO_FILE_FORMAT	GetFileFormatByData (void);

	VO_BOOL		IsMP4 (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsMOV (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsASF (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsAVI (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsREAL (VO_PBYTE pHeadData, VO_S32 nHeadSize);

	VO_BOOL		IsAMR (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsAWB (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsQCP (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsWAV (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsMID (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsFLAC (VO_PBYTE pHeadData, VO_S32 nHeadSize);
	VO_BOOL		IsOGG (VO_PBYTE pHeadData, VO_S32 nHeadSize);

	VO_BOOL		IsFLV (VO_PBYTE pHeadData, VO_S32 nHeadSize);

protected:
	voCOMXThreadMutex  		mMutex;
	OMX_VO_FILE_OPERATOR *	m_pFilePipe;

	char					m_szFile[1024];
	FILE *					m_hFile;
	int64_t					m_nOffset;
	int64_t					m_nLength;

	OMX_VO_FILE_SOURCE		m_Source;

	VO_PTR					m_hAVIModule;
	VO_PTR					m_hASFModule;
	VO_PTR					m_hMP4Module;
	VO_PTR					m_hFLVModule;
	VO_PTR					m_hRealModule;
	VO_PTR					m_hAudioModule;

	VO_SOURCE_READAPI		m_funFileRead;
	VO_SOURCE_OPENPARAM		m_paramOpen;
	VO_PTR					m_hSourceFile;

	MetaDataString			m_strMetaText;
	char					m_szMetaData[1024];

	VO_FILE_FORMAT			m_nFileFormat;
};

#endif // __CVOMEMetadata_H__

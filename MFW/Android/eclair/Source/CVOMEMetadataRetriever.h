	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMEMetadataRetriever.h

	Contains:	CVOMEMetadataRetriever header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/

#ifndef __CVOMEMetadataRetriever_H__
#define __CVOMEMetadataRetriever_H__

#include "voFile.h"
#include "voMetaData.h"
#include "voMMPlay.h"

#include "vomeFilePipe.h"
#include "CVOMEMetadata.h"
#include "CVOWThumbnail.h"

// A simple buffer to hold binary data
class MediaAlbumArt 
{
public:
    MediaAlbumArt(): mSize(0), mData(0) {}

    explicit MediaAlbumArt(const char* url) {
        mSize = 0;
        mData = NULL;
        FILE *in = fopen(url, "r");
        if (!in) {
            return;
        }
        fseek(in, 0, SEEK_END);
        mSize = ftell(in);  // Allocating buffer of size equals to the external file size.
        if (mSize == 0 || (mData = new unsigned char[mSize]) == NULL) {
            fclose(in);
            if (mSize != 0) {
                mSize = 0;
            }
            return;
        }
        rewind(in);
        if ((int)fread(mData, 1, mSize, in) != mSize) {  // Read failed.
            delete[] mData;
            mData = NULL;
            mSize = 0;
            return;
        }
        fclose(in);
    }

    MediaAlbumArt(const MediaAlbumArt& copy) { 
        mSize = copy.mSize; 
        mData = NULL;  // initialize it first 
        if (mSize > 0 && copy.mData != NULL) {
           mData = new unsigned char[copy.mSize];
           if (mData != NULL) {
               memcpy(mData, copy.mData, mSize);
           } else {
               mSize = 0;
           }
        }
    }

    ~MediaAlbumArt() {
        if (mData != 0) {
            delete[] mData;
        }
    }

    // Intentional public access modifier:
    // We have to know the internal structure in order to share it between
    // processes?
    int					mSize;            // Number of bytes in mData
    unsigned char *		mData;            // Actual binary data
};

// Represents a color converted (RGB-based) video frame
// with bitmap pixels stored in FrameBuffer
class VideoFrame
{
public:
    VideoFrame(): mWidth(0), mHeight(0), mDisplayWidth(0), mDisplayHeight(0), mSize(0), mData(0)
#ifdef _GingerBread
		, mRotationAngle(0)
#endif	//_GingerBread
	{}
 
    VideoFrame(const VideoFrame& copy) {
        mWidth = copy.mWidth;
        mHeight = copy.mHeight;
        mDisplayWidth = copy.mDisplayWidth;
        mDisplayHeight = copy.mDisplayHeight;
        mSize = copy.mSize;
        mData = NULL;  // initialize it first
        if (mSize > 0 && copy.mData != NULL) {
            mData = new unsigned char[mSize];
            if (mData != NULL) {
                memcpy(mData, copy.mData, mSize);
            } else {
                mSize = 0;
            }
        }
#ifdef _GingerBread
		mRotationAngle = copy.mRotationAngle;
#endif	//_GingerBread
    }

    ~VideoFrame() {
        if (mData != 0) {
            delete[] mData;
        }
    }

    // Intentional public access modifier:
    int				mWidth;
    int				mHeight;
    int				mDisplayWidth;
    int				mDisplayHeight;
    int				mSize;            // Number of bytes in mData
    unsigned char*	mData;            // Actual binary data
#ifdef _GingerBread
	int				mRotationAngle;   // rotation angle, clockwise
#endif	//_GingerBread
};

class CVOMEMetadataRetriever
{
public:
	CVOMEMetadataRetriever();
    virtual ~CVOMEMetadataRetriever();

	virtual int		setDataSource(const char *url);
	virtual int		setDataSource(int fd, int64_t offset, int64_t length);

	virtual int		SetDrmApi(void *pDrmApi) {mpDRMAPI = (VO_DRM_API *)pDrmApi ; return 0;}
	virtual int		SetFileBasedDrmApi(void *pDrmApi);

	// @param mode The intended mode of operations:
	// 1. 0x00: Experimental - just add and remove data source.
	// 2. 0x01: For capture frame/thumbnail only.
	// 3. 0x02: For meta data retrieval only.
	// 4. 0x03: For both frame capture and meta data retrieval.
	virtual int		setMode(int mode);
	virtual int		getMode(int* mode) const;

	virtual VideoFrame*		getFrameAtTime (const VOA_GETFRAMEATTIME_PARAM* pParam);
	virtual MediaAlbumArt*	extractAlbumArt();
	virtual const char*		extractMetadata(int keyCode);
	virtual int				SetFilePipe (void * pFilePipe);

	static VO_PTR	FileBasedDrm_Open (VO_FILE_SOURCE * pSource);
	static VO_S32	FileBasedDrm_Read (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);
	static VO_S32	FileBasedDrm_Write (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);
	static VO_S32	FileBasedDrm_Flush (VO_PTR pHandle);
	static VO_S64	FileBasedDrm_Seek (VO_PTR pHandle, VO_S64 nPos, VO_FILE_POS uFlag);
	static VO_S64	FileBasedDrm_Size (VO_PTR pHandle);
	static VO_S64	FileBasedDrm_Save (VO_PTR pHandle);
	static VO_S32	FileBasedDrm_Close (VO_PTR pHandle);

protected:
	virtual bool			InitMetadata(int keyCode = 0);
	virtual void			UninitMetadata();
	virtual bool			InitThumbnail();
	virtual void			UninitThumbnail();

protected:
	int						m_nMode;

	char					m_szFile[1024];
	int						m_nFD;
	int64_t					m_nOffset;
	int64_t					m_nLength;

	CVOMEMetadata *			m_pMetadata;
	CVOWThumbnail *			m_pThumbnail;
	bool					m_bInitThumbnailError;

	VO_DRM_API *			mpDRMAPI;

	OMX_VO_FILE_OPERATOR *	m_pFilePipe;
	VO_FILE_OPERATOR 		m_FileOpt;
	IVOFileBasedDRM*		m_pFileBasedDrmAPI;
};

#endif // __CVOMEMetadataRetriever_H__

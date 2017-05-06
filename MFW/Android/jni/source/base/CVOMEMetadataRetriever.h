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
        if (fread(mData, 1, mSize, in) != (size_t)mSize) {  // Read failed.
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
    VideoFrame(): mWidth(0), mHeight(0), mDisplayWidth(0), mDisplayHeight(0), mSize(0), mData(0) {}
 
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
};

class CVOMEMetadataRetriever
{
public:
	CVOMEMetadataRetriever();
    virtual ~CVOMEMetadataRetriever();

	virtual int		setDataSource(const char *url);
	virtual int		setDataSource(int fd, int64_t offset, int64_t length);

	// @param mode The intended mode of operations:
	// 1. 0x00: Experimental - just add and remove data source.
	// 2. 0x01: For capture frame/thumbnail only.
	// 3. 0x02: For meta data retrieval only.
	// 4. 0x03: For both frame capture and meta data retrieval.
	virtual int		setMode(int mode);
	virtual int		getMode(int* mode) const;

	virtual VideoFrame*		captureFrame();
	virtual VideoFrame*		captureFrame2(int nWidth, int nHeight, uint8_t* out_pBuffer, uint32_t* out_npDuration, uint32_t* out_nWidth, uint32_t* out_nHeight);
	virtual MediaAlbumArt*	extractAlbumArt();
	virtual const char*		extractMetadata(int keyCode);
	virtual void		extractMetadata();

	const char* getfilename() const { return m_szFile; }

protected:
	int						m_nMode;

	char					m_szFile[1024];
	int						m_nFD;
	FILE *					m_hFile;
	int64_t					m_nOffset;
	int64_t					m_nLength;

	CVOMEMetadata *			m_pMetadata;

public:
	int                                    m_nWidth;
	int                                    m_nHeight;
	int						m_nDuration;
	char					      m_szVideoFormat[1024];
	char						m_szAudioFormat[1024];
	char                                  m_szFPS[1024];
	
};

#endif // __CVOMEMetadataRetriever_H__

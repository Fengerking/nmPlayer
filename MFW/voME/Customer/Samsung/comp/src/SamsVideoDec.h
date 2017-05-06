// SamsVideoDec.h
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		SamsVideoDec.h

    aim:    for providing the uniform interface for Samsung hardware decoder

	Written by:	Xia GuangTai

	Change History (most recent first):
	2009-1-19		gtxia			Create file

*******************************************************************************/

#ifndef __SAMS_VIDEO_DEC_H
#define __SAMS_VIDEO_DEC_H

class SamsVideoDec
{
  public:
	SamsVideoDec();
	virtual ~SamsVideoDec();
	
  public:
	int             setInputData(unsigned char* inBuffer, unsigned int inSize);
    unsigned char*  getOutputData(unsigned int* outSize);
	int             init(unsigned char* inExt, unsigned int inSize);
	int             deinit();
	int             setParam(unsigned int inID, void* inValue);
	int             getParam(unsigned int inID, void* outValue);
	void            getVideoInfo(long& outWidth, long& outHeight);
  protected:
	virtual int             onSetParam(unsigned int inID, void* inValue);
	virtual int             onGetParam(unsigned int inID, void* outValue);
	virtual int             mfc_create(void);
	virtual int             mfc_destory();
	virtual int             mfc_dec_slice (unsigned char* data, unsigned int size);
	virtual unsigned char*	mfc_get_yuv   (unsigned int* out_size);
	virtual int             mfc_flag_video_frame(unsigned char* data, int size);


  public:
	bool                    setCodecType(int nType);
	
  protected:
	unsigned short  iDisplay_Width, iDisplay_Height;
	void*        	m_mfc_handle;
	int             m_mfc_codec_type; // MFC CODEC_TYPE
	unsigned int    m_mfc_buffer_size;
	unsigned char*	m_mfc_buffer_base;
	unsigned char* 	m_mfc_buffer_now;
	int             m_mfc_flag_info_out;
	int             m_mfc_flag_create;
	int             m_mfc_flag_vol_found;
	unsigned char 	delimiter_h264[4];

	unsigned char* m_pHeader;
	unsigned int   m_nHeaderSize;
	bool           m_bSetHeader;
  protected:
	enum MFC_CODEC_TYPE
	{
		MFC_CODEC_BASE  = -1,
		MFC_CODEC_MP4D  = 0,
		MFC_CODEC_H263D = 1,
		MFC_CODEC_H264D = 2,
		MFC_CODEC_WMVD  = 3,
		MFC_CODEC_MAX   = 4,
	};	
};
#endif

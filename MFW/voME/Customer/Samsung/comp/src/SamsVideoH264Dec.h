// SamsVideoH264Dec.h
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		SamsVideoH264Dec.h

    aim:    for providing the uniform interface for Samsung h264 hardware decoder

	Written by:	Xia GuangTai

	Change History (most recent first):
	2009-1-19		gtxia			Create file

*******************************************************************************/

#ifndef __SAMS_VIDEO_H264_DEC_H
#define __SAMS_VIDEO_H264_DEC_H

#include "SamsVideoDec.h"

class SamsVideoH264Dec : public SamsVideoDec
{
  public:
	SamsVideoH264Dec();
	virtual ~SamsVideoH264Dec();
  protected:
	int             onSetParam(unsigned int inID, void* inValue);
	int             onGetParam(unsigned int inID, void* outValue);
	int             mfc_create(void);
	int             mfc_destory();
	int             mfc_dec_slice (unsigned char* data, unsigned int size);
	unsigned char*	mfc_get_yuv   (unsigned int* out_size);
	int             mfc_flag_video_frame(unsigned char* data, int size);	
};

#endif //__SAMS_VIDEO_H264_DEC_H

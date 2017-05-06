#include <string.h>
#include <stdio.h>
#include "SamsVideoDec.h"

SamsVideoDec::SamsVideoDec()
{
	iDisplay_Width = iDisplay_Height = 0;
	m_mfc_handle = NULL;
	m_mfc_codec_type = -1;
	m_mfc_buffer_size = 0;
	m_mfc_buffer_base = NULL;
	m_mfc_buffer_now = NULL;
	m_mfc_flag_info_out = -1;
	m_mfc_flag_create = 0;
	m_mfc_flag_vol_found = 0;
	memset(delimiter_h264, 0, sizeof(unsigned char));
	m_pHeader = NULL;
	m_nHeaderSize = 0;
	m_bSetHeader = false;
}

SamsVideoDec::~SamsVideoDec()
{
	mfc_destory();
}	

int SamsVideoDec::setInputData(unsigned char* inBuffer, unsigned int inSize)
{
	return mfc_dec_slice(inBuffer, inSize);
}

unsigned char* SamsVideoDec::getOutputData(unsigned int* outSize)
{
	return mfc_get_yuv(outSize);
}
int SamsVideoDec::init(unsigned char* inExt, unsigned int inSize)
{
	m_pHeader = inExt;
	m_nHeaderSize = inSize;
	return mfc_create();
}
int SamsVideoDec::deinit()
{
	return mfc_destory();
}
int SamsVideoDec::setParam(unsigned int inID, void* inValue)
{
	return onSetParam(inID, inValue);
}
int  SamsVideoDec::getParam(unsigned int inID, void* outValue)
{
	return onGetParam(inID, outValue);
}


int SamsVideoDec::onSetParam(unsigned int inID, void* inValue)
{
	return -1;
}
int SamsVideoDec::onGetParam(unsigned int inID, void* outValue)
{
	return -1;
}
int  SamsVideoDec::mfc_create(void)
{
	return -1;
}
int  SamsVideoDec::mfc_destory()
{
	return -1;
}
int  SamsVideoDec::mfc_dec_slice (unsigned char* data, unsigned int size)
{
	return -1;
}
unsigned char*	SamsVideoDec::mfc_get_yuv   (unsigned int* out_size)
{
	return NULL;
}
int SamsVideoDec::mfc_flag_video_frame(unsigned char* data, int size)
{
	return -1;
}

bool SamsVideoDec::setCodecType(int nType)
{
	if(nType != m_mfc_codec_type)
	{
		m_mfc_codec_type = nType;
		return true;
	}
	return false;
}

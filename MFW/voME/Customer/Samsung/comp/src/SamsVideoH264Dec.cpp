#include <stdlib.h>
#include <utils/Log.h>
#include "SamsVideoH264Dec.h"

#ifndef __SAMSUNG_SYSLSI_APDEV_MFCLIB_SSBSIPH264DECODE_H__
#include "SsbSipH264Decode.h"
#endif

#define LOG_TAG "SamsVideoH264Dec"

SamsVideoH264Dec::SamsVideoH264Dec()
{
	delimiter_h264[0] = 0x00;
	delimiter_h264[1] = 0x00;
	delimiter_h264[2] = 0x00;
	delimiter_h264[3] = 0x01;

}
SamsVideoH264Dec::~SamsVideoH264Dec()
{
	
}

int SamsVideoH264Dec::onSetParam(unsigned int inID, void* inValue)
{
	if(m_mfc_handle)
	{
		return SsbSipH264DecodeSetConfig(m_mfc_handle, inID, inValue);
	}
	return -1;
}

int SamsVideoH264Dec::onGetParam(unsigned int inID, void* outValue)
{
	if(m_mfc_handle)
	{
		return SsbSipH264DecodeGetConfig(m_mfc_handle, inID, outValue);
	}
	return -1;
}

int SamsVideoH264Dec::mfc_create(void)
{
	if(m_mfc_flag_create == 1)
	{
		LOGE("mfc_create aleady done \n");
		return 0;
	}

	m_mfc_handle = SsbSipH264DecodeInit();
	if (m_mfc_handle == NULL)
	{
		LOGE("SsbSipH264DecodeInit Failed.\n");
		return -1;
	}

	m_mfc_buffer_base = (unsigned char*)SsbSipH264DecodeGetInBuf(m_mfc_handle, 0);

	m_mfc_buffer_now    = m_mfc_buffer_base;
	m_mfc_buffer_size   = 0;	
	m_mfc_flag_info_out = 0;
	m_mfc_flag_create   = 1;
					
    return 0;
}

int SamsVideoH264Dec::mfc_destory()
{
	if(m_mfc_flag_create == 0)
	{
		return 0;
	}

	if(SsbSipH264DecodeDeInit(m_mfc_handle) < 0)
	{
		LOGE("SsbSipH264DecodeDeInit\n");
		return -1;
	}

	m_mfc_buffer_base   = NULL;
	m_mfc_buffer_now    = NULL;
	m_mfc_buffer_size   = 0;
	m_mfc_flag_info_out = 0;
	m_mfc_flag_create   = 0;
	return 0;
}

int SamsVideoH264Dec::mfc_dec_slice (unsigned char* data, unsigned int size)
{
	SSBSIP_H264_STREAM_INFO avcd_info;
	int flag_video_frame = 0;

	if(m_mfc_flag_create == 0)
	{
		LOGE("mfc codec not yes created \n");
		return -1;
	}

	flag_video_frame = mfc_flag_video_frame(data, size);
	if(flag_video_frame < 0)
	{
		LOGE("mfc_flag_video_frame error \n");
		return -1;
	}
	
	if(flag_video_frame == 1)
	{
		// since our parser has been add these byte, no necessary do that.
/*
		memcpy(m_mfc_buffer_now, delimiter_h264, 4);
		m_mfc_buffer_now  += 4;
		m_mfc_buffer_size += 4;
*/
		// set header into the buffer
		if(!m_bSetHeader && m_pHeader)
		{
			memcpy(m_mfc_buffer_now, m_pHeader, m_nHeaderSize);
			m_mfc_buffer_now  += m_nHeaderSize;
			m_mfc_buffer_size += m_nHeaderSize;
			m_bSetHeader = true;
		}

		memcpy(m_mfc_buffer_now, data, size);
		m_mfc_buffer_now  += size;
		m_mfc_buffer_size += size;
	
		if(m_mfc_flag_info_out == 0)
		{
			if(SsbSipH264DecodeExe(m_mfc_handle, m_mfc_buffer_size) < 0)
			{
				LOGE("SsbSipH264DecodeExe for GetConfig fail \n");
				return -1;
			}

			if(SsbSipH264DecodeGetConfig(m_mfc_handle, H264_DEC_GETCONF_STREAMINFO, &avcd_info) < 0)
			{
				LOGE("SsbSipH264DecodeGetConfig fail\n");
				return -1;
			}
			
			iDisplay_Width  = avcd_info.width;
			iDisplay_Height = avcd_info.height;

			m_mfc_flag_info_out =  1;
		}

		if(SsbSipH264DecodeExe(m_mfc_handle, m_mfc_buffer_size) < 0)
		{
			LOGE("SsbSipH264DecodeExe(Main) fail \n");
			return -1;
		}

		m_mfc_buffer_now  = m_mfc_buffer_base;
		m_mfc_buffer_size = 0;
	}
	else
	{	
		/*
		memcpy(m_mfc_buffer_now, delimiter_h264, 4);
		m_mfc_buffer_now  += 4;
		m_mfc_buffer_size += 4;
		*/

		memcpy(m_mfc_buffer_now, data, size);
		m_mfc_buffer_now  += size;
		m_mfc_buffer_size += size;

		return 0;
	}
	
	return 1;
}
unsigned char*	SamsVideoH264Dec::mfc_get_yuv   (unsigned int* out_size)
{
	unsigned char*	pYUVBuf = NULL;	// memory address of YUV420 Frame Buffer
	long	        nYUVLeng = 0;	// size of frame buffer	

	pYUVBuf = (unsigned char *)SsbSipH264DecodeGetOutBuf(m_mfc_handle, &nYUVLeng);

	if(pYUVBuf)
	{
		*out_size = (unsigned int)nYUVLeng;
	}
	else
	{
		*out_size  = 0;
	}

	return pYUVBuf;
}
int SamsVideoH264Dec::mfc_flag_video_frame(unsigned char* data, int size)
{
	int flag_video_frame = 0; // 0 : only header  1 : I frame include
	int forbidden_zero_bit;

	if (size > 0)
	{
		forbidden_zero_bit = data[4] >> 7;
		if (forbidden_zero_bit != 0)
			return -1;

		if(	   1 == (data[4] & 0x1F)   // NALTYPE_SLICE
			|| 5 == (data[4] & 0x1F))  // NALTYPE_IDR
		{
			flag_video_frame = 1;
		}
		else
			flag_video_frame = 0;
	}

	return flag_video_frame;
}

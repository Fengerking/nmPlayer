#include <stdlib.h>
#include <utils/Log.h>

#ifndef __SAMSUNG_SYSLSI_APDEV_MFCLIB_SSBSIPMPEG4DECODE_H__
#  include "SsbSipMpeg4Decode.h"
#endif

#include "SamsVideoMP4Dec.h"

#define LOG_TAG "SamsVideoMP4Dec"
SamsVideoMP4Dec::SamsVideoMP4Dec()
{
}
SamsVideoMP4Dec::~SamsVideoMP4Dec()
{
	
}

int SamsVideoMP4Dec::onSetParam(unsigned int inID, void* inValue)
{
	if(m_mfc_handle)
	{
		return SsbSipMPEG4DecodeSetConfig(m_mfc_handle, inID, inValue);
	}
	return -1;
}

int SamsVideoMP4Dec::onGetParam(unsigned int inID, void* outValue)
{
	if(m_mfc_handle)
	{
		return SsbSipMPEG4DecodeGetConfig(m_mfc_handle,inID, outValue);
	}
	return -1;
}

int SamsVideoMP4Dec::mfc_create(void)
{
	if(m_mfc_flag_create == 1)
	{
		LOGE("mfc_create aleady done \n");
		return 0;
	}

	m_mfc_handle = SsbSipMPEG4DecodeInit();
	if (m_mfc_handle == NULL)
	{
		LOGE("SsbSipMPEG4DecodeInit Failed.\n");
		return -1;
	}

	m_mfc_buffer_base = (unsigned char*)SsbSipMPEG4DecodeGetInBuf(m_mfc_handle, 0);

	m_mfc_buffer_now    	= m_mfc_buffer_base;
	m_mfc_buffer_size   	= 0;	
	m_mfc_flag_info_out 	= 0;
	m_mfc_flag_create   	= 1;
	m_mfc_flag_vol_found	= 0;
					
    return 0;
}

int SamsVideoMP4Dec::mfc_destory()
{
	if(m_mfc_flag_create == 0)
	{
		return 0;
	}

	if(SsbSipMPEG4DecodeDeInit(m_mfc_handle) < 0)
	{
		LOGE("SsbSipMPEG4DecodeDeInit\n");
		return -1;
	}

	m_mfc_buffer_base   	= NULL;
	m_mfc_buffer_now    	= NULL;
	m_mfc_buffer_size   	= 0;
	m_mfc_flag_info_out 	= 0;
	m_mfc_flag_create   	= 0;
	m_mfc_flag_vol_found	= 0;
	return 0;
}

int SamsVideoMP4Dec::mfc_dec_slice (unsigned char* data, unsigned int size)
{
	SSBSIP_MPEG4_STREAM_INFO m4vd_info;
	int flag_video_frame = 0;
	unsigned char* pData;
	int i;
	bool bFindVop = false;

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
	
		// RainAde : add for debugging (case: VOL -> VOL+VOP -> VOP... )
		if(m_mfc_flag_vol_found == 1)
		{
			if(m_mfc_flag_info_out == 0)
			{
				if(SsbSipMPEG4DecodeExe(m_mfc_handle, m_mfc_buffer_size) < 0)
				{
					LOGE("SsbSipMPEG4DecodeExe for GetConfig fail \n");
					return -1;
				}

				if(SsbSipMPEG4DecodeGetConfig(m_mfc_handle, MPEG4_DEC_GETCONF_STREAMINFO, &m4vd_info) < 0)
				{
					LOGE("SsbSipMPEG4DecodeGetConfig fail\n");
					return -1;
				}
			
				iDisplay_Width  = m4vd_info.width;
				iDisplay_Height = m4vd_info.height;

				m_mfc_flag_info_out =  1;
			}

			if(SsbSipMPEG4DecodeExe(m_mfc_handle, m_mfc_buffer_size) < 0)
			{
				LOGE("SsbSipMPEG4DecodeExe(Main) fail \n");
				return -1;
			}

			m_mfc_buffer_now  = m_mfc_buffer_base;
			m_mfc_buffer_size = 0;
		}
		else	// RainAde : add for debugging (case: VOP without VOL ) 
		{
			LOGE("Receieved VOP before getting VOL header \n");
			return -1;
		}
	}
	else
	{		
		// case: VOL -> VOL+VOP -> VOP... 
		if(m_mfc_flag_vol_found == 0)
		{
			memcpy(m_mfc_buffer_now, data, size);
			m_mfc_buffer_now  += size;
			m_mfc_buffer_size += size;
			
			m_mfc_flag_vol_found = 1;
		}
		else
		{
			for(pData = data, i = 0; i < (size-3); i++, pData++)
			{
				if(0x00 == *pData)
					if(0x00 == *(pData+1))
						if(0x01 == *(pData+2))
							if(0xb3 == *(pData+3) || 0xb6 == *(pData+3))
							{
								bFindVop = true;
								break;
							}
			}

			if(bFindVop)
			{
				memcpy(m_mfc_buffer_now, pData, size-i);
				m_mfc_buffer_now  += (size-i);
				m_mfc_buffer_size += (size-i);
			
				// RainAde : add for debugging (case: VOL -> VOL+VOP -> VOP... )
				if(m_mfc_flag_info_out == 0)
				{
					if(SsbSipMPEG4DecodeExe(m_mfc_handle, m_mfc_buffer_size) < 0)
					{
						LOGE("SsbSipMPEG4DecodeExe for GetConfig fail \n");
						return -1;
					}

					if(SsbSipMPEG4DecodeGetConfig(m_mfc_handle, MPEG4_DEC_GETCONF_STREAMINFO, &m4vd_info) < 0)
					{
						LOGE("SsbSipMPEG4DecodeGetConfig fail\n");
						return -1;
					}
			
					iDisplay_Width  = m4vd_info.width;
					iDisplay_Height = m4vd_info.height;

					m_mfc_flag_info_out =  1;
				}

				if(SsbSipMPEG4DecodeExe(m_mfc_handle, m_mfc_buffer_size) < 0)
				{
					LOGE("SsbSipMPEG4DecodeExe(Main 2) fail \n");
					return -1;
				}
			}
			
			m_mfc_buffer_now  = m_mfc_buffer_base;
			m_mfc_buffer_size = 0;

			return 1;
		}

		return 0;
	}
	
	return 1;
}

unsigned char*	SamsVideoMP4Dec::mfc_get_yuv   (unsigned int* out_size)
{
	unsigned char*	pYUVBuf = NULL;	// memory address of YUV420 Frame Buffer
	long	        nYUVLeng = 0;	// size of frame buffer	

	pYUVBuf = (unsigned char *)SsbSipMPEG4DecodeGetOutBuf(m_mfc_handle, &nYUVLeng);	

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
int SamsVideoMP4Dec::mfc_flag_video_frame(unsigned char* data, int size)
{
	int flag_video_frame = 0; // 0 : only header  1 : I frame include

	switch(m_mfc_codec_type)
	{
	case MFC_CODEC_MP4D:
	{
		if (4 <= size)
		{
			if(0xb6 == (int)data[3] || 0xb3 == (int)data[3])
				flag_video_frame = 1;
			else
				flag_video_frame = 0;
		}
		else
			return -1;

		break;
	}
	case MFC_CODEC_H263D:
	{
		flag_video_frame = 1;
		m_mfc_flag_vol_found = 1;
		break;
	}
	default:
		break;
	}

	return flag_video_frame;
}

void SamsVideoDec::getVideoInfo(long& outWidth, long& outHeight)
{
	outWidth  = iDisplay_Width;
	outHeight = iDisplay_Height;
}

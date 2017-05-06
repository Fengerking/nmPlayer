/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2010	    *
*									                                    *
************************************************************************/
/************************************************************************
File:		voAPE_HeadParser.c

Contains:	Parser APE file Header format, get APE information

Description:    ID3v2(Optional) + APE_DESCRIPTOR(52Bytes) + APEHeader(24Bytes) 
                + nSeekTableBytes(7284) + ..... + Raw Data + ID3v1(Optional) 
				+ APE Tag(Optional) 

Written by:	Huaping Liu

Change History (most recent first):
2009-11-11		LHP			Create file

**************************************************************************/
#include "voAPETag.h"
#include "voAPE_Global.h"

int voAPEHeaderParser(VOPTR hCodec)
{
	voAPE_GData        *gData;
	voAPE_DESCRIPTOR   *ape_desc;
	voAPE_HEADER       *ape_hdr;
	voAPE_HEADER_OLD   *ape_hdr_old;
	voAPE_FILE_INFO    *ape_info;
	FrameStream        *stream_buf; 

	gData = (voAPE_GData *)hCodec;

	ape_desc    = (voAPE_DESCRIPTOR *)gData->ape_desc;
	ape_hdr     = (voAPE_HEADER *)gData->ape_hdr;
	ape_hdr_old = (voAPE_HEADER_OLD *)gData->ape_hdr_old;
	ape_info    = (voAPE_FILE_INFO *)gData->ape_info;
	stream_buf  = (FrameStream *)gData->stream_buf;

	if(stream_buf->framebuffer_len >= 52)
	{
		memcpy(ape_desc, stream_buf->frame_ptr, sizeof(voAPE_DESCRIPTOR));
		stream_buf->frame_ptr += sizeof(voAPE_DESCRIPTOR);
		stream_buf->framebuffer_len  -= sizeof(voAPE_DESCRIPTOR);
		//stream_buf->used_len   += sizeof(voAPE_DESCRIPTOR);
	}

	if(ape_desc->nDescriptorBytes > sizeof(voAPE_DESCRIPTOR))
	{
		stream_buf->frame_ptr += (ape_desc->nDescriptorBytes - sizeof(voAPE_DESCRIPTOR));
		stream_buf->framebuffer_len  -= (ape_desc->nDescriptorBytes - sizeof(voAPE_DESCRIPTOR));
		//stream_buf->used_len  += sizeof(voAPE_DESCRIPTOR);
	}

	if(stream_buf->framebuffer_len  >= 24)
	{
		memcpy(ape_hdr, stream_buf->frame_ptr, sizeof(voAPE_HEADER));
		stream_buf->frame_ptr += sizeof(voAPE_HEADER);
		stream_buf->framebuffer_len  -= sizeof(voAPE_HEADER);
		//stream_buf->used_len  += sizeof(voAPE_HEADER);
	}
	if(ape_desc->nHeaderBytes > sizeof(voAPE_HEADER))
	{
		stream_buf->frame_ptr += (ape_desc->nHeaderBytes - sizeof(voAPE_HEADER));
		stream_buf->framebuffer_len  -= (ape_desc->nHeaderBytes - sizeof(voAPE_HEADER));
		//stream_buf->used_len  += sizeof(voAPE_HEADER);
	}

	//fill the APE info structure
	ape_info->nVersion               = (int)ape_desc->nVersion;
	ape_info->nCompressionLevel      = (int)ape_hdr->nCompressionLevel;
	ape_info->nFormatFlags           = (int)ape_hdr->nFormatFlags;
	ape_info->nTotalFrames           = (int)ape_hdr->nTotalFrames;
	ape_info->nFinalFrameBlocks      = (int)ape_hdr->nFinalFrameBlocks;
	ape_info->nBlocksPerFrame        = (int)ape_hdr->nBlocksPerFrame;
	ape_info->nChannels              = (int)ape_hdr->nChannels;
	ape_info->nSampleRate            = (int)ape_hdr->nSampleRate;
	ape_info->nBitsPerSample         = (int)ape_hdr->nBitsPerSample;
	ape_info->nBytesPerSample        = ape_info->nBitsPerSample / 8;
	ape_info->nBlockAlign            = ape_info->nBytesPerSample * ape_info->nChannels;
	ape_info->nTotalBlocks           = (ape_hdr->nTotalFrames == 0) ? 0 : ((ape_hdr->nTotalFrames -  1) * ape_info->nBlocksPerFrame) + ape_hdr->nFinalFrameBlocks;
	//pInfo->nWAVHeaderBytes         = (APEHeader.nFormatFlags & MAC_FORMAT_FLAG_CREATE_WAV_HEADER) ? sizeof(WAVE_HEADER) : pInfo->spAPEDescriptor->nHeaderDataBytes;
	//pInfo->nWAVTerminatingBytes    = pInfo->spAPEDescriptor->nTerminatingDataBytes;
	//pInfo->nWAVDataBytes           = pInfo->nTotalBlocks * pInfo->nBlockAlign;
	//pInfo->nWAVTotalBytes          = pInfo->nWAVDataBytes + pInfo->nWAVHeaderBytes + pInfo->nWAVTerminatingBytes;
	//pInfo->nAPETotalBytes          = m_pIO->GetSize();
	ape_info->nLengthMS              = (int)((((double)ape_info->nTotalBlocks) * (double)1000) / ((double)ape_info->nSampleRate));
	ape_info->nAverageBitrate        = (ape_info->nLengthMS <= 0) ? 0 : (int)(((double)(ape_info->nAPETotalBytes) * (double)(8)) / (double)(ape_info->nLengthMS));
	ape_info->nDecompressedBitrate   = (ape_info->nBlockAlign * ape_info->nSampleRate * 8) / 1000;
	ape_info->nSeekTableElements     = ape_desc->nSeekTableBytes / 4;

	////get the seek tables(really no reason to get the whole thing if there's extra)
	//if(stream_buf->framebuffer_len  < ape_info->nSeekTableElements)
	//	printf("Have not enough Byte for fill the seek tables");

	////skip the seek tables
	//stream_buf->frame_ptr += ape_info->nSeekTableElements * 4;
	//stream_buf->framebuffer_len  -= ape_info->nSeekTableElements * 4;
	////stream_buf->used_len  += ape_info->nSeekTableElements * 4;

	//if(!ape_info->nFormatFlags)
	//{
	//	stream_buf->frame_ptr += 44;
	//	stream_buf->framebuffer_len  -= 44;
	//	//stream_buf->used_len  += 44;
	//}

	return  VO_ERR_NONE;
}

int voAPEMetaInfo(VOPTR hCodec, unsigned char* inbuf, int length)
{
	voAPE_GData        *gData;
	voAPE_DESCRIPTOR   *ape_desc;
	voAPE_HEADER       *ape_hdr;
	voAPE_HEADER_OLD   *ape_hdr_old;
	voAPE_FILE_INFO    *ape_info;


	gData = (voAPE_GData *)hCodec;
	ape_desc    = (voAPE_DESCRIPTOR *)gData->ape_desc;
	ape_hdr     = (voAPE_HEADER *)gData->ape_hdr;
	ape_hdr_old = (voAPE_HEADER_OLD *)gData->ape_hdr_old;
	ape_info    = (voAPE_FILE_INFO *)gData->ape_info;


	memcpy(ape_desc, inbuf, sizeof(voAPE_DESCRIPTOR));
	inbuf  += sizeof(voAPE_DESCRIPTOR);
	length -= sizeof(voAPE_DESCRIPTOR);

	if(ape_desc->nDescriptorBytes > sizeof(voAPE_DESCRIPTOR))
	{
		inbuf  += (ape_desc->nDescriptorBytes - sizeof(voAPE_DESCRIPTOR));
		length -= (ape_desc->nDescriptorBytes - sizeof(voAPE_DESCRIPTOR));
	}

    memcpy(ape_hdr, inbuf, sizeof(voAPE_HEADER));
	inbuf  += sizeof(voAPE_HEADER);
	length -= sizeof(voAPE_HEADER);


	if(ape_desc->nHeaderBytes > sizeof(voAPE_HEADER))
	{
		inbuf  += (ape_desc->nHeaderBytes - sizeof(voAPE_HEADER));
		length -= (ape_desc->nHeaderBytes - sizeof(voAPE_HEADER));
	}

	//fill the APE info structure
	ape_info->nVersion               = (int)ape_desc->nVersion;
	ape_info->nCompressionLevel      = (int)ape_hdr->nCompressionLevel;
	ape_info->nFormatFlags           = (int)ape_hdr->nFormatFlags;
	ape_info->nTotalFrames           = (int)ape_hdr->nTotalFrames;
	ape_info->nFinalFrameBlocks      = (int)ape_hdr->nFinalFrameBlocks;
	ape_info->nBlocksPerFrame        = (int)ape_hdr->nBlocksPerFrame;
	ape_info->nChannels              = (int)ape_hdr->nChannels;
	ape_info->nSampleRate            = (int)ape_hdr->nSampleRate;
	ape_info->nBitsPerSample         = (int)ape_hdr->nBitsPerSample;
	ape_info->nBytesPerSample        = ape_info->nBitsPerSample / 8;
	ape_info->nBlockAlign            = ape_info->nBytesPerSample * ape_info->nChannels;
	ape_info->nTotalBlocks           = (ape_hdr->nTotalFrames == 0) ? 0 : ((ape_hdr->nTotalFrames -  1) * ape_info->nBlocksPerFrame) + ape_hdr->nFinalFrameBlocks;
	ape_info->nLengthMS              = (int)((((double)ape_info->nTotalBlocks) * (double)1000) / ((double)ape_info->nSampleRate));
	ape_info->nAverageBitrate        = (ape_info->nLengthMS <= 0) ? 0 : (int)(((double)(ape_info->nAPETotalBytes) * (double)(8)) / (double)(ape_info->nLengthMS));
	ape_info->nDecompressedBitrate   = (ape_info->nBlockAlign * ape_info->nSampleRate * 8) / 1000;
	ape_info->nSeekTableElements     = ape_desc->nSeekTableBytes / 4;

	////get the seek tables(really no reason to get the whole thing if there's extra)
	//if(length  < ape_info->nSeekTableElements)
	//	printf("Have not enough Byte for fill the seek tables");

	////skip the seek tables
	//inbuf  += ape_info->nSeekTableElements * 4;
	//length -= ape_info->nSeekTableElements * 4;

	//if(!ape_info->nFormatFlags)
	//{
	//	inbuf  += 44;
	//	length -= 44;
	//}

	return  VO_ERR_NONE;
}

int voAPEIDv2_Parser(VOPTR hCodec)
{
	return  VO_ERR_NONE;
}

















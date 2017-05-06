

#ifndef __APE_DATASTRUCT_H__

#define __APE_DATASTRUCT_H__

#include "voType.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


#define MAC_WITH_WAV_HEADER    32    // wave header existing magic


struct APE_DESCRIPTOR_BLOCK
{
	VO_BYTE		tag[4];					//'M''A''C'' '
	VO_U16		file_version;
	VO_U32		descriptor_length;		//block size
	VO_U32		header_length;
	VO_U32		seektable_length;
	VO_U32		wavheader_length;
	VO_U32		audiodata_length_low;
	VO_U32		audiodata_length_high;
	VO_U32		wavtail_length;
	VO_BYTE		md5[16];
};

struct APE_HEADER_BLOCK
{
	VO_U16		compression_type;
	VO_U16		format_flag;
	VO_U32		blocks_per_frame;
	VO_U32		finalframeblocks;
	VO_U32		total_frames;
	VO_U16		bps;
	VO_U16		channels;
	VO_U32		sample_rate;
};

struct APE_INFO
{
	APE_DESCRIPTOR_BLOCK descriptor;
	APE_HEADER_BLOCK header;
};

struct APE_FRAME
{
	VO_S64		pos;
	VO_U32		blocks;
	VO_S32		offset;
	VO_S64		size;
	VO_S64		timestamp;
};

struct WAVE_HEADER
{
    // RIFF header
    char cRIFFHeader[4];
    unsigned int nRIFFBytes;

    // data type
    char cDataTypeID[4];

    // wave format
    char cFormatHeader[4];
    unsigned int nFormatBytes;

    unsigned short nFormatTag;
    unsigned short nChannels;
    unsigned int nSamplesPerSec;
    unsigned int nAvgBytesPerSec;
    unsigned short nBlockAlign;
    unsigned short nBitsPerSample;
    
    // data chunk header
    char cDataHeader[4];
    unsigned int nDataBytes;
};

#ifdef _VONAMESPACE
}
#endif

#endif

/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2010        *
*									                                    *
************************************************************************/
/***********************************************************************
File:		voAPE_Header.h

Contains:	APE Decoder Header Parser structure define		

Written by:	Huaping Liu

Change History (most recent first):
2009-11-11		LHP			Create file

************************************************************************/

#ifndef  __VOAPE_HEADER_H__
#define  __VOAPE_HEADER_H__

#include  "voTypedef.h"

/*****************************************************************************************
All structures are designed for 4-byte alignment
*****************************************************************************************/
#pragma pack(push)
#pragma pack(4)

typedef struct
{
	char   cID[4];                             /* should equal 'MAC ' */
	uint16 nVersion;                           /* version number * 1000 (3.81 = 3810) (remember that 4-byte alignment causes this to take 4-bytes) */
	uint32 nDescriptorBytes;                   /* the number of descriptor bytes (allows later expansion of this header) */
	uint32 nHeaderBytes;                       /* the number of header APE_HEADER bytes */
	uint32 nSeekTableBytes;                    /* the number of bytes of the seek table */
	uint32 nHeaderDataBytes;                   /* the number of header data bytes (from original file) */
	uint32 nAPEFrameDataBytes;                 /* the number of bytes of APE frame data */
	uint32 nAPEFrameDataBytesHigh;             /* the high order number of APE frame data bytes */
	uint32 nTerminatingDataBytes;              /* the terminating data of the file (not including tag data) */
	uint8  cFileMD5[16];                       /* the MD5 hash of the file (see notes for usage... it's a littly tricky) */
}voAPE_DESCRIPTOR;

typedef struct
{
	uint16 nCompressionLevel;                  /* the compression level (see defines I.E. COMPRESSION_LEVEL_FAST) */
	uint16 nFormatFlags;                       /* any format flags (for future use) */
	uint32 nBlocksPerFrame;                    /* the number of audio blocks in one frame */
	uint32 nFinalFrameBlocks;                  /* the number of audio blocks in the final frame */
	uint32 nTotalFrames;                       /* the total number of frames */
	uint16 nBitsPerSample;                     /* the bits per sample (typically 16) */
	uint16 nChannels;                          /* the number of channels (1 or 2) */
	uint32 nSampleRate;                        /* the sample rate (typically 44100) */
}voAPE_HEADER;

typedef struct 
{
	char cID[4];                            /* should equal 'MAC ' */
	uint16 nVersion;                        /* version number * 1000 (3.81 = 3810) */
	uint16 nCompressionLevel;               /* the compression level */
	uint16 nFormatFlags;                    /* any format flags (for future use) */
	uint16 nChannels;                       /* the number of channels (1 or 2) */
	uint32 nSampleRate;                     /* the sample rate (typically 44100) */
	uint32 nHeaderBytes;                    /* the bytes after the MAC header that compose the WAV header */
	uint32 nTerminatingBytes;               /* the bytes after that raw data (for extended info) */
	uint32 nTotalFrames;                    /* the number of frames in the file */
	uint32 nFinalFrameBlocks;               /* the number of samples in the final frame */
}voAPE_HEADER_OLD;

typedef struct
{
	int nVersion;                                   /* file version number * 1000 (3.93 = 3930) */
	int nCompressionLevel;                          /* the compression level */
	int nFormatFlags;                               /* format flags */
	int nTotalFrames;                               /* the total number frames (frames are used internally) */
	int nBlocksPerFrame;                            /* the samples in a frame (frames are used internally) */
	int nFinalFrameBlocks;                          /* the number of samples in the final frame */
	int nChannels;                                  /* audio channels */
	int nSampleRate;                                /* audio samples per second */
	int nBitsPerSample;                             /* audio bits per sample */
	int nBytesPerSample;                            /* audio bytes per sample */
	int nBlockAlign;                                /* audio block align (channels * bytes per sample) */
	int nWAVHeaderBytes;                            /* header bytes of the original WAV */
	int nWAVDataBytes;                              /* data bytes of the original WAV */
	int nWAVTerminatingBytes;                       /* terminating bytes of the original WAV */
	int nWAVTotalBytes;                             /* total bytes of the original WAV */
	int nAPETotalBytes;                             /* total bytes of the APE file */
	int nTotalBlocks;                               /* the total number audio blocks */
	int nLengthMS;                                  /* the length in milliseconds */
	int nAverageBitrate;                            /* the kbps (i.e. 637 kpbs) */
	int nDecompressedBitrate;                       /* the kbps of the decompressed audio (i.e. 1440 kpbs for CD audio) */
	int nJunkHeaderBytes;                           /* used for ID3v2, etc. */
	int nSeekTableElements;                         /* the number of elements in the seek table(s) */
	int nMD5Invalid;                                /* whether the MD5 is valid */
}voAPE_FILE_INFO;

int voAPEHeaderParser(void * hCodec);
int voAPEDecodeStep(void *hCodec);

#pragma pack(pop)

#endif   //__VOAPE_HEADER_H__

//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2007 VisualOn SoftWare Co., Ltd.

Module Name:

    alactest.c

Abstract:

    caller for ALAC decoder.

Author:

    Witten Wen 23-April-2010

Revision History:

*************************************************************************/

#include "stdio.h"
#include "cmnMemory.h"
#include "voALAC.h"
#include "alacstrmdec.h"
#include "headerparser.h"

#if defined(ARM)
//char *Sfilename = "Storage Card\\ALAC\\10s.m4a";
//char *Dfilename = "Storage Card\\ALAC\\10s_arm.wav";
char *Sfilename = "ResidentFlash\\ALAC\\10s.m4a";
char *Dfilename = "ResidentFlash\\ALAC\\10s_arm.wav";
#else
char *Sfilename = "input.m4a";
char *Dfilename = "output.wav";
#endif
VO_AUDIO_FORMAT pcmformat;

#define OUTWAV	1

#if OUTWAV
FILE *faudio = NULL;
int wav_size = 0;
__inline void shutdown_output ( void ) 
{
	printf("Shutting Down Sound System\n");
	if (faudio != NULL)
	{
		fclose(faudio);
		faudio = NULL;
	}
}

int
open_wav_output ( char *file_name ) {

	char wav_hdr[] = { 0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00,
				0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
				0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
				0x44, 0xAC, 0x00, 0x00, 0x10, 0xB1, 0x02, 0x00,
				0x04, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61,
				0x00, 0x00, 0x00, 0x00 };

	if (file_name[0] == '\0')
		return -1;
	//if ((faudio = open(wav_file, (O_RDWR | O_CREAT | O_TRUNC | O_BINARY))) < 0)
	if ((faudio = fopen(file_name,  "wb")) == NULL)
	{
		return -1;
	} else {
		unsigned long int bytes_per_sec, bytes_per_sample;
		
//		printf("open output file success!\n");
		//chaanel count
		wav_hdr[22] = (char)((pcmformat.Channels) & 0xFF);
		//sample rate
		wav_hdr[24] = (char)((pcmformat.SampleRate) & 0xFF);
		wav_hdr[25] = (char)((pcmformat.SampleRate >> 8) & 0xFF);
		wav_hdr[26] = (char)((pcmformat.SampleRate >> 16) & 0xFF);
		wav_hdr[27] = (char)((pcmformat.SampleRate >> 24) & 0xFF);
		//data remited rate 
		bytes_per_sample = (pcmformat.SampleBits * pcmformat.Channels) >> 3;
		bytes_per_sec = pcmformat.SampleRate * bytes_per_sample;
		wav_hdr[28] = (char)((bytes_per_sec) & 0xFF);
		wav_hdr[29] = (char)((bytes_per_sec >> 8) & 0xFF);
		wav_hdr[30] = (char)((bytes_per_sec >> 16) & 0xFF);
		wav_hdr[31] = (char)((bytes_per_sec >> 24) & 0xFF);
		//byte count per sample
		wav_hdr[32] = (char)((bytes_per_sample) & 0xFF);
		//sample bits
		wav_hdr[34] = (char)((pcmformat.SampleBits) & 0xFF);
	}
	wav_size = fwrite(&wav_hdr, 44, 1, faudio);
	if ( wav_size != 1) 
	{
		shutdown_output();
		return -1;
	}

	wav_size = 0;
//	send_output = write_wav_output;
//	close_output = close_wav_output;
	return 0;
}

int
write_wav_output (unsigned char * output_data, int output_size) 
{
	if (fwrite(output_data, output_size, 1, faudio) != 1) 
	{
		shutdown_output();
		return -1;
	}

	wav_size += output_size;
//	if(wav_size >= 0x380000)
//		wav_size = wav_size+2-2;
	return 0;
}

void
close_wav_output ( void ) {
	char wav_count[4];
	if (faudio == NULL)
		return;

	wav_count[0] = (char)((wav_size) & 0xFF);
	wav_count[1] = (char)((wav_size >> 8) & 0xFF);
	wav_count[2] = (char)((wav_size >> 16) & 0xFF);
	wav_count[3] = (char)((wav_size >> 24) & 0xFF);
	fseek(faudio, 40, SEEK_SET);
	fwrite(&wav_count, 4, 1, faudio);

	wav_size += 36;
	wav_count[0] = (char)((wav_size) & 0xFF);
	wav_count[1] = (char)((wav_size >> 8) & 0xFF);
	wav_count[2] = (char)((wav_size >> 16) & 0xFF);
	wav_count[3] = (char)((wav_size >> 24) & 0xFF);
	fseek(faudio, 4, SEEK_SET);
	fwrite(&wav_count, 4, 1, faudio);
//	printf("out wave is done!");
	shutdown_output();
}
#endif //OUTWAV

void alacreadstream(FILE *pfSrc, int size, char *pBuffer)
{
	fread(pBuffer, 1, size, pfSrc);
}

VO_U32 readU32(FILE *pfSrc)
{
	VO_U32	value;
	alacreadstream(pfSrc, 4, &value);
	return SWAPBYTE32(value);
}

void skipstream(FILE *pfSrc, int len)
{
	fseek(pfSrc, len, SEEK_CUR);
}

#ifdef WINCE
int wmain(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	FILE *pfSrc = NULL;
	FILE *pfDes = NULL;
    VO_PBYTE pbSrc = NULL;
	VO_U32	remainsize=0;
    int cbDst = 0;
    VO_PBYTE pbDst = NULL;
	int	outbuffersize;
	int	i, t1, t2;
	int total = 0;
	VO_U32 read_size = 0;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;
	VO_U32	returnCode;
	VO_HANDLE hCodec = 0;
	VO_AUDIO_CODECAPI pDecHandle;
	VO_WAVEFORMATEX	*WAVFormat = NULL;
	VO_U8 *phead = NULL;
	VO_MEM_OPERATOR *pmemoper;
	VO_CODEC_INIT_USERDATA userData;

	/* alac needed. */
	VO_U32	chunk_len = 0;
	VO_U32	chunk_id = 0;
	CALACObjectDecoder *palacdec=NULL;
	VO_U32	Header_OK = 0;
	HeadParse headparse;
	
	phead = (VO_U8 *)malloc(18 + 238);//max Codec Data size is 238;
	memset(phead, 0, 256);
	headparse.ALACFormat = (VO_WAVEFORMATEX	*)phead;
	if(argc<=1 && argv[1] == NULL)
	{
		if((pfSrc = fopen(Sfilename, "rb")) == NULL)goto gCleanup;
	}
	else
	{
		if((pfSrc = fopen(argv[1], "rb")) == NULL)goto gCleanup;
	}

	cmnMemFillPointer(VO_AUDIO_CodingWMA);
	pmemoper = &g_memOP;
	
	userData.memData = pmemoper;
	userData.memflag = VO_IMF_USERMEMOPERATOR;
	returnCode = voGetALACDecAPI(&pDecHandle);
	if(returnCode != VO_ERR_NONE)
		goto gCleanup;

	returnCode = pDecHandle.Init(&hCodec, VO_AUDIO_CodingALAC, &userData);
	palacdec = (CALACObjectDecoder *)hCodec;
	while(1)
	{
		chunk_len = readU32(pfSrc);
		chunk_id = readU32(pfSrc);
		switch(chunk_id)
		{
		case MAKEFOURC('f', 't', 'y', 'p'):
			{
				VO_U32 type =  readU32(pfSrc);
				remainsize = chunk_len - 8;				
				remainsize -= 4;
				if(type != MAKEFOURC('M', '4', 'A', ' '))
					goto gCleanup;
				while(remainsize)
				{
					readU32(pfSrc);
					remainsize -= 4;
				}
				break;
			}
		case MAKEFOURC('m', 'o', 'o', 'v'):
			{
				remainsize = chunk_len - 8;
				pbSrc = (VO_PBYTE)malloc(remainsize);
				alacreadstream(pfSrc, remainsize, pbSrc);
//				inData.Buffer = pbSrc;
//				inData.Length = remainsize;
//				returnCode = pDecHandle.SetParam(hCodec, VO_PID_COMMON_HEADDATA, &inData);
				returnCode = ParseHeaderMoov(palacdec, &headparse, pbSrc, remainsize);
				if(returnCode < 0)
					goto gCleanup;
				if(pbSrc)
				{
					free(pbSrc); 
					pbSrc = NULL;
				}				
			}
			break;
		case MAKEFOURC('m', 'd', 'a', 't'):
			{
				/* parse the mdat trunk */
				remainsize = chunk_len-8;
				pbSrc = (VO_PBYTE)malloc(remainsize);
				alacreadstream(pfSrc, remainsize, pbSrc);
				Header_OK = 1;
				break;
			}
		case MAKEFOURC('f', 'r', 'e', 'e'):
			{
				remainsize = chunk_len-8;
				skipstream(pfSrc, remainsize);
				break;
			}
		default:
			{
				remainsize = chunk_len - 8;
				while(remainsize)
				{
					readU32(pfSrc);
					remainsize -= 4;
				}
			}
			break;
		}
		if(Header_OK == 1)
			break;
	}
	inData.Buffer = (VO_PBYTE)headparse.ALACFormat;
	inData.Length = (VO_U32)headparse.ALACFormat->cbSize + 18;

	returnCode = pDecHandle.SetParam(hCodec, VO_PID_COMMON_HEADDATA, &inData);
	if(returnCode < 0)
		goto gCleanup;

	returnCode = pDecHandle.GetParam(hCodec, VO_PID_ALAC_OUTBUFFERSIZE, &outbuffersize);
	if(returnCode < 0)
		goto gCleanup;
	pbDst = (VO_PBYTE)malloc(outbuffersize);

	returnCode = pDecHandle.GetParam(hCodec, VO_PID_AUDIO_FORMAT, &pcmformat);
	if(returnCode < 0)
		goto gCleanup;
#if	OUTWAV
	if(argc <= 1 || argv[2] == NULL)
	{
		if (open_wav_output(Dfilename) == -1)goto gCleanup;
	}
	else
		if (open_wav_output(argv[2]) == -1)goto gCleanup;
#endif
	
	/* processing */	
	inData.Buffer = pbSrc;
	inData.Length = 0;
	for(i=0; i<headparse.m_NumSampleByteSize; i++)
	{
		inData.Buffer +=  inData.Length;
		inData.Length = headparse.m_pSampleByteSize[i];
		returnCode = pDecHandle.SetInputData(hCodec, &inData);
		if(returnCode != VO_ERR_NONE)
			break;
		do
		{
			outData.Buffer = pbDst;
			outData.Length = outbuffersize;
			returnCode = pDecHandle.GetOutputData(hCodec, &outData, &outFormat);
			if(returnCode == VO_ERR_NONE)
			{
#if OUTWAV
				write_wav_output(outData.Buffer, outData.Length);
#endif
//				datalength += outData.Length;
			}
			else if(returnCode == VO_ERR_INPUT_BUFFER_SMALL)
			{
				break;
			}
		}while(1);
	}
gCleanup:
		pDecHandle.Uninit(hCodec);
		FreeALACParser(&headparse);
		close_wav_output();
		if(pbSrc)
			free(pbSrc);
		if(pbDst)
			free(pbDst);
		if(phead)
			free(phead);
		return 0;
}
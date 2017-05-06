//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

voSBCDecSample.c

Abstract:

caller for SBC decoder.

Author:

Witten Wen 14-December-2009

Revision History:

*************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#ifdef LINUX
#include <dlfcn.h>
#endif

#include "cmnmemory.h"
#include "voSBC.h"
#include "sbctypes.h"
#include "sbc.h"

VO_AUDIO_FORMAT pcmformat;

#define OUTWAV	1

#if OUTWAV
FILE *faudio=NULL;
unsigned long int wav_size;
__inline void shutdown_output ( void ) 
{
	printf("Shutting Down Sound System\n");
	if (faudio != NULL)
		fclose(faudio);
}

int
open_wav_output ( char *file_name )
{

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

int SetPCMFormat()
{
	unsigned long int bytes_per_sec;
	short wav_channel, bytes_per_sample, samplebits;

	/* channel */
	wav_channel = (short)(pcmformat.Channels);
	fseek(faudio, 22, SEEK_SET);//占两个字节
	fwrite(&wav_channel, 2, 1, faudio);//22bits

	/* sample rate */
	fwrite(&pcmformat.SampleRate, 4, 1, faudio);//24bits

	//data remited rate 
	bytes_per_sample = (pcmformat.SampleBits * pcmformat.Channels) >> 3;
	bytes_per_sec = pcmformat.SampleRate * bytes_per_sample;
	fwrite(&bytes_per_sec, 4, 1, faudio);//28

	//byte count per sample
	fwrite(&bytes_per_sample, 2, 1, faudio);//32

	//sample bits
	samplebits = (short)pcmformat.SampleBits;
	fwrite(&samplebits, 2, 1, faudio);//34

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
#endif	//OUTWAV

typedef int (VO_API * VOGETAUDIOENCAPI) (VO_AUDIO_CODECAPI * pDecHandle);

#define OUTSIZE	2<<14
#define INSIZE	1<<10
void main(int argc, char **argv)
{
	FILE *hSrc = NULL, *hDst = NULL;
	char *pSrc = NULL, *fileend, *pfile;
	char *pDst = NULL;
	sbc_t *psbcdec = NULL;
	int filesize = 0, written, inputsize=0, consumed=0, returnCode;
	int FrameCount = 0;
	/* VidsualOn define */
	VO_AUDIO_FORMAT voauFormat;
	VO_CODEC_INIT_USERDATA userData;
	VO_AUDIO_CODECAPI pDecHandle;
	VO_CODECBUFFER inData, OutData;
	VO_MEM_OPERATOR *pSBCMemOP = NULL;
	VO_HANDLE hCodec = 0;
	VO_AUDIO_OUTPUTINFO pOutInfo;

#ifdef LINUX
	void  *handle;
	void  *pfunc;
	VOGETAUDIOENCAPI pGetAPI;
#endif

	/* Open the SBC file. */
	if(argc == 1 || argv[1] == NULL)
		goto FailOut;
	if((hSrc = fopen(argv[1], "rb")) == NULL)
		goto FailOut;

	/* Get file size */
	fseek(hSrc, 0, SEEK_END);
	filesize = ftell(hSrc);
	fseek(hSrc, 0, SEEK_SET);

	/* Allocate Buffer and read SBC data. */
	pfile = (char *)malloc(filesize);
	pDst = (char *)malloc(OUTSIZE);
	if(fread(pfile, 1, filesize, hSrc) != filesize)
	{
		fclose(hSrc);
		goto FailOut;
	}
	fclose(hSrc);
	fileend = pfile + filesize;
	pSrc = pfile;

	cmnMemFillPointer(VO_AUDIO_CodingSBC);
	pSBCMemOP = &g_memOP;
	userData.memflag = VO_IMF_USERMEMOPERATOR;
	userData.memData = pSBCMemOP;

#ifdef LINUX
	handle = dlopen("/data/local/tmp/libvoSBCDec.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}

	pfunc = dlsym(handle, "voGetSBCDecAPI");	
	if(pfunc == 0)
	{
		printf("open function error......");
		return -1;
	}

	pGetAPI = (VOGETAUDIOENCAPI)pfunc;

	returnCode  = pGetAPI(&pDecHandle);
	if(returnCode)
		return -1;
#else
	returnCode = voGetSBCDecAPI(&pDecHandle);
#endif

	/* Decoder Initiaization. */
	if(pDecHandle.Init(&hCodec, VO_AUDIO_CodingSBC, &userData)!=VO_ERR_NONE)
		goto FailOut;
	//	psbcdec = (sbc_t *)malloc(sizeof(sbc_t));
	//	if(sbc_init(psbcdec, 0) != 0)
	//		goto FailOut;

	psbcdec = (sbc_t *)hCodec;

	pcmformat.Channels		= psbcdec->m_Channels;
	pcmformat.SampleRate	= psbcdec->m_Rate;
	pcmformat.SampleBits	= 16;
	open_wav_output(argv[2]);
	inputsize = INSIZE;
	inData.Buffer = (VO_PBYTE)pSrc;
	inData.Length = inputsize;
	OutData.Buffer	= pDst;

	while(1)
	{
		if(pDecHandle.SetInputData(hCodec, &inData)!=VO_ERR_NONE)
			goto FailOut;
		while(1)
		{
			returnCode = pDecHandle.GetOutputData(hCodec, &OutData, &pOutInfo);
			if(returnCode == VO_ERR_NONE)
			{
				FrameCount++;
				write_wav_output(OutData.Buffer, OutData.Length);
			}
			else if(returnCode == VO_ERR_INPUT_BUFFER_SMALL)
				break;
			else 
				goto Finished;
		}
		inData.Buffer += inData.Length;
		if((int)inData.Buffer + INSIZE <= (int)fileend)
			inData.Length = INSIZE;
		else
		{
			if((int)inData.Buffer < (int)fileend)
				inData.Length = (int)fileend - (int)inData.Buffer;
			else
				break;
		}
	}
Finished:
	pcmformat.Channels = psbcdec->m_Channels;
	pcmformat.SampleRate = psbcdec->m_Rate;
	SetPCMFormat();
	//	sbc_finish(psbcdec);

FailOut:
	close_wav_output();
	pDecHandle.Uninit(hCodec);
	if(pfile)
		free(pfile);
	if(pDst)
		free(pDst);

#ifdef LINUX
	dlclose(handle);
#endif
	//	if(psbcdec)
	//		free(psbcdec);
}
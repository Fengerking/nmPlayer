//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2007 VisualOn SoftWare Co., Ltd.

Module Name:

    Main_test.cpp

Abstract:

    caller for WMA decoder.

Author:
	
    Witten Wen 12-July-2007

Revision History:

*************************************************************************/
#if		0
#ifndef LINUX
#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#else
#include <windows.h>
#endif // _WIN32_WCE
#endif	//LINUX

#include	<time.h>
#include	<stdio.h>
#include	<stdlib.h>
//#include <string.h>

#include "wmadec.h"
#include "wavfileexio.h"
#include "msaudiofmt.h"

#define ErrorMessage()

FILE *faudio;
unsigned long int wav_size;
static int rate = 44100;//11025;//16000;//44100;
#ifdef _WIN32_
static char *wav_file = "E:\\WMA\\wma10pro\\prj\\win32\\wma10pro_win32test\\output.wav";
#else
#ifndef LINUX
//static char *wav_file = "\\SDMMC\\wma\\output.wav";
//static char *wav_file = "\\wma\\output.wav";
static char *wav_file = "\\ResidentFlash\\wma\\output.wav";
//static char *wav_file = "E:\\wma10_rvds22\\wma10-rvds22\\output.wav";
#else
static char *wav_file = "output.wav";
#endif
#endif
__inline void shutdown_output ( void ) 
{
	printf("Shutting Down Sound System\n");
	if (faudio != NULL)
		fclose(faudio);
}

int
open_wav_output ( void ) {

	char wav_hdr[] = { 0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00,
				0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
				0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
				0x44, 0xAC, 0x00, 0x00, 0x10, 0xB1, 0x02, 0x00,
				0x04, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61,
				0x00, 0x00, 0x00, 0x00 };

	if (wav_file[0] == '\0')
		return -1;
	//if ((faudio = open(wav_file, (O_RDWR | O_CREAT | O_TRUNC | O_BINARY))) < 0)
	if ((faudio = fopen(wav_file,  "wb")) == NULL)
	{
		return -1;
	} else {
		unsigned long int bytes_per_sec;

		wav_hdr[24] = (rate) & 0xFF;
		wav_hdr[25] = (rate >> 8) & 0xFF;

		bytes_per_sec = rate * 4;
		wav_hdr[28] = (char)((bytes_per_sec) & 0xFF);
		wav_hdr[29] = (char)((bytes_per_sec >> 8) & 0xFF);
		wav_hdr[30] = (char)((bytes_per_sec >> 16) & 0xFF);
		wav_hdr[31] = (char)((bytes_per_sec >> 24) & 0xFF);
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
	return 0;
}

void
close_wav_output ( void ) {
	char wav_count[4];
	if (faudio == NULL)
		return;

	wav_count[0] = (wav_size) & 0xFF;
	wav_count[1] = (wav_size >> 8) & 0xFF;
	wav_count[2] = (wav_size >> 16) & 0xFF;
	wav_count[3] = (wav_size >> 24) & 0xFF;
	fseek(faudio, 40, SEEK_SET);
	fwrite(&wav_count, 4, 1, faudio);

	wav_size += 36;
	wav_count[0] = (wav_size) & 0xFF;
	wav_count[1] = (wav_size >> 8) & 0xFF;
	wav_count[2] = (wav_size >> 16) & 0xFF;
	wav_count[3] = (wav_size >> 24) & 0xFF;
	fseek(faudio, 4, SEEK_SET);
	fwrite(&wav_count, 4, 1, faudio);

	shutdown_output();
}

int wmain(int argc, char **argv)
{

 	WAVEFORMATEXTENSIBLE wfx;
    uint32 dwFrameSizeInSample;
    FILE *pfSrc = NULL;
	FILE *pfDes = NULL;
    uint8* pbSrc = NULL;
    int cbDst = 0;
    uint8* pbDst = NULL;
	LONG	outbuffersize;
	int		t1,t2;
	int total = 0;
	uint32 read_size = 0;
	uint32 g_iVersion = 0;
	VOCODECDATABUFFER inData;
	VOCODECDATABUFFER outData;
	VOCODECAUDIOFORMAT outFormat;
	VOWMADECRETURNCODE	returnCode;
	HVOCODEC hCodec = 0;
	WMAHeaderInfo	WMAFormat;
	TCHAR msg[256];
	int i=0, datalength=0, FPs=0;
	double performance=0;
//	int x = 1, y = 1;
//	int z = x^y;	//^Bitwise exclusive OR
#ifdef _WIN32_	
	char* szInputName     = "E:\\WMA\\wma10pro\\prj\\win32\\wma10pro_win32test\\input.cmp";
#else
    //TCHAR* szInputName     = TEXT("wmatest.cmp");
 //   char* szInputName     = "\\Storage Card\\wma\\wmatest.cmp";

 //   char* szOutputName    = "\\Storage Card\\wma\\wmatest.pcm";
 //   char* szInputName     = "\\wma\\input.cmp";
//		char* szInputName     = "\\SDMMC\\wma\\input.cmp";
    char* szInputName     = "\\ResidentFlash\\wma\\input.cmp";
 //   char* szInputName     = "E:\\wma10_rvds22\\wma10-rvds22\\input.cmp";
#endif

    
   
   // cmp format
    if((pfSrc = fopen(szInputName, "rb")) == NULL) {
        goto gCleanup;
    }

//	if(!(pfDes = fopen(szOutputName, "w+b"))){
	if (open_wav_output() == -1){
		goto gCleanup;
	}
    
    // read the header for initial parameters
//    if(fread(&(wfx.Format), sizeof(uint8), sizeof(WAVEFORMATEX), pfSrc) != sizeof(WAVEFORMATEX))
    if(fread(&(wfx.Format),0x12, 1, pfSrc) != 1)
    {
        goto gCleanup;
    }

	if (wfx.Format.cbSize == MSAUDIO1_WFX_EXTRA_BYTES) 
	{
		//WMA v1 bitstream
		g_iVersion = 1;
	}
	else if ((wfx.Format.cbSize == WMAUDIO2_WFX_EXTRA_BYTES) || (wfx.Format.cbSize == WMAUDIO2_WFX_EXTRA_BYTES + 22))
	{
		//WMA v2/v3 bitstream
		g_iVersion = 2;
	}
	else
	{
		goto gCleanup;
	}
	
	WMAFormat.nChannels = wfx.Format.nChannels;          
	WMAFormat.nSamplesPerSec = wfx.Format.nSamplesPerSec;     
	WMAFormat.nBlockAlign = wfx.Format.nBlockAlign;
	
	if (wfx.Format.wFormatTag == WAVE_FORMAT_PCM) // v1 or v2
	{
		WMAFormat.wFormatTag = (g_iVersion == 1) ? 0x0160 : 0x0161;
		WMAFormat.nValidBitsPerSample = 16;
		//WMAFormat.nChannelMask = WMAProDefaultChannelMask(WMAFormat.nChannels);
	}
	else if (wfx.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE) // v3
	{
		// Read additional information that might be in the header (multichannel/ 24 bit depth etc)
		if (fread(&(WMAFormat.nValidBitsPerSample), 1, sizeof(uint16), pfSrc) != sizeof(uint16)) {
			goto gCleanup;
		}
		
		if (fread(&(WMAFormat.nChannelMask), 1, sizeof(uint32), pfSrc) != sizeof(uint32)) {
			goto gCleanup;
		}
		
		if (fread(&(wfx.SubFormat), 1, sizeof(GUID), pfSrc) != sizeof(GUID)) {
			goto gCleanup;
		}
		
		WMAFormat.wFormatTag = (uint16) wfx.SubFormat.Data1;
		g_iVersion = 3;
	}
	else
	{
		goto gCleanup;
	}
	
	if (g_iVersion == 1)
	{
		uint16 wFrameSizeInSample;
		if (fread(&wFrameSizeInSample, 1, sizeof(uint16), pfSrc) != sizeof(uint16)) {
			goto gCleanup;
		}
		dwFrameSizeInSample = wFrameSizeInSample;
	}
	else 
	{
		if (fread(&dwFrameSizeInSample, 1, sizeof(uint32), pfSrc) != sizeof(uint32)) {
			goto gCleanup;
		}
	}
	

	if (fread(&WMAFormat.wEncodeOpt, 1, sizeof(uint16), pfSrc) != sizeof(uint16)) {
		goto gCleanup;
	}
	
	// after ENCOPT3_RTMBITS was added, wEncodeOpt < 512
	if (fread(&WMAFormat.nAvgBytesPerSec, sizeof(uint32), 1, pfSrc) != 1) {
		goto gCleanup;
	}

	returnCode = voWMADecInit(&hCodec);
	if(returnCode < 0)
		goto gCleanup;

	returnCode = voWMADecSetParameter(hCodec, VOID_WMA_PROTITRY, (LONG)&WMAFormat);
	if(returnCode < 0)
		goto gCleanup;

//	pbSrc = (uint8 *)malloc(WMAFormat.nBlockAlign *3 + 100);
	returnCode = voWMADecGetParameter(hCodec, VOID_WMA_OUTBUFFERSIZE, &outbuffersize);
	pbDst = (uint8 *)malloc(outbuffersize);
	outData.length = outbuffersize;
	pbSrc = (uint8 *)malloc(WMAFormat.nBlockAlign + 100);
	
//	read_size = fread(pbSrc, 1, WMAFormat.nBlockAlign *3, pfSrc);
	read_size = fread(pbSrc, 1, WMAFormat.nBlockAlign, pfSrc);
	if(!read_size)
	{
		goto gCleanup;
	}

	inData.buffer = pbSrc;
	inData.length = read_size;

	do{
		outData.buffer = pbDst;
		outData.length = outbuffersize;
		memset(pbDst, 0, outbuffersize);
		memset(&outFormat, 0, sizeof(VOCODECAUDIOFORMAT));
		t1 = GetTickCount();
//		t1 = clock();
		returnCode = voWMADecProcess(hCodec, &inData,
			&outData, &outFormat);
		t2 = GetTickCount();
//		t2 = clock();
		total += t2 - t1;

		if(returnCode == VORC_OK)
		{
//			write_wav_output(outData.buffer, outData.length);
//			fwrite(outData.buffer, outData.length, 1, pfDes);
			datalength += outData.length;
//			i++;
			inData.buffer = 0;
			inData.length = 0;
		}
		else if(returnCode == VORC_COM_INPUT_BUFFER_SMALL)
		{
			i++;
//			if(!(i%10))
#if 1
			if(!(i%10))
			{
				i-=5;
				returnCode = voWMADecSetParameter(hCodec, VOID_WMA_FLUSH, 0);
				if(returnCode < 0)
					break;
				if(fseek(pfSrc,5*WMAFormat.nBlockAlign, SEEK_CUR)!=0)break;
			}
#endif
//			read_size = fread(pbSrc, 1, WMAFormat.nBlockAlign *3, pfSrc);
			read_size = fread(pbSrc, 1, WMAFormat.nBlockAlign, pfSrc);
			if(!read_size) 
				break;
			inData.buffer = pbSrc;
			inData.length = read_size;
		}
		else
		{
				break;
		}

	}while(1);
	performance = 0.6*total*WMAFormat.nSamplesPerSec*4/datalength;//MHz = cpu_freq(MHz) * (total/1000) / ((datalength/4)/samplerate)
	FPs = (int)(i*1000.0/total);
//	wsprintf(msg, TEXT("Decode Time: %d clocks"), total);
	wsprintf(msg, TEXT("Performance: %f Mhz/s, %d frames, %d F/S"), performance, i, FPs);
	MessageBox(NULL, msg, TEXT("WMA Decode Finished"), MB_OK);
gCleanup:

	voWMADecUninit(hCodec);
//	if(pfDes) fclose(pfDes);
	close_wav_output();
	if(pfSrc) fclose(pfSrc);
    if(pbSrc) free(pbSrc);
    if(pbDst) free(pbDst);	
}
#endif

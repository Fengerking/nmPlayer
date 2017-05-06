//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2007 VisualOn SoftWare Co., Ltd.

Module Name:

    main_test_vop.cpp

Abstract:

    caller for WMA decoder.

Author:

    Witten Wen 12-July-2007

Revision History:

*************************************************************************/
#if		1
#ifndef LINUX
#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#include	<time.h>
#else
#include <windows.h>
#endif // _WIN32_WCE
#else
#ifdef RVDS
#include	<time.h>
#else
#include	<sys/time.h>
#endif
#include	"wmamalloc.h"
#endif	//LINUX

//#include	<stdlib.h>
#include	<stdio.h>
#include 	<string.h>
//#include	"wmamalloc.h"
//#include "wmaprodecS_api.h"
#include "cmnMemory.h"
#include "voWMA.h"
//#include "wavfileexio.h"
//#include "msaudiofmt.h"
#ifdef LINUX
#include <dlfcn.h>
#endif

#define OUTWAV 1
#define ErrorMessage()

FILE *faudio;
unsigned long int wav_size;
static int rate = 44100;//11025;//16000;//44100;

VO_AUDIO_FORMAT pcmformat;

#ifdef LINUX
//extern void *voWMAMemcpy (char *pDest, char *pSrc, unsigned int size);
#if RVDS
/////////////////////////////////////////////////
__asm void init_cpu() {
// Set up CPU state
	MRC p15,0,r4,c1,c0,0
	ORR r4,r4,#0x00400000 // enable unaligned mode (U=1)
	BIC r4,r4,#0x00000002 // disable alignment faults (A=0) // MMU not enabled: no page tables
	MCR p15,0,r4,c1,c0,0
#ifdef __BIG_ENDIAN
SETEND BE
#endif
	MRC p15,0,r4,c1,c0,2 // Enable VFP access in the CAR -
	ORR r4,r4,#0x00f00000 // must be done before any VFP instructions
	MCR p15,0,r4,c1,c0,2
	MOV r4,#0x40000000 // Set EN bit in FPEXC
	MSR FPEXC,r4

	IMPORT __main
	B __main
}
///////////////////////////////////////////////////////////////////////

VO_MEM_OPERATOR		g_memOP;

VO_U32 cmnMemAlloc (VO_S32 uID,  VO_MEM_INFO * pMemInfo)
{
	if (!pMemInfo)
		return VO_ERR_INVALID_ARG;

	pMemInfo->VBuffer = (VO_PTR)malloc (pMemInfo->Size);

	return 0;
}

VO_U32 cmnMemFree (VO_S32 uID, VO_PTR pMem)
{
	free (pMem);

	return 0;
}

VO_U32	cmnMemSet (VO_S32 uID, VO_PTR pBuff, VO_U8 uValue, VO_U32 uSize)
{
	memset (pBuff, uValue, uSize);

	return 0;
}

VO_U32	cmnMemCopy (VO_S32 uID, VO_PTR pDest, VO_PTR pSource, VO_U32 uSize)
{
	memcpy (pDest, pSource, uSize);

	return 0;
}

VO_U32	cmnMemCheck (VO_S32 uID, VO_PTR pBuffer, VO_U32 uSize)
{
	return 0;
}

VO_S32 cmnMemCompare (VO_S32 uID, VO_PTR pBuffer1, VO_PTR pBuffer2, VO_U32 uSize)
{
	return memcmp(pBuffer1, pBuffer2, uSize);
}

VO_U32	cmnMemMove (VO_S32 uID, VO_PTR pDest, VO_PTR pSource, VO_U32 uSize)
{
	memmove (pDest, pSource, uSize);

	return 0;
}

VO_S32 cmnMemFillPointer (VO_S32 uID)
{
	g_memOP.Alloc = cmnMemAlloc;
	g_memOP.Free = cmnMemFree;
	g_memOP.Set = cmnMemSet;
	g_memOP.Copy = cmnMemCopy;
	g_memOP.Check = cmnMemCheck;
	g_memOP.Compare = cmnMemCompare;
	g_memOP.Move = cmnMemMove;

	return 0;
}
#endif
#endif

#if 0
void *voWMAMemcpy (char *pDest, char *pSrc, unsigned int size)
{
	while(size--)
		*pDest++ = *pSrc++;
	return pDest;
}
#endif
#if 0
void *voWMAMalloc(size_t size)
{
	return malloc(size);
}

void *voWMACalloc(size_t size, size_t count)
{
	return calloc(size, count);
}

void voWMAFree(void *pDest)
{
	free(pDest);
}

void *voWMAMemmove(void *pDest, void *pSrc, unsigned int count)
{
	return memmove(pDest, pSrc, count);
}

void *voWMAMemset(void *pDest, int c, unsigned int size)
{
	return memset(pDest, c, size);
}

#endif
#if OUTWAV
__inline void shutdown_output ( void ) 
{
	printf("Shutting Down Sound System\n");
	if (faudio != NULL)
		fclose(faudio);
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

//#define TESTMEMORYLEAK
//#define MEMORYSIZE
#ifdef TESTMEMORYLEAK
extern int MallocNumber;
#endif

#ifdef MEMORYSIZE
extern size_t memorysize;
#endif

//#define TESTMEM1000
typedef int (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);
#ifdef LINUX
int main( int argc, char** argv )
#else
int main(int argc, char **argv)
#endif
{
// 	WAVEFORMATEXTENSIBLE wfx;
//    uint32 dwFrameSizeInSample;
    FILE *pfSrc = NULL;
	FILE *pfDes = NULL;
    VO_PBYTE pbSrc = NULL;
    int cbDst = 0;
    VO_PBYTE pbDst = NULL;
	int	outbuffersize;
	int		t1,t2;
	int total = 0;
	VO_U32 read_size = 0;
	VO_U32 g_iVersion = 0;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;
	VO_U32	returnCode;
	VO_HANDLE hCodec = 0;
	VO_WAVEFORMATEX	*WAVFormat;
	VO_AUDIO_CODECAPI pDecHandle;
	VO_MEM_OPERATOR wmaoper;
	VO_CODEC_INIT_USERDATA userData;

	void  *handle;
	void  *pfunc;
	VOGETAUDIODECAPI pGetAPI;

	char *temp, *filebufbegin;//, *temp1; 
	int offset = 0;
	unsigned char len[4];
#ifndef LINUX
	TCHAR msg[256];
#endif
	int i=0, datalength=0, FPs=0, k;
	double performance=0;

#ifdef _WIN32_
static char *wav_file = "output.wav";
#else
#ifndef LINUX
//static char *wav_file = "\\SDMMC\\wma\\output.wav";
//static char *wav_file = "\\MUSIC\\output.wav";
//static char *wav_file = "\\ResidentFlash\\wma2\\output.wav";
static char *wav_file = "\\wmp\\output.wav";
//static char *wav_file = "E:\\wma10_rvds22\\wma10-rvds22\\output.wav";
#else
static char *wav_file = "D:\\Numen\\trunk\\Codec\\Audio\\WMA\\DECODER\\prj\\wmatest\\output_rvds.wav";
#endif
#endif

#ifndef LINUX 
#ifdef _WIN32_
	char* szInputName     = "10325549_WM_WMA'Lossless_6158K'1m27s'6c'48KHz.wrw";
//	char* szInputName     = "she.hrd";
//	char* szInputName     = "pro_192kbps_44100_6ch_16bits.hrd";
//	char* szInputName     = "Input_pro_bxdd.hrd";
#else
    //TCHAR* szInputName     = TEXT("wmatest.cmp");
//    char* szInputName     = "\\Storage Card\\music\\test1_WMA_v8_1pCBR_64kbps_44kHz_2.hrd";
//    char* szInputName     = "\\Storage Card\\wma\\Classical_96_24_8_768000_0_1.hrd";
//    char* szInputName     = "\\wma\\pro_192kbps_44100_6ch_16bits.hrd";
//		char* szInputName     = "\\SDMMC\\wma\\input.vop";
    char* szInputName     = "\\wmp\\pro_192kbps_44100_6ch_16bits.hrd";
//    char* szInputName     = "input.vop";
#endif
#else
	char* szInputName     = "D:\\Numen\\trunk\\Codec\\Audio\\WMA\\DECODER\\prj\\wmatest\\Input_pro_bxdd.hrd";
	VO_U32 filesize = 0, pos_end = 0;
	char *fileBuffer = NULL; 
#endif

   if((temp = (char *)malloc(0x100)) == NULL)
	   printf("temp is not distributed buffer!\n");
/*   {
	   temp1 = voWMAMemcpy(temp+1, szInputName+1, 0x1d);//0x1d
	   temp1 = voWMAMemset(temp+1, 0x20, 2);
   }*/
   // cmp format
	if(argc <= 1 || argv[1] == NULL)
	{
		if((pfSrc = fopen(szInputName, "rb")) == NULL)goto gCleanup;
	}
	else
	{
		printf("open input file beginning!\n");
		if((pfSrc = fopen(argv[1], "rb")) == NULL)goto gCleanup;
		printf("open input file success!\n");
	}
   
    // read the header for initial parameters
//    if(fread(&(wfx.Format), sizeof(uint8), sizeof(WAVEFORMATEX), pfSrc) != sizeof(WAVEFORMATEX))
//#ifdef LINUX
#if 0
	fseek(pfSrc, 0, SEEK_END);
	filesize = ftell(pfSrc);
	fseek(pfSrc, 0, SEEK_SET);
	fileBuffer = (char *)malloc(filesize);
	if(fread(fileBuffer, 1, filesize, pfSrc) != filesize)
		goto gCleanup;
	pos_end = (VO_U32)fileBuffer + filesize;
#ifdef TESTMEM1000
	filebufbegin = fileBuffer;
	for(k=0; k<1000; k++)
	{
		fileBuffer = filebufbegin;
		printf("k: %d\n", k);
#endif	//TESTMEM1000
	voWMAMemcpy(&(inData.Length), fileBuffer, 4);
	
	inData.Buffer = temp;
	fileBuffer += 4;
	voWMAMemcpy(temp, fileBuffer, inData.Length);
	fileBuffer += inData.Length;
	WAVFormat = (VO_WAVEFORMATEX *)inData.Buffer;
#else	
	if(fread(len, 4, 1, pfSrc) != 1)
		goto gCleanup;

	inData.Length = len[0]; //(len[0] << 24) | (len[1] << 16) | (len[2] << 8) |len[3];
//	temp1 = voWMAMemcpy(temp, &inData.Length, 4);
	if(fread(temp, 1, inData.Length, pfSrc) != inData.Length)
		goto gCleanup;
	inData.Buffer = temp;
	WAVFormat = (VO_WAVEFORMATEX *)inData.Buffer;
#endif

	wmaoper.Alloc =		cmnMemAlloc;
	wmaoper.Free =		cmnMemFree;
	wmaoper.Copy =		cmnMemCopy;
	wmaoper.Set =		cmnMemSet;
	wmaoper.Compare =	cmnMemCompare;
	wmaoper.Check =		cmnMemCheck;
	wmaoper.Move =		cmnMemMove;

	userData.memflag = VO_IMF_USERMEMOPERATOR;
	userData.memData = &wmaoper;
	
#ifdef LINUX
	handle = dlopen("/data/local/tmp/wma/libvoWMADec.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}

	pfunc = dlsym(handle, "voGetWMADecAPI");	
	if(pfunc == 0)
	{
		printf("open function error......");
		return -1;
	}

	pGetAPI = (VOGETAUDIODECAPI)pfunc;

	returnCode  = pGetAPI(&pDecHandle);
	
	if(returnCode)
		return -1;
#else
	returnCode = voGetWMADecAPI(&pDecHandle);
	if(returnCode != VO_ERR_NONE)
		goto gCleanup;
#endif
	
	returnCode = pDecHandle.Init(&hCodec, VO_AUDIO_CodingWMA, &userData);	
	if(returnCode < 0)
		goto gCleanup;
	
//	returnCode = pDecHandle.SetParam(hCodec, VO_PID_WMA_SUPPTHISMPLRT, 0);
//	if(returnCode < 0)
//		goto gCleanup;
//	returnCode = pDecHandle.SetParam(hCodec, VO_PID_WMA_SUPPTMTCHANL, 0);
//	if(returnCode < 0)
//		goto gCleanup;
//	returnCode = pDecHandle.SetParam(hCodec, VO_PID_WMA_SUPPORT24BIT, 0);
//	if(returnCode < 0)
//		goto gCleanup;
	
	returnCode = pDecHandle.SetParam(hCodec, VO_PID_COMMON_HEADDATA, &inData);
	if(returnCode < 0)
		goto gCleanup;
	
	//returnCode = pDecHandle.SetParam(hCodec, VO_PID_COMMON_HEADDATA, &inData);
	//if(returnCode < 0)
	//	goto gCleanup;
	
	returnCode = pDecHandle.GetParam(hCodec, VO_PID_WMA_OUTBUFFERSIZE, &outbuffersize);
	if(returnCode < 0)
		goto gCleanup;
	
	pbDst = (VO_U8 *)malloc(outbuffersize);
	outData.Length = outbuffersize;
//	pbSrc = (VO_U8 *)malloc(WAVFormat->nBlockAlign*5 + 100);
	pbSrc = (VO_U8 *)malloc(WAVFormat->nBlockAlign + 100);
	returnCode = pDecHandle.GetParam(hCodec, VO_PID_AUDIO_FORMAT, &pcmformat);
	if(returnCode < 0)
		goto gCleanup;
#if	OUTWAV
	if(argc <= 1 || argv[2] == NULL)
	{
		if (open_wav_output(wav_file) == -1)goto gCleanup;
	}
	else
		if (open_wav_output(argv[2]) == -1)goto gCleanup;
#endif

//#ifdef LINUX
#if 0
	read_size = WAVFormat->nBlockAlign;
	memcpy(pbSrc, fileBuffer, read_size);
	printf("first 4 bytes: 0x%x, 0x%x, 0x%x, 0x%x\n", pbSrc[0], pbSrc[1], pbSrc[2], pbSrc[3]);
	fileBuffer += read_size;
	printf("read_size: 0x%x\n", read_size);
#else
	//read_size = fread(len, 1, 4, pfSrc);
	//inData.Length = (len[0] << 24) | (len[1] << 16) | (len[2] << 8) |len[3];
	read_size = fread(pbSrc, 1, WAVFormat->nBlockAlign, pfSrc);
	if(!read_size)
	{
		goto gCleanup;
	}
#endif
	inData.Buffer = pbSrc;
	inData.Length = read_size;

	do{
		returnCode = pDecHandle.SetInputData(hCodec, &inData);
		if(returnCode != VO_ERR_NONE)
			break;
		do
		{
			outData.Buffer = pbDst;
			outData.Length = outbuffersize;
			memset(pbDst, 0, outbuffersize);
#ifndef LINUX
			t1 = GetTickCount();
//			t1 = clock();
#endif			
			returnCode = pDecHandle.GetOutputData(hCodec, &outData, &outFormat);
#ifndef LINUX
			t2 = GetTickCount();
//			t2 = clock();
			total += t2 - t1;
//			printf("total: %dms,\n", total);
#endif
			if(returnCode == VO_ERR_NONE)
			{
#if OUTWAV
				write_wav_output(outData.Buffer, outData.Length);
#endif
				datalength += outData.Length;
//				if(datalength == 0xc32000)
//					datalength = 0xc32000;
//				i++;
//				inData.buffer = 0;
//				inData.length = 0;
			}
			else if(returnCode == VO_ERR_INPUT_BUFFER_SMALL)
			{
				i++;
				if(i == 0xb50)
					i = 0xb50;
				break;
			}
		}while(1);

//#ifdef LINUX
#if 0
		if((VO_U32)fileBuffer + WAVFormat->nBlockAlign <= pos_end)
		{
			read_size = WAVFormat->nBlockAlign;
			memcpy(pbSrc, fileBuffer, read_size);
			fileBuffer += read_size;
		}
		else
		{
			read_size = pos_end - (VO_U32)fileBuffer;
			if(read_size <= 0)
				break; 
			memcpy(pbSrc, fileBuffer, read_size);
			fileBuffer += read_size;
		}
			
#else
//		read_size = fread(pbSrc, 1, WAVFormat->nBlockAlign*5, pfSrc);
		//read_size = fread(len, 1, 4, pfSrc);
		//inData.Length = (len[0] << 24) | (len[1] << 16) | (len[2] << 8) |len[3];
		read_size = fread(pbSrc, 1, WAVFormat->nBlockAlign, pfSrc);
		if(!read_size) 
			break;
#endif
		inData.Buffer = pbSrc;
		inData.Length = read_size;
	}while(1);
#if 1
#ifndef LINUX
	performance = 0.2*total*WAVFormat->nSamplesPerSec*4/datalength;//MHz = cpu_freq(MHz) * (total/1000) / ((datalength/4)/samplerate)
	FPs = (int)(i*1000.0/total);
//	wsprintf(msg, TEXT("Decode Time: %d clocks"), total);
	//wsprintf(msg, TEXT("Performance: %f Mhz/s, %d frames, %d F/S"), performance, i, FPs);
	//MessageBox(NULL, msg, TEXT("WMA Decode Finished"), MB_OK);
#else
	printf("decoding is finished!\n");
#endif
#endif
gCleanup:

	pDecHandle.Uninit(hCodec);
//	printf("Uninit is finished!\n");
#ifdef TESTMEMORYLEAK
	printf("the number of memory leak is: %d,\n", MallocNumber);
#endif

#ifdef MEMORYSIZE
	printf("the size of RAM is: %d,\n", memorysize);
#endif

//#ifdef LINUX
#if 0
#ifdef TESTMEM1000
	if(pbSrc) free(pbSrc);
    if(pbDst) free(pbDst);	
	}
#endif
	fileBuffer -= filesize;
//	printf("fileBuffer address: ox%x,\n", (int)fileBuffer);
    if(fileBuffer) 
	{
		free(fileBuffer);
		printf("Free filebuffer is finished!\n");
	}
//	else
//		printf("filebuffer is not free!\n");
#endif

//	if(pfDes) fclose(pfDes);
#if OUTWAV
	close_wav_output();
#endif	//OUTWAV
	if(pfSrc) fclose(pfSrc);
    
	if(pbSrc) free(pbSrc);
    if(pbDst) free(pbDst);
    if(temp) free(temp);
    return 0;
}
#endif

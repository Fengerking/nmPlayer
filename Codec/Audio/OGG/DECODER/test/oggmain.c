//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    oggmain.c

Abstract:

    caller for OGG decoder.

Author:

    Witten Wen 15-January-2010

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
//#include <windows.h>
#endif // _WIN32_WCE
#else
#include	<sys/time.h>
#include	"vorbismemory.h"
#endif	//LINUX

//#include	<stdlib.h>
#include	<stdio.h>
#include 	<string.h>
#include        "cmnMemory.h"
#include        "voOGG.h"
#include        "vovorbisdec.h"

#define INPUT_LEN	8192

//#define TESTSEEK
#ifdef	TESTSEEK
int i = 0;
#endif	//TESTSEEK
#define OUTWAV 1
#define ErrorMessage()

FILE *faudio;
unsigned long int wav_size;
static int rate = 44100;//11025;//16000;//44100;

VO_AUDIO_FORMAT pcmformat;
//extern HINSTANCE g_hVidDecInst;
//extern void *g_hVidDecInst;

#ifdef LINUX
#if 0
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
#endif
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

	shutdown_output();
}
#endif //OUTWAV

#define PACKETHEADERSIZE	27
//return OGG audio head length, 0 for failure.
VO_U32 voGetOGGFormat2Header(FILE *phandle, VO_CODECBUFFER *pOutBuffer)
{
//	struct VORBISFORMAT2 *pTemp = malloc(sizeof());
//	pOutBuffer->Buffer = 
//	fread();
	VO_U32 i, j, bodylength, HeadLength = 0;
	VO_U8 bytes, temp[64];
	VO_BOOL	headendflag = VO_FALSE;
	int pos = ftell(phandle);//record the original position.
	
	//Get the first 2 block
	for(i=0; i<2; i++)
	{
		bodylength = 0;
		if(fseek(phandle, HeadLength+PACKETHEADERSIZE-1, SEEK_SET))
			return 0;
		if(fread(&bytes, 1, 1, phandle) != 1)
			return 0;
		if(bytes != fread(&temp, 1, bytes, phandle))
			return 0;
		for (j=0; j<bytes; j++)
			bodylength += temp[j];
		HeadLength += bodylength + (PACKETHEADERSIZE + bytes);
	}
	//if span is exit, get the 3rd block.
	while(temp[bytes-1] == 255 && !headendflag)
	{
		bodylength = 0;
		if(fseek(phandle, HeadLength+PACKETHEADERSIZE-1, SEEK_SET))
			return 0;
		if(fread(&bytes, 1, 1, phandle) != 1)
			return 0;
		if(bytes != fread(&temp, 1, bytes, phandle))
			return 0;
		for (j=0; j<bytes; j++)
		{
			if(temp[j] < 255)
				headendflag = VO_TRUE;
			bodylength += temp[j];
		}
		HeadLength += bodylength + (PACKETHEADERSIZE + bytes);
	}

	//back to original position.
	if(fseek(phandle, pos, SEEK_SET))
		return 0;

	return HeadLength;
}

VO_U32 voGetPageoffset(FILE *phandle)
{
	int offset = 0, readsize = 0;
	int pos = ftell(phandle);//record the original position.
	VO_U8 *temp = (VO_U8 *)malloc(INPUT_LEN);

	readsize = fread(temp, 1, INPUT_LEN, phandle);
	while(offset++ < INPUT_LEN)
	{
		if(*temp++ == 'O')
			if(temp[0] == 'g' && temp[1] == 'g' && temp[2] == 'S')
				break;
	}
	fseek(phandle, pos, SEEK_SET);
	free(temp);
	return (offset < INPUT_LEN)?offset-1:0;
}

typedef struct SegInfo{
	int SegNo;
	int SegRead;
	int CurNotEnd;
	unsigned char SegSeq[255];
}SegInfo;

typedef struct Packet{
	int Len;
	int PacketEnd;
	char *Buffer;
	char *readpos;
}Packet;
int voPageParser(FILE *fSrc, SegInfo *seginfo)
{
	char temp[4];
	if(4!=fread(&temp, 1, 4, fSrc))
		return -1;
	if(memcmp(&temp, "OggS", 4))
	{
		printf("This is not a page!");
		return -1;
	}
	fseek(fSrc, 22, SEEK_CUR);
	memset(seginfo, 0, sizeof(SegInfo));
	fread(&seginfo->SegNo, 1, 1, fSrc);
	if(seginfo->SegNo != fread(&seginfo->SegSeq, 1, seginfo->SegNo, fSrc))
		return -2;
	return 0;
}

int voGetpacketLen(SegInfo *seginfo, Packet *packet)
{
	while(seginfo->SegRead<seginfo->SegNo)
	{
		if(seginfo->SegSeq[seginfo->SegRead] < 255)
		{
			packet->Len += seginfo->SegSeq[seginfo->SegRead];
			seginfo->CurNotEnd = 0;
			packet->PacketEnd = 1;
			break;
		}
		packet->Len += seginfo->SegSeq[seginfo->SegRead];
		seginfo->CurNotEnd = 1;
	}
	return packet->Len;
}
int voGetPacket(FILE *fSrc, SegInfo *seginfo, Packet *packet)
{
	int i;
	while(seginfo->SegRead<seginfo->SegNo)
	{
		if(seginfo->SegSeq[seginfo->SegRead] < 255&&seginfo->SegSeq[seginfo->SegRead]!=0)
		{
			fread(packet->readpos, 1, seginfo->SegSeq[seginfo->SegRead], fSrc); 
			packet->readpos += seginfo->SegSeq[seginfo->SegRead];
			packet->Len += seginfo->SegSeq[seginfo->SegRead];
			seginfo->CurNotEnd = 0;
			packet->PacketEnd = 1;
			seginfo->SegRead++;
			return 0;
		}
		fread(packet->readpos, 1, seginfo->SegSeq[seginfo->SegRead], fSrc); 
		packet->readpos += seginfo->SegSeq[seginfo->SegRead];
		packet->Len += seginfo->SegSeq[seginfo->SegRead];
		seginfo->SegRead++;
		seginfo->CurNotEnd = 1;
		packet->PacketEnd = 0;
	}
	seginfo->CurNotEnd = 1;
	return 1;
}
//#define MEMORYSIZE
#ifdef MEMORYSIZE
extern VO_U32 memorysize;
#endif

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#elif _IOS 
beginTest_ogg(char *inFile, char *outFile, int times)
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
	VO_HANDLE hCodec = NULL;
	VO_WAVEFORMATEX	*WAVFormat;
	VO_AUDIO_CODECAPI pDecHandle;
	VO_MEM_OPERATOR oggoper;
	VO_CODEC_INIT_USERDATA userData;
	char *temp;//, *temp1; 
	int offset = 0;

	/* new AP added */
	SegInfo seginfo;
	Packet	packet;
	struct VORBISFORMAT2 *format2= NULL;
#ifndef LINUX
	//TCHAR msg[256];
#endif
	int i=0, m_DataLength=0, FPs=0;
	double performance=0;

	int filesize = 0;

#ifdef _WIN32_
static char *wav_file = "output.wav";
#else
#ifndef LINUX
//static char *wav_file = "\\SDMMC\\wma\\output.wav";
//static char *wav_file = "\\MUSIC\\output.wav";
//static char *wav_file = "\\ResidentFlash\\ogg\\output.wav";
static char *wav_file = "\\My Storage\\ogg\\output.wav";
//static char *wav_file = "E:\\wma10_rvds22\\wma10-rvds22\\output.wav";
#else
static char *wav_file = "e:\\wma10pro\\output.wav";
#endif
#endif

#ifndef LINUX 
#ifdef _WIN32_
//	char* szInputName     = "D:\\Numen\\trunk\\Codec\\Audio\\OGG\\DECODER\\prj\\OggTest\\44k-2ch-128kbps.ogg";
//	char* szInputName     = "D:\\Numen\\trunk\\Codec\\Audio\\OGG\\DECODER1\\prjnew\\win32ogg\\oggds.vot";
//	char* szInputName     = "D:\\Numen\\trunk\\Codec\\Audio\\OGG\\DECODER\\music\\44k-2ch-16b.ogg";;//floor1
	char* szInputName     = "14574164_OGG_Vorbis_40K'18s721ms'2c'44KHz.ogg";//floor1
//	char* szInputName	= "Y:\\Mobileappliance\\100111\\sample_00.ogg";
#else
    //TCHAR* szInputName     = TEXT("wmatest.cmp");
    char* szInputName     = "\\My Storage\\OGG\\xinxingquan.ogg";
//    char* szInputName     = "D:\\Numen\\trunk\\Codec\\Audio\\OGG\\DECODER\\music\\input.ogg";
//		char* szInputName     = "\\SDMMC\\wma\\input.vop";
//    char* szInputName     = "\\wmp\\44k-2ch-16b.ogg";
//    char* szInputName     = "input.vop";
#endif
#else
	char* szInputName     = "e:\\wma10pro\\input.hrd";
	VO_U32 filesize = 0, pos_end = 0;
	char *fileBuffer = NULL; 
#endif
 //  g_hVidDecInst = NULL;
   if((temp = (char *)malloc(0x100)) == NULL)
	   printf("temp is not distributed buffer!\n");
/*   {
	   temp1 = voWMAMemcpy(temp+1, szInputName+1, 0x1d);//0x1d
	   temp1 = voWMAMemset(temp+1, 0x20, 2);
   }*/
   // cmp format
#ifdef _IOS
        pfSrc = fopen(inFile, "rb");
#else
	if(argc == 1 || argv[1] == NULL)
	{
		if((pfSrc = fopen(szInputName, "rb")) == NULL)goto gCleanup;
	}
	else
	{
		printf("open input file beginning!\n");
		if((pfSrc = fopen(argv[1], "rb")) == NULL)goto gCleanup;
		printf("open input file success!\n");
	}
#endif
	oggoper.Alloc =		cmnMemAlloc;
	oggoper.Free =		cmnMemFree;
	oggoper.Copy =		cmnMemCopy;
	oggoper.Set =		cmnMemSet;
	oggoper.Compare =	cmnMemCompare;
	oggoper.Check =		cmnMemCheck;
	oggoper.Move =		cmnMemMove;

	userData.memflag = VO_IMF_USERMEMOPERATOR;
	userData.memData = &oggoper;
	returnCode = voGetOGGDecAPI(&pDecHandle);
	if(returnCode != VO_ERR_NONE)
		goto gCleanup;

	returnCode = pDecHandle.Init(&hCodec, VO_AUDIO_CodingOGG, &userData);
	if(returnCode < 0)
		goto gCleanup;
#if 1
//	if(!(inData.Length = voGetOGGHeader(pfSrc)))
//		goto gCleanup;
	{		
		int headpack=0;
		fseek(pfSrc, 0, SEEK_END);
		filesize = ftell(pfSrc);
		fseek(pfSrc, 0, SEEK_SET);		
		format2 = (struct VORBISFORMAT2  *)malloc(sizeof(struct VORBISFORMAT2));
//		if(fread(pbSrc, 1, filesize, pfSrc) != filesize)
//			goto gCleanup;
		memset(format2, 0, sizeof(struct VORBISFORMAT2));
//		memset(&seginfo, 0, sizeof(SegInfo));
		memset(&packet, 0, sizeof(Packet));
		packet.Buffer = (VO_PBYTE)malloc(filesize); //if malloc size is less than filesize, program may crash when decoding some clips, such as Lock.ogg
		packet.readpos = packet.Buffer + sizeof(struct VORBISFORMAT2);
		while(headpack<3)
		{
			voPageParser(pfSrc, &seginfo);
			
			while(1)
			{
				voGetPacket(pfSrc, &seginfo, &packet);
				if(packet.PacketEnd == 0)
				{
					break;
				}
				format2->HeadPacketSize[headpack] = packet.Len;
				packet.PacketEnd = 0;
				packet.Len = 0;
				headpack++;
				if(headpack >= 3)
					break;
			}
		}
	}
	inData.Buffer = packet.Buffer;
	if(!inData.Buffer)
		goto gCleanup;
	inData.Length = format2->HeadPacketSize[0] + format2->HeadPacketSize[1] + 
		format2->HeadPacketSize[2] + sizeof(struct VORBISFORMAT2);
	memcpy(inData.Buffer, format2, sizeof(struct VORBISFORMAT2));
//	inData.Length = fread(inData.Buffer, 1, inData.Length, pfSrc); 
	if(inData.Length <= 0)
		goto gCleanup;
	//	inData.Length = read_size;361,403
	inData.Time = 0;

	returnCode = pDecHandle.SetParam(hCodec, VO_PID_COMMON_HEADDATA, &inData);
	if(returnCode != VO_ERR_NONE)
	{
		goto gCleanup;
	}
//	free(inData.Buffer);
//	inData.Buffer = NULL;

	returnCode = pDecHandle.GetParam(hCodec, VO_PID_OGG_OUTBUFFERSIZE, &outbuffersize);
	if(returnCode < 0)
		goto gCleanup;

	pbDst = (VO_U8 *)malloc(outbuffersize);
	outData.Length = outbuffersize;
//	pbSrc = (VO_U8 *)malloc(WAVFormat->nBlockAlign*5 + 100);
//	pbSrc = (VO_U8 *)malloc(INPUT_LEN);
	returnCode = pDecHandle.GetParam(hCodec, VO_PID_AUDIO_FORMAT, &pcmformat);
	if(returnCode < 0)
		goto gCleanup;
#if OUTWAV
#ifdef _IOS
	if(open_wav_output(outFile) == -1)
		goto gCleanup;
#else
	if(argc == 1 || argv[2] == NULL)
	{
		if (open_wav_output(wav_file) == -1)goto gCleanup;
	}
	else
		if (open_wav_output(argv[2]) == -1)goto gCleanup;
#endif
#endif

#ifdef LINUX
	read_size = WAVFormat->nBlockAlign;
	memcpy(pbSrc, fileBuffer, read_size);
	printf("first 4 bytes: 0x%x, 0x%x, 0x%x, 0x%x\n", pbSrc[0], pbSrc[1], pbSrc[2], pbSrc[3]);
	fileBuffer += read_size;
	printf("read_size: 0x%x\n", read_size);
#else
	//	read_size = fread(pbSrc, 1, WAVFormat->nBlockAlign*5, pfSrc);
//	read_size = fread(pbSrc, 1, INPUT_LEN, pfSrc);
//	if(!read_size)
//	{
//		goto gCleanup;
//	}
#endif
	memset(inData.Buffer, 0, inData.Length);
	fseek(pfSrc, 0 ,SEEK_SET);
	memset(&seginfo, 0, sizeof(SegInfo));
	seginfo.CurNotEnd = 1;
//	memset(&packet, 0, sizeof(Packet));
	packet.readpos = packet.Buffer;
	packet.PacketEnd = 0;
	packet.Len = 0;
	while(packet.PacketEnd == 0)
	{
		if(seginfo.CurNotEnd == 1)
			voPageParser(pfSrc, &seginfo);
		voGetPacket(pfSrc, &seginfo, &packet);
		if(packet.PacketEnd == 1)
		{
			packet.readpos = packet.Buffer;
			inData.Length = packet.Len;
			packet.PacketEnd = 0;
			packet.Len = 0;
			break;
		}else{
			seginfo.CurNotEnd = 1;
		}
	}

//	inData.Buffer = pbSrc + inData.Length;
//	inData.Length = filesize - inData.Length;

	do{
//		printf("SetInputData beginning!\n");
		returnCode = pDecHandle.SetInputData(hCodec, &inData);
		if(returnCode != VO_ERR_NONE)
			break;
		do
		{
			outData.Buffer = pbDst;
			outData.Length = outbuffersize;
			memset(pbDst, 0, outbuffersize);
//#ifndef LINUX
//			//t1 = GetTickCount();
//			t1 = clock();
//#endif
			returnCode = pDecHandle.GetOutputData(hCodec, &outData, &outFormat);
//#ifndef LINUX
//			//t2 = GetTickCount();
//			t2 = clock();
//			total += t2 - t1;
//#endif
			if(returnCode == VO_ERR_NONE)
			{
#if OUTWAV
				write_wav_output(outData.Buffer, outData.Length);
#endif
				m_DataLength += outData.Length;
//				if(m_DataLength == 0xc32000)
//					m_DataLength = 0xc32000;
//				i++;
//				inData.buffer = 0;
//				inData.length = 0;
			}
			else if(returnCode == VO_ERR_INPUT_BUFFER_SMALL)
			{
				i++;
//				if(i == 0x2a)
//					i=0x2a;
#ifdef TESTSEEK
					if(i==0x10)
					{
						int pageoffset;
						fseek(pfSrc, INPUT_LEN * 0x5, SEEK_CUR);
						pageoffset = voGetPageoffset(pfSrc);
						fseek(pfSrc, pageoffset, SEEK_CUR);
						pDecHandle.SetParam(hCodec, VO_PID_COMMON_FLUSH, NULL);
					}
#endif	//TESTSEEK				
				break;
			}
		}while(1);

#ifdef LINUX
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
		inData.Buffer = pbSrc;
		inData.Length = read_size;	
#else
//		read_size = fread(pbSrc, 1, WAVFormat->nBlockAlign*5, pfSrc);
		memset(inData.Buffer, 0, inData.Length);
		while(packet.PacketEnd == 0)
		{
			if(seginfo.CurNotEnd == 1)
				if(voPageParser(pfSrc, &seginfo)==-1)
					goto gEnd;
			voGetPacket(pfSrc, &seginfo, &packet);
			if(packet.PacketEnd == 1)
			{
				packet.readpos = packet.Buffer;
				inData.Length = packet.Len;
				packet.PacketEnd = 0;
				packet.Len = 0;
				break;
			}else{
				seginfo.CurNotEnd = 1;
			}
		}
//		read_size = fread(pbSrc, 1, INPUT_LEN, pfSrc);
//		if(!read_size) 
//			break;
#endif
		
	}while(1);
#if 1
#ifndef LINUX
gEnd:
	performance = 1.0*total*pcmformat.SampleRate*2*pcmformat.Channels/m_DataLength;//MHz = cpu_freq(MHz) * (total/1000) / ((m_DataLength/4)/samplerate)
	FPs = (int)(i*1000.0/total);
//	wsprintf(msg, TEXT("Decode Time: %d clocks"), total);
	//wsprintf(msg, TEXT("Performance: %f Mhz/s, %d frames, %d F/S"), performance, i, FPs);
	//MessageBox(NULL, msg, TEXT("WMA Decode Finished"), MB_OK);
#else
	printf("decoding is finished!\n");
#endif
#endif
#endif
gCleanup:

	if(hCodec)
		pDecHandle.Uninit(hCodec);
//	if(pfDes) fclose(pfDes);
#if OUTWAV
	close_wav_output();
#endif	//OUTWAV
#ifdef MEMORYSIZE
	printf("the size of RAM is: %d,\n", memorysize);
#endif
	if(pfSrc) fclose(pfSrc);
    if(pbSrc) free(pbSrc);
    if(pbDst) free(pbDst);	
#ifdef LINUX
    if(fileBuffer) free(fileBuffer);
#endif
    if(temp) free(temp);	
    return 0;
}
#endif

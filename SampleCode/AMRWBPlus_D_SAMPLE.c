/************************************************************************
*												*
*		VisualOn, Inc. Confidential and Proprietary, 2006				*			
*												*		
************************************************************************/
#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE
#include <windows.h>
#include		<stdio.h>
#include		<stdlib.h>
#include		<time.h>
#include		<limits.h>
#include        "voAMRWBP.h"
#include        "cmnMemory.h"

#define OUTPUT	1
//Assume the input file is IF1

#define  INPUT_SIZE   40960*2
unsigned char  InputBuf[INPUT_SIZE];
unsigned char  OutputBuf[40960];
int    usedSize=0;
int    decodedSize=0;
int  GetNextBuf(FILE* inFile,unsigned char* dst,int size)
{
	int size2 = fread(dst, sizeof(signed char), size,inFile);
	return size2;
}
static int write_wav_header(FILE* fp, int channelNum,int sampleRate)//AACDecLibParam* pFrameInfo)
{
	unsigned char header[44];
	unsigned char* p = header;
	unsigned int bytes = 2;
	float data_size = (float)bytes * 0;//pFrameInfo->outputSize;//pFrameInfo->outputSamps;//aufile->total_samples;
	unsigned long word32;
	//pFrameInfo->channelNum = 1;
	//pFrameInfo->sampleRate*=2;//pFrameInfo->channelNum;
	*p++ = 'R'; *p++ = 'I'; *p++ = 'F'; *p++ = 'F';

	word32 = 0x010cd024;//word32 = (data_size + (44 - 8) < (float)MAXWAVESIZE) ?(unsigned long)data_size + (44 - 8)  :  (unsigned long)MAXWAVESIZE;

	*p++ = (unsigned char)(word32 >>  0);
	*p++ = (unsigned char)(word32 >>  8);
	*p++ = (unsigned char)(word32 >> 16);
	*p++ = (unsigned char)(word32 >> 24);

	*p++ = 'W'; *p++ = 'A'; *p++ = 'V'; *p++ = 'E';

	*p++ = 'f'; *p++ = 'm'; *p++ = 't'; *p++ = ' ';

	*p++ = 0x10; *p++ = 0x00; *p++ = 0x00; *p++ = 0x00;

	if (0)//aufile->outputFormat == FAAD_FMT_FLOAT)
	{
		*p++ = 0x03; *p++ = 0x00;
	} else {
		*p++ = 0x01; *p++ = 0x00;
	}

	*p++ = (unsigned char)(channelNum >> 0);
	*p++ = (unsigned char)(channelNum >> 8);

	word32 = (unsigned long)(sampleRate);
	*p++ = (unsigned char)(word32 >>  0);
	*p++ = (unsigned char)(word32 >>  8);
	*p++ = (unsigned char)(word32 >> 16);
	*p++ = (unsigned char)(word32 >> 24);

	word32 = sampleRate * bytes * channelNum;
	*p++ = (unsigned char)(word32 >>  0);
	*p++ = (unsigned char)(word32 >>  8);
	*p++ = (unsigned char)(word32 >> 16);
	*p++ = (unsigned char)(word32 >> 24);

	word32 = bytes * channelNum;
	*p++ = (unsigned char)(word32 >>  0);
	*p++ = (unsigned char)(word32 >>  8);

	*p++ = (unsigned char)(16 >> 0);
	*p++ = (unsigned char)(16 >> 8);

	*p++ = 'd'; *p++ = 'a'; *p++ = 't'; *p++ = 'a';
	word32 = 0x010cd000;//data_size < MAXWAVESIZE ?(unsigned long)data_size : (unsigned long)MAXWAVESIZE;


	*p++ = (unsigned char)(word32 >>  0);
	*p++ = (unsigned char)(word32 >>  8);
	*p++ = (unsigned char)(word32 >> 16);
	*p++ = (unsigned char)(word32 >> 24);

	return fwrite(header, sizeof(header), 1, fp);
}
//static short synth[4096];             /* Synthesis                     */
#ifdef _WIN32_WCE
//int WinMain()
int _tmain(int argc, TCHAR **argv) 
{
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	FILE  *fsrc = NULL;
	FILE  *fdst = NULL;
	char  *infileName;
	char  *outfileName;
	DWORD t1, t2;
	int   ret = 0;
	char  *inBuf = InputBuf;
	char  *outBuf = OutputBuf;

	VO_AUDIO_CODECAPI  AudioAPI;
	VO_MEM_OPERATOR    moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE  hCodec;
	VO_CODECBUFFER  inData;
	VO_CODECBUFFER  outData;
	VO_AUDIO_OUTPUTINFO  outFormat;

	int     Frame_Lens = 0;
	int     frameCount = 0;
	int     returnCode;
	int     eofFile = 0;
	int     Relens;
	int     first = 1;
	double	duration = 0.0;

#ifdef _WIN32_WCE
	DWORD  total = 0;
	TCHAR  msg[256];
	infileName = "/Storage Card/test/AMRWBP/1.awb";
	outfileName = "/Storage Card/test/AMRWBP/output/out.pcm";
#else//_WIN32_WCE
	if (argc != 3)
	{
		fprintf (stderr, " Usage:\n\n   %s  src.amr dst.pcm\n\n", argv[0]);
		exit (1);
	}
	infileName = argv[1];
	outfileName = argv[2];
#endif//_WIN32_WCE
	if ((fsrc = fopen (infileName, "rb")) == NULL)
	{
		ret = 1;
		goto safe_exit;
	}

	if ((fdst = fopen (outfileName, "wb")) == NULL)
	{
		ret = 2;
		goto safe_exit;
	}

	//2 Byte Header info 
	Relens  = GetNextBuf(fsrc,InputBuf,2);
	if(Relens < 2)
	{
		eofFile = 1;
	}

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);

	returnCode = voGetAMRWBPDecAPI(&AudioAPI);

	if(returnCode)
	{
		ret = -1;
		goto safe_exit;
	}

	//#######################################   Init Decoding Section   #########################################
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingAMRWBP, &useData);

	if(returnCode)
	{
		ret = 3;
		goto safe_exit;
	}

	inData.Buffer = (unsigned char *)inBuf;
	inData.Length = Relens;

	//Get the frame length
	returnCode = AudioAPI.SetParam(hCodec, VO_PID_AMRWBP_HEADER_METE, &inData);
	if (returnCode)
	{
		ret = 3;
		goto safe_exit;
	}

	returnCode = AudioAPI.GetParam(hCodec, VO_PID_AMRWBP_FRAME_LENS, &Frame_Lens);
	if (returnCode)
	{
		ret = 3;
		goto safe_exit;
	}
	fseek(fsrc, -2, SEEK_CUR);

	do {
		Relens  = GetNextBuf(fsrc,InputBuf,Frame_Lens);
		if (Relens != Frame_Lens)
		{
			break;
		}

		inData.Buffer    = (unsigned char *)inBuf;
		inData.Length    = Relens;
		outData.Buffer   = (unsigned char *)outBuf;
		outData.Length   = 40960;
		/* decode one amrwb+ block */

#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		t1 = clock();
#endif
		returnCode = AudioAPI.SetInputData(hCodec,&inData);
		returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outFormat);


#ifdef _WIN32_WCE
		t2 = GetTickCount();
		total += t2 - t1;
#else
		t2 = clock();
		duration += t2 - t1;
#endif
		if(returnCode == 0)
		{
			frameCount++;
			fwrite(outData.Buffer, 1, outData.Length, fdst);
			fflush(fdst);
		}

	} while (Relens);

safe_exit:
	returnCode = AudioAPI.Uninit(hCodec);
#ifdef _WIN32_WCE
	wsprintf(msg, TEXT("Decode Time: %d clocks, Decode Frames: %d, fps: %d"), total, frameCount, frameCount*1000/total);
	MessageBox(NULL, msg, TEXT("AMR_WB Decoder Finished"), MB_OK);
#else
	printf( "\n%2.5f seconds\n", (double)duration);
#endif
	if (fsrc)
		fclose(fsrc);
	if (fdst)
		fclose(fdst);
	return ret;
}

/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		voEVRC_SAMPLES.c

Contains:	EVRC Sample code

Written by:	Huaping Liu

Change History (most recent first):
2009-06-09		LHP			Create file
*******************************************************************************/

#ifdef _WIN32_WCE
#include    <windows.h>
#include    <objbase.h>
#include    <Winbase.h>
#endif // _WIN32_WCE

#ifdef LINUX
#include <dlfcn.h>
#endif

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    "voEVRC.h"
#include    "cmnMemory.h"

#ifdef RVDS
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
#endif

#define   CODEC_MODE          1               //1 encoder, and 0 decoder
#define   L_FRAME             320             //output buffer length, 320 Byte for Decoder, vary Bytes(Max 171bits/frame 23Bytes) for Encoder
#define   INPUT_SIZE          40960
#define   VO_NXP_CODEC_KEY    0x4000FFFF         /* License key ID */

unsigned char  InputBuf[INPUT_SIZE ];

int  GetNextBuf(FILE* inFile,unsigned char* dst,int size)
{
	int size2 = fread(dst, sizeof(signed char), size,inFile);
	return size2;
}

typedef int (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#elif _IOS
int beginTest_evrc(char *inFile, char *outFile, int times)
#else
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	int   decode_flag;
	int   encode_flag;
	FILE  *fsrc = NULL;
	FILE  *fdst = NULL;
	char  *infileName;
	char  *outfileName;
	long  t1, t2;
	int   ret = 0;
	int   EVRC_Mode = 0;               /*EVRC_Mode = 0 for Decoder, 1 for encoder */
	char  *inBuf = InputBuf;
	int   KeyValue = 1;                         /* KeyValude = 1 means disable time limitation */

	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;

	short  synth[L_FRAME];             /* Synthesis*/
	int    frameCount = 0;
	int    returnCode;
	int    eofFile = 0;
	int    Relens;
	double duration = 0.0;

	void  *handle;
	void  *pfunc;
	VOGETAUDIODECAPI pGetAPI;

	//if(argc < 4)
	//{
	//	printf ("EVRC Reference Codec HELP   Displays this text\n");
	//	printf ("\n");
	//	printf ("Usage:\n");
	//	printf ("voQCELPDec Input_file output_file Mode \n");
	//	printf ("mode:  1 encoder, and 0 decoder");
	//	return -1;
	//}

	//if(argv[3][0] == '1')
	//       EVRC_Mode = 1;
	//else if(argv[3][0] == '0')
	//	EVRC_Mode = 0;
	//else
	//	printf("Wrong mode number!\n");

#ifdef _WIN32_WCE
	DWORD total = 0;
	TCHAR msg[256];
#if(CODEC_MODE) // set 1 for encode  
	decode_flag = 0;
	encode_flag = 1;
	infileName  = "/Storage Card/EVRC/EXPT1OBJ.pcm";
	outfileName = "/Storage Card/EVRC/EXPT1OBJ.evrc";
#else  //decode
	decode_flag = 1;
	encode_flag = 0;
	infileName  = "/Storage Card/EVRC/EXPT1OBJ2.evrc";
	outfileName = "/Storage Card/EVRC/EXPT1OBJ.evrc.pcm";
#endif
#elif WIN32  //_WIN32_WCE

	if(EVRC_Mode) // set 1 for encode  
	{
		decode_flag = 0;
		encode_flag = 1;
		infileName  = argv[1];
		outfileName = argv[2];
	}
	else  //decode
	{
		decode_flag = 1;
		encode_flag = 0;
		infileName  = argv[1];
		outfileName = argv[2];
	}
#endif  //_WIN32_WCE

#ifdef  RVDS
	infileName = "EXPT1OBJ1.pcm";
	outfileName = "EXPT1OBJ1.evrc";
#endif

#ifdef _IOS
	fsrc = fopen(inFile, "rb");
	fdst = fopen(outFile, "wb");
#else
	if ((fsrc = fopen (infileName, "rb")) == NULL)
	{
		ret = 1;
		goto END;
	}

	if ((fdst = fopen (outfileName, "wb")) == NULL)
	{
		ret = 2;
		goto END;
	}
#endif 

	Relens = GetNextBuf(fsrc,InputBuf,INPUT_SIZE);

	if(Relens < INPUT_SIZE)
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

	if(decode_flag)
	{
#ifdef LINUX
		handle = dlopen("/data/local/tmp/voEVRCDec.so", RTLD_NOW);
		if(handle == 0)
		{
			printf("open dll error......");
			return -1;
		}

		pfunc = dlsym(handle, "voGetEVRCDecAPI");	
		if(pfunc == 0)
		{
			printf("open function error......");
			return -1;
		}

		pGetAPI = (VOGETAUDIODECAPI)pfunc;

		returnCode  = pGetAPI(&AudioAPI);
		if(returnCode)
			return -1;
#else

		returnCode = voGetEVRCDecAPI(&AudioAPI);
		if(returnCode)
		{
			ret = -1;
			goto END;
		}
#endif

		//#######################################   Init Decoding Section   #########################################
		returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingEVRC, &useData);

		if(returnCode)
		{
			ret = 3;
			goto END;
		}

#ifdef NXP
		returnCode = AudioAPI.SetParam(hCodec, VO_NXP_CODEC_KEY, &KeyValue);
		if(returnCode)
		{
			ret = 3;
			goto END;
		}
#endif

		do {
			inData.Buffer    = (unsigned char *)inBuf;
			inData.Length    = Relens;
			outData.Buffer   = (unsigned char *)synth;
			outData.Length   = 320*2;

#ifdef _WIN32_WCE
			t1 = GetTickCount();
#else
			t1 = clock();
#endif
			/* decode one evrc block */
			returnCode = AudioAPI.SetInputData(hCodec,&inData);

			do {
				returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outFormat);

				if(returnCode == 0)
				{
					frameCount++;
					fwrite(outData.Buffer, 1, outData.Length, fdst);
					fflush(fdst);
				}
			} while(returnCode != VO_ERR_INPUT_BUFFER_SMALL);

#ifdef _WIN32_WCE
			t2 = GetTickCount();
			total += t2 - t1;
#else
			t2 = clock();
			duration += t2 - t1;
#endif
			if (!eofFile) {
				Relens = GetNextBuf(fsrc, InputBuf, INPUT_SIZE);
				inBuf = InputBuf;
				if (feof(fsrc) && Relens == 0)
					eofFile = 1;
			}

		} while (!eofFile && returnCode);

		returnCode = AudioAPI.Uninit(hCodec);
	}

	if(encode_flag)
	{
#ifdef LINUX
		handle = dlopen("/data/local/tmp/voEVRCEnc.so", RTLD_NOW);
		if(handle == 0)
		{
			printf("open dll error......");
			return -1;
		}

		pfunc = dlsym(handle, "voGetEVRCEncAPI");	
		if(pfunc == 0)
		{
			printf("open function error......");
			return -1;
		}

		pGetAPI = (VOGETAUDIODECAPI)pfunc;

		returnCode  = pGetAPI(&AudioAPI);
		if(returnCode)
			return -1;
#else
		returnCode = voGetEVRCEncAPI(&AudioAPI);
		if(returnCode)
		{
			ret = -1;
			goto END;
		}
#endif

		//#######################################   Init Encoding Section   #########################################
		returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingEVRC, &useData);

		if(returnCode)
		{
			ret = 3;
			goto END;
		}

		do {
			inData.Buffer    = (unsigned char *)inBuf;
			inData.Length    = Relens;
			outData.Buffer   = (unsigned char *)synth;

#ifdef _WIN32_WCE
			t1 = GetTickCount();
#else
			t1 = clock();
#endif
			/* encode one evrc block */
			returnCode = AudioAPI.SetInputData(hCodec,&inData);

			do {
				returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outFormat);

				if(returnCode == 0)
				{
					frameCount++;
					fwrite(outData.Buffer, 1, outData.Length, fdst);
					fflush(fdst);
				}
			} while(returnCode != VO_ERR_INPUT_BUFFER_SMALL);

#ifdef _WIN32_WCE
			t2 = GetTickCount();
			total += t2 - t1;
#else
			t2 = clock();
			duration += t2 - t1;
#endif
			if (!eofFile) {
				Relens = GetNextBuf(fsrc, InputBuf, INPUT_SIZE);
				inBuf = InputBuf;
				if (feof(fsrc) && Relens == 0)
					eofFile = 1;
			}

		} while (!eofFile && returnCode);

		returnCode = AudioAPI.Uninit(hCodec);
	}

#ifdef _WIN32_WCE
	if(encode_flag)
	{
		wsprintf(msg, TEXT("Encode Time: %d clocks, Encode Frames: %d, fps: %d"), total, frameCount, frameCount*1000/total);
		MessageBox(NULL, msg, TEXT("EVRC Encode Finished"), MB_OK);
	}

	if(decode_flag)
	{
		wsprintf(msg, TEXT("Decode Time: %d clocks, Decode Frames: %d, fps: %d"), total, frameCount, frameCount*1000/total);
		MessageBox(NULL, msg, TEXT("EVRC Decoder Finished"), MB_OK);
	}
#endif

END:
	if (fsrc)
		fclose(fsrc);
	if (fdst)
		fclose(fdst);

#ifdef LINUX
	dlclose(handle);
#endif

	return ret;
}



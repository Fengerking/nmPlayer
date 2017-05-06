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
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    "voEVRC.h"
#include    "cmnMemory.h"

#define     CODEC_MODE      0     //1 encoder, and 0 decoder

#define   L_FRAME 320             //output buffer length, 320 Byte for Decoder, vary Bytes(Max 171bits/frame 23Bytes) for Encoder
#define   INPUT_SIZE   40960
unsigned char  InputBuf[INPUT_SIZE ];

int  GetNextBuf(FILE* inFile,unsigned char* dst,int size)
{
	int size2 = fread(dst, sizeof(signed char), size,inFile);
	return size2;
}

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
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
	char  *inBuf = InputBuf;

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

#ifdef _WIN32_WCE
	DWORD total = 0;
	TCHAR msg[256];
#if CODEC_MODE // set 1 for encode  
	decode_flag = 0;
	encode_flag = 1;
	infileName  = "/Storage Card/EVRC/EXPT1OBJ.pcm";
	outfileName = "/Storage Card/EVRC/EXPT1OBJ.evrc";
#else  //decode
	decode_flag = 1;
	encode_flag = 0;
	infileName  = "/Storage Card/EVRC/EXPT1OBJ_ref.evrc";
	outfileName = "/Storage Card/EVRC/EXPT1OBJ.evrc.pcm";
#endif
#else  //_WIN32_WCE
#if CODEC_MODE // set 1 for encode  
	decode_flag = 0;
	encode_flag = 1;
	infileName  = argv[1];
	outfileName = argv[2];
#else  //decode
	decode_flag = 1;
	encode_flag = 0;
	infileName  = argv[1];
	outfileName = argv[2];
#endif
#endif  //_WIN32_WCE

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
		returnCode = voGetEVRCDecAPI(&AudioAPI);
		if(returnCode)
		{
			ret = -1;
			goto END;
		}

		//#######################################   Init Decoding Section   #########################################
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
		returnCode = voGetEVRCEncAPI(&AudioAPI);
		if(returnCode)
		{
			ret = -1;
			goto END;
		}

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
	return ret;
}



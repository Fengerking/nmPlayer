/*
*  Copyright 2008 by Visualon software Incorporated.
*  All rights reserved. Property of Visualon software Incorporated.
*  Restricted rights to use, duplicate or disclose this code are
*  granted through contract.
*  
*/
/***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver         Description             Author
;*    -----------     --------     ------------------        -----------
;*    11-12-2008        1.0        File imported from        Huaping Liu
;*                                             
;**********************************************************************/
#ifdef _WIN32_WCE
#include      <windows.h>
#include      <objbase.h>
#include      <Winbase.h>
#else
#ifdef LINUX
#include <dlfcn.h>
#endif
#endif // _WIN32_WCE
#include      <stdio.h>
#include      <stdlib.h>
#include      <time.h>
#include      "voAMRWB.h"
#include      "cmnMemory.h"

//Assume the input file is IF1
#define AMR_MAGIC_NUMBER "#!AMR-WB\n"

#define  INPUT_SIZE   640
#define  OUTPUT_SIZE  1024
unsigned char  InputBuf[INPUT_SIZE];
unsigned char  OutputBuf[OUTPUT_SIZE];

int  GetNextBuf(FILE* inFile,unsigned char* dst,int size)
{
	int size2 = (int)fread(dst, sizeof(signed char), size,inFile);
	return size2;
}

typedef int (VO_API * VOGETAUDIOENCAPI) (VO_AUDIO_CODECAPI * pEncHandle);

int encode(int mode, const char* srcfile, const char* dstfile)
{
	int			ret = 0;
	int         returnCode;
	FILE		*fsrc = NULL;
	FILE		*fdst = NULL;
	int         framenum = 0;
	int         eofFile = 0;
	int         size1;
	int         Relens;
	short       allow_dtx = 0;

	VO_AUDIO_CODECAPI       AudioAPI;
	VO_MEM_OPERATOR         moper;
	VO_CODEC_INIT_USERDATA  useData;
	VO_HANDLE               hCodec;
	VO_CODECBUFFER          inData;
	VO_CODECBUFFER          outData;
	VO_AUDIO_OUTPUTINFO     outFormat;

	unsigned char *inBuf = InputBuf;
	unsigned char *outBuf = OutputBuf;
	VOAMRWBFRAMETYPE frameType = VOAMRWB_RFC3267;

#ifdef LINUX
	void  *handle;
	void  *pfunc;
    VOGETAUDIOENCAPI pGetAPI;
#endif

#ifdef _WIN32_WCE
	TCHAR   msg[256];
	int     t1, t2 = 0;	
#else
	clock_t   start, finish;
	double    duration = 0.0;
#endif
	if ((fsrc = fopen (srcfile, "rb")) == NULL)
	{
		ret = 1;
		goto safe_exit;
	}

	if ((fdst = fopen (dstfile, "wb")) == NULL)
	{
		ret = 2;
		goto safe_exit;
	}

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);

#ifdef LINUX
	handle = dlopen("/data/local/tmp/voAMRWBEnc.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}
	
	pfunc = dlsym(handle, "voGetAMRWBEncAPI");	
	if(pfunc == 0)
	{
		printf("open function error......");
		return -1;
	}

	pGetAPI = (VOGETAUDIOENCAPI)pfunc;

	returnCode  = pGetAPI(&AudioAPI);
	if(returnCode)
		return -1;
#else
	ret = voGetAMRWBEncAPI(&AudioAPI);
	if(ret)
	{
		ret = -1;
		goto safe_exit;
	}
#endif 

	//#######################################   Init Encoding Section   #########################################
	ret = AudioAPI.Init(&hCodec, VO_AUDIO_CodingAMRWB, &useData);

	if(ret)
	{
		ret = 3;
		goto safe_exit;
	}

	Relens = GetNextBuf(fsrc,InputBuf,INPUT_SIZE);

	if(Relens!=INPUT_SIZE && !feof(fsrc))
	{
		ret = 5; //Invalid magic number
		goto safe_exit;
	}

	//###################################### set encode Mode ##################################################
	ret = AudioAPI.SetParam(hCodec, VO_PID_AMRWB_FRAMETYPE, &frameType);
	ret = AudioAPI.SetParam(hCodec, VO_PID_AMRWB_MODE, &mode);
	ret = AudioAPI.SetParam(hCodec, VO_PID_AMRWB_DTX, &allow_dtx);

	if(frameType == VOAMRWB_RFC3267)
	{
		/* write magic number to indicate single channel AMR file storage format */
		size1 = (int)strlen(AMR_MAGIC_NUMBER);
		memcpy(outBuf,AMR_MAGIC_NUMBER,size1);
		outBuf += size1;
	}

	//#######################################   Encoding Section   #########################################
	do{
		inData.Buffer = (unsigned char *)inBuf;
		inData.Length = Relens;
		outData.Buffer = outBuf;

#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		start = clock();
#endif
		/* decode one amrwb block */
		returnCode = AudioAPI.SetInputData(hCodec,&inData);

		do {
			returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outFormat);
			//if(returnCode == 0)
			//{
			//	framenum++;
			//	if(framenum == 1)
			//	{
			//		fwrite(OutputBuf, 1, outData.Length + size1, fdst);
			//		fflush(fdst);	
			//	}
			//	else
			//	{
			//		fwrite(outData.Buffer, 1, outData.Length, fdst);
			//		fflush(fdst);
			//	}
			//}
		} while(returnCode != VO_ERR_INPUT_BUFFER_SMALL);

#ifdef _WIN32_WCE
		t2 += (GetTickCount() - t1);
#else
		finish = clock();
		duration += finish - start;
#endif
		if (!eofFile) {
			Relens = GetNextBuf(fsrc, InputBuf, INPUT_SIZE);
			inBuf = InputBuf;
			if (feof(fsrc) && Relens == 0)
				eofFile = 1;
		}
	} while (!eofFile && returnCode);
	//#######################################   End Encoding Section   #########################################

safe_exit:
	returnCode = AudioAPI.Uninit(hCodec);

#ifdef _WIN32_WCE
	wsprintf(msg, TEXT("Encode Time: %d clocks, Encode frames: %d"), t2, framenum);
	MessageBox(NULL, msg, TEXT("AMR Encode Finished"), MB_OK);
#else
	printf( "\n%2.5f seconds\n", (double)duration);
#endif

	if (fsrc)
		fclose(fsrc);
	if (fdst)
		fclose(fdst);

#ifdef LINUX
	dlclose(handle);
#endif

	return ret;
}

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
int main(int argc, char **argv)  // for gcc compiler;
#endif//_WIN32_WCE
{
	int   mode, r;
	char   *inFileName,*outFileName;

#ifdef _WIN32_WCE
	mode = VOAMRWB_MD66;
	inFileName  = "/Storage Card/AMRWB/output.pcm";
	outFileName = "/Storage Card/AMRWB/output.awb";
#else
	if (argc != 3)
	{
		fprintf(stderr, "\nUsage: %s mode srcfile.pcm dstfile.amr\n", argv[0]);
		return -1;
	}
	mode = VOAMRWB_MD2385;
	inFileName  = argv[1];
	outFileName = argv[2];
#endif
	r = encode(mode, inFileName, outFileName);
	if(r)
	{
		fprintf(stderr, "error: %d\n", r);
	}
	return r;
}


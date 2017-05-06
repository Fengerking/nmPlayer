/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2010	    *
*									                                    *
************************************************************************/
/************************************************************************
File:		voAPE_D_Sample.c

Contains:	APE Lossless Codec Decoder Sample code.

Written by:	Huaping Liu

Change History (most recent first):
2009-11-11		LHP			Create file

*************************************************************************/
#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#else
#ifdef LINUX
#include <dlfcn.h>
#endif
#endif // _WIN32_WCE

#include  "stdio.h"
#include  "string.h"
#include  "voAPEDec.h"
#include  "cmnMemory.h"

#define   BUF_LEN    1024*10

unsigned char  In_Buf[BUF_LEN];
unsigned char  Out_Buf[BUF_LEN];

int  GetNextBuf(FILE* inFile,unsigned char* dst,int size)
{
	int size2 = fread(dst, sizeof(signed char), size,inFile);
	return size2;
}

void usage()
{
	printf("Proper usage: [EXE] [input_file] [output_file] \n");
	printf("such as: \n");
	printf("./APE.exe E:/give_me.ape D:/give_me.pcm \n");

}

#ifdef LINUX
typedef int (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);
#endif

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv)
#else
int main(int argc, char *argv[])
#endif
{
	long int      t1, t2;
	char          *infileName;
	char          *outfileName;
	int           ret = 0;
	FILE          *fsrc = NULL;
	FILE          *fdst = NULL;
	int           read_len = 0;
	int           endFile = 0; 
	int           returnCode = 0;
	int           frameCount = 0;
	int           mv_len = 0;
	double        duration = 0.0;

	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER OutData;
	VO_AUDIO_OUTPUTINFO outFormat;

	//VOCODECBUFFER inData;
	//VOCODECBUFFER OutData;

#ifdef LINUX
	void  *handle;
	void  *pfunc;
	VOGETAUDIODECAPI pGetAPI;	
#endif

#ifdef _WIN32_WCE
	DWORD total = 0;
	TCHAR msg[256];
	infileName = "/Storage Card2/APE/I_am_fine.ape";
	outfileName = "/Storage Card2/APE/output.pcm";
#else
	if(argc < 3)
	{
		usage();
		printf("please input file names \n");
	}
	infileName  = argv[1];
	outfileName = argv[2];
#endif
	if(!(fsrc = fopen(infileName, "rb")))
	{
		printf("open input file is fail!\n");
	}

	if(!(fdst = fopen(outfileName, "wb+")))
	{
		printf("open output file is fail!\n");
	}

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);

#ifdef LINUX
	handle = dlopen("/data/local/tmp/voAPEDec.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}

	pfunc = dlsym(handle, "voGetAPEDecAPI");	
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
	returnCode = voGetAPEDecAPI(&AudioAPI);
	if(returnCode)
	{
		ret = -1;
	}
#endif

	//#######################################   Init Decoding Section   #########################################
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingAPE, &useData);
	if(returnCode)
	{
		ret = -1;
		goto safe_exit;
	}

	/* Set APE Header Info */
	/*returnCode = AudioAPI.SetParam(hCodec, VO_AUDIO_CodingAPE, &HeaderInfo);
	if(returnCode)
	{
	ret = -1;
	goto safe_exit;
	}*/

	read_len = GetNextBuf(fsrc, In_Buf, BUF_LEN);

	if(read_len != BUF_LEN)
	{
		goto safe_exit;
	}
	//#######################################    Decoding Section   #########################################
	do{
		inData.Buffer = In_Buf;
		inData.Length = BUF_LEN;
		OutData.Buffer = Out_Buf;

#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		t1 = clock();
#endif	
		/* Set Input Data */	

		returnCode = AudioAPI.SetInputData(hCodec, &inData);

		do {
			returnCode = AudioAPI.GetOutputData(hCodec, &OutData, &outFormat);

			frameCount++;
			if(returnCode == 0)
			{	
				fwrite(OutData.Buffer, 1, OutData.Length, fdst);
				fflush(fdst);
			}
			if(frameCount > 700)
			   goto safe_exit;

		} while(returnCode != VO_ERR_INPUT_BUFFER_SMALL);

#ifdef _WIN32_WCE
		t2 = GetTickCount();
		total += t2 - t1;
#else
		t2 = clock();
		duration += t2 - t1;
#endif
		if (!endFile) {
			read_len = GetNextBuf(fsrc, In_Buf, BUF_LEN);
			if (feof(fsrc) && read_len == 0)
				endFile = 1;
		}

	}while(!endFile && returnCode);

safe_exit:

#ifdef _WIN32_WCE	
	wsprintf(msg, TEXT("Decode Time: %d clocks, Decode Frames: %d, fps: %d"), total, frameCount,(1000*frameCount)/total);
	MessageBox(NULL, msg, TEXT("APE Decoder Finished"), MB_OK);
#else
	printf( "\n%2.5f seconds\n", (double)duration);
#endif



	returnCode = AudioAPI.Uninit(hCodec);

	if(returnCode != 0)
	{
		printf("#### VOI_Error4:fail to destroy the decoder ###\n");
	}

	if(fsrc)
		fclose(fsrc);

	if(fdst)
		fclose(fdst);

#ifdef LINUX
	dlclose(handle);
#endif
	return ret;
}







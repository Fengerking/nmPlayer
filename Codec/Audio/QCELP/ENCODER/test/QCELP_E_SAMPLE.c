#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#else
#ifdef LINUX
#include <dlfcn.h>
#endif
#endif // _WIN32_WCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "voQCELP.h"
#include "cmnMemory.h"

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


#define   L_FRAME       40        //QCELP Encoder output: full bitrate 266bit(35Bytes) 
#define   INPUT_SIZE    40960     //input buffer size 40 * 1024 = 40K
unsigned char InputBuf[INPUT_SIZE];

int  GetNextBuf(FILE* inFile, unsigned char* dst, int size)
{
	int size2 = (int)fread(dst, sizeof(signed char), size,inFile);
	return size2;
}

typedef int (VO_API * VOGETAUDIOENCAPI) (VO_AUDIO_CODECAPI * pEncHandle);

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv)
//int WinMain()
#else // _WIN32_WCE
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	FILE  *fsrc = NULL;
	FILE  *fdst = NULL;
	VO_HANDLE hCodec;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;
	int  Relens = 0;
	int  frameCount = 0;
	int  eofFile = 0;
	int  returnCode, ret;
	unsigned char   *inBuf = InputBuf;
	unsigned char   output[40];
	long  start=0, end=0, total=0;
	void  *handle;
	void  *pfunc;
	VOGETAUDIOENCAPI pGetAPI;

	if(argc < 3)
	{
		printf ("QCELP Encoder HELP   Displays this text\n");
		printf ("\n");
		printf ("Usage:\n");
		printf ("voQCELPEnc Input_file output_file \n");
		return -1;
	}

#ifdef _WIN32_WCE
	//TCHAR msg[256];
	if(!(fsrc = fopen ("/Storage Card/QCELP/EXPT1OBJ1.pcm", "rb"))){
		return -1;
	}	

	if(!(fdst = fopen("/Storage Card/QCELP/test1.QCELP", "w+b"))){
		return -1;
	}
#else
	if((fsrc=fopen(argv[1],"rb")) == NULL)
	{
		return -1;
	}

	if((fdst=fopen(argv[2],"wb")) == NULL)
	{
		return -1;
	}
#endif

	Relens = GetNextBuf(fsrc, InputBuf, INPUT_SIZE);
	if(Relens != INPUT_SIZE)
	{
		eofFile = 0;
	}

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);

#ifdef LINUX
	handle = dlopen("/data/local/tmp/voQCELPEnc.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}

	pfunc = dlsym(handle, "voGetQCELPEncAPI");	
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
	returnCode = voGetQCELPEncAPI(&AudioAPI);
	if(returnCode)
	{
		ret = -1;
		goto safe_exit;
	}
#endif
	//#######################################   Init Encoding Section   #########################################
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingQCELP13, &useData);

	if(returnCode < 0)
	{
		printf("#### VOI_Error2:fail to initialize the decoder###\n");
		ret = -1;
		goto safe_exit;
	}
	
	total = 1;
	AudioAPI.SetParam(&hCodec, VO_PID_QCELP_RDA, &total);

	do {
		inData.Buffer    = (unsigned char *)inBuf;
		inData.Length    = Relens;
		outData.Buffer   = (unsigned char *)output;

#ifdef _WIN32_WCE
		start = GetTickCount();
#else
		start = clock();
#endif
		/* decode one amr block */
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
		end = GetTickCount();
		total += end - start;
#else
		end = clock();
		total += end - start;
#endif
		if (!eofFile) {
			Relens = GetNextBuf(fsrc, InputBuf, INPUT_SIZE);
			inBuf = InputBuf;
			if (feof(fsrc) && Relens == 0)
				eofFile = 1;
		}

	} while (!eofFile && returnCode);

safe_exit:
	AudioAPI.Uninit(hCodec);

#ifdef _WIN32_WCE	
	//wsprintf(msg, TEXT("Encode Time: %d clocks"), total);

	//yover, MessageBox(NULL, msg, TEXT("QCELP Encode Finished"), MB_OK);
#else
	printf("Used Time: %10d\n", total);
#endif
	fclose(fsrc);
	fclose(fdst);
#ifdef LINUX
	dlclose(handle);
#endif
	return 0;
}/* end of main() */



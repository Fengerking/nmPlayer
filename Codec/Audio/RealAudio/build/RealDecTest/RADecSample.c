#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE

#ifdef LINUX
#include <dlfcn.h>
#endif

#define RAWMODE 0
#define FMTMODE	1
#define BUFMODE 2

#define EXAMPLEMODE  FMTMODE

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<time.h>
#include		<limits.h>
#include		"voRealAudio.h"
#include		"cmnMemory.h"
static int	outTotalFrame = INT_MAX;
static VORA_RAW_INIT_PARAM  initParam;
static char  configfile[256];
static FILE* fconfig=NULL;
static int nDecodeNextFile=0;

int ReadFile2Buf(FILE* infile,unsigned char* dest,int readSize)
{
	int readBytes = 0;
	readBytes = fread(dest, 1, readSize, infile);
	return readBytes;
}

static void ParseConfig(char* inFile,char* outFile,char* speedFile,int* loopCount,int* outFrameNumber)
{
	nDecodeNextFile = 0;//default is 0
	if(fconfig)
	{
		//printf("file pos=%d\n",ftell(fconfig));
		fscanf(fconfig,"%*[^\"]");
		fscanf(fconfig,"\"%[^\"]",inFile);
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		fscanf(fconfig,"\"%[^\"]",outFile);               // RAW (YUV/RGB) output file
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		fscanf(fconfig,"\"%[^\"]",speedFile);               // speed file
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",outFrameNumber);           
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",loopCount);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&(initParam.version));          
		fscanf(fconfig,"%*[^\n]");



		fscanf(fconfig,"%d",&(initParam.samplesPerFrame));          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&(initParam.channelNum));          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&(initParam.sample_rate));          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&(initParam.region));          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&(initParam.frameSizeInBits));          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&(initParam.cplStart));          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&(initParam.cplQbits));          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&nDecodeNextFile);          
		fscanf(fconfig,"%*[^\n]");
		//printf("file pos=%d\n",ftell(fconfig));

	}
}

static char* sdcard[]={"storage card","sd memory","sdmmc"};
enum
{
	STORAGE_CARD=0,
	SD_MEMORY,
	SDMMC
};
FILE *inFile, *outFile, *speedFile, *paramFile;
int frameNumber;

#if EXAMPLEMODE == RAWMODE

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv) 
#endif // _WIN32_WCE
{
	char *inputBuf,*outputBuf=NULL;
	int returnCode;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outInfo;
	void* state;

	char inFileName[256], outFileName[256], outSpeedName[256];
	double duration;
	int outTotalFrame = INT_MAX;
	int format;
	int loopCount=1;
	int inSize = 0;
	int inFrame = 0;
	int	endOfFile = 0;
	int IsUseCallBack = 1;
	char* nextFrame,*currFrame;
	int leftSize=0;
	int size=0;
	int Isoutput = 1;
#ifdef _WIN32_WCE
	DWORD start, finish;
#define TEST_SPEED 1

	int sdPath=STORAGE_CARD;
	if(TEST_SPEED)
		sprintf(configfile,"/%s/ra_ppc/%s/ra_cfg_s.txt",sdcard[sdPath],sdcard[sdPath]);
	else
		sprintf(configfile,"/%s/ra_ppc/%s/ra_cfg.txt",sdcard[sdPath],sdcard[sdPath]);

	fconfig = fopen(configfile,"r");
#else
	int i;
	clock_t start, finish;
	strcpy(configfile,argv[1]);
	fconfig = fopen(configfile,"r");
#endif

	do
	{
		ParseConfig(inFileName,outFileName,outSpeedName,&loopCount,&outTotalFrame);
		if (outSpeedName[0] != '\0')
			speedFile = fopen(outSpeedName, "wb");
		else
			speedFile = NULL;

		if (!speedFile){
			printf("\nWarning: no output speed file!");
		}
#define MAX_INPUT_BUFSIZE  128*1024
#define MAX_OUTPUT_BUFSIZE 2048*2;
		// allocate  buffer;
		inputBuf = (char *)malloc(MAX_INPUT_BUFSIZE);//
		outputBuf = (char *)malloc(MAX_INPUT_BUFSIZE);//
		outData.Buffer = outputBuf;
		do 
		{
			endOfFile = 0;
			inFile= fopen (inFileName, "rb");
			if (!inFile){
				printf("\nError: cannot open input RealVideo file!%s",inFileName);
				exit(0);
			}

			if (outFileName[0] != '\0'&&loopCount==1)
				outFile = fopen(outFileName, "wb");
			else
				outFile = NULL;

			if (!outFile){
				printf("\nWarning: no output video file!");
			}

			moper.Alloc = cmnMemAlloc;
			moper.Copy = cmnMemCopy;
			moper.Free = cmnMemFree;
			moper.Set = cmnMemSet;
			moper.Check = cmnMemCheck;

			useData.memflag = VO_IMF_USERMEMOPERATOR;
			useData.memData = (VO_PTR)(&moper);

			returnCode  = voGetRADecAPI(&AudioAPI);
			if(returnCode<0)	
			{
				printf("\nError %d: Decoder init failed!\n", returnCode);
				return 1;
			}

			returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingRA, &useData);
			if(returnCode < 0)
			{
				printf("#### VOI_Error2:fail to initialize the Encoderr###\n");
				return -1;
			}

			returnCode = returnCode = AudioAPI.SetParam(hCodec, VOID_PID_RA_RAW_INIT, &initParam);
			if(returnCode<0)	
			{
				printf("\nError %d: Decoder init SetParameter failed!\n", returnCode);
				return 1;
			}

			frameNumber = 0;
			currFrame = inputBuf;
			leftSize = fread(currFrame,1,MAX_INPUT_BUFSIZE,inFile);
			inSize = initParam.frameSizeInBits/8;

#ifdef _WIN32_WCE
			start = GetTickCount();
#else
			start = clock();
#endif
			while(frameNumber < outTotalFrame){


				if(leftSize<inSize)
				{
					if(!feof(inFile))
					{
						int readSize;
						memmove(inputBuf,currFrame,leftSize);
						currFrame=inputBuf;
						readSize=fread(currFrame+leftSize,1,MAX_INPUT_BUFSIZE-leftSize,inFile);
						leftSize+=readSize;
					}
					else
						break;
				}

				leftSize-=inSize;
				inData.Length = inSize;
				inData.Buffer = currFrame;
				currFrame += inSize;

				frameNumber++;

				returnCode = AudioAPI.SetInputData(hCodec,&inData);

				do {
					outData.Buffer   = outputBuf;
					outData.Length = MAX_INPUT_BUFSIZE;

					returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outInfo);

					if(returnCode == 0)
						frameNumber++;

					if (Isoutput && returnCode == 0)
					{
						fwrite(outData.Buffer, 1, outData.Length, outFile);
						printf("Enocding frame: %5d\n", frameNumber);
					}
				} while(returnCode != (VO_ERR_INPUT_BUFFER_SMALL));
			}

			returnCode = AudioAPI.Uninit(hCodec);

#ifdef _WIN32_WCE
			finish = GetTickCount();
			duration = (finish - start) * 0.001;
#else
			finish = clock();
			duration = (double)(finish - start) / CLOCKS_PER_SEC;
			printf("\nRealVideo decoder speed = %2.3f (fps)", frameNumber/duration);
			printf("\n\ttotal frame = %d, total time = %2.3f seconds\n", 
				frameNumber, duration);
#endif //_WIN32_WCE

			fclose(inFile);
			inFile=NULL;
			if (outFile)
				fclose(outFile);
			outFile=NULL;
		} while(--loopCount>0);

		free(inputBuf);
		free(outputBuf);

		fclose (speedFile);
		speedFile=NULL;
	}while (nDecodeNextFile);

	fclose(fconfig);
	fconfig= NULL;
	return 0;
}

#elif EXAMPLEMODE == FMTMODE

unsigned char bbuf[16] = {1, 0, 0, 3, 8, 0 ,0, 37, 0, 0, 0, 0, 0, 8, 0, 5};

typedef struct ra_block_struct
{
	unsigned char*  pData;
	unsigned int ulDataLen;
	unsigned int ulTimestamp;
	unsigned int ulDataFlags;
} ra_block;

typedef int (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
int main(int argc, char **argv) 
#endif // _WIN32_WCE
{
	char *inputBuf,*outputBuf=NULL;
	char *ConfigDate;
	int returnCode;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outInfo;
	VORA_INIT_PARAM  *poutFormat;
	VORA_FORMAT_INFO *pformat;

#ifdef LINUX
	void  *handle;
	void  *pfunc;
	VOGETAUDIODECAPI pGetAPI;
#endif
	//ra_block rbk;	
#ifdef _WIN32_WCE	
	TCHAR msg[256];
#endif
	void* state;
	ra_block* pBlock;
	double duration;
	int outTotalFrame = INT_MAX;
	int loopCount=1;
	int inSize = 0;
	int inFrame = 0;
	int	endOfFile = 0;
	int IsUseCallBack = 1;
	int leftSize=0;
	int size=0;
	int	t1,t2,total;
	int Isoutput = 1;
	int start, end;
	int currentPos =0;

#define MAX_INPUT_BUFSIZE  128*1024
#define MAX_OUTPUT_BUFSIZE 2048*2
#define MAX_CONFIG_BUFSIZE 2048*4
	// allocate  buffer;
	inputBuf = (char *)malloc(MAX_INPUT_BUFSIZE);//
	outputBuf = (char *)malloc(MAX_INPUT_BUFSIZE);//
	ConfigDate = (char *)malloc(MAX_CONFIG_BUFSIZE);
	memset(outputBuf, 0, MAX_INPUT_BUFSIZE);

#ifdef _WIN32_WCE
	inFile= fopen ("/rm/rawdata1.data", "rb");
	if (!inFile){
		printf("\nError: cannot open input RealAudio file!%s",argv[1]);
		exit(0);
	}

	outFile = fopen("/rm/rawdata1.pcm", "wb");
	if (!outFile){
		printf("\nWarning: no output audio file!");
	}
#else
	inFile= fopen (argv[1], "rb");
	if (!inFile){
		printf("\nError: cannot open input RealAudio file!%s",argv[1]);
		exit(0);
	}
	fseek(inFile, 0, SEEK_END);
    currentPos = ftell(inFile); 
	fseek(inFile,0,SEEK_SET);
	outFile = fopen(argv[2], "wb");
	if (!outFile){
		printf("\nWarning: no output audio file!");
	}
#endif
	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);

#ifdef LINUX
	handle = dlopen("/data/local/tmp/realaudio/libvoRADec.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}

	pfunc = dlsym(handle, "voGetRADecAPI");	
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
	returnCode  = voGetRADecAPI(&AudioAPI);
	if(returnCode<0)	
	{
		printf("\nError %d: Decoder init failed!\n", returnCode);
		return 1;
	}
#endif

	leftSize = fread(&inData.Length,1, 4,inFile);
	if(leftSize != 4)
		return -1;

	if(inData.Length < sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO))
	{
		return -1;
	}

	leftSize = fread(ConfigDate,1, inData.Length,inFile);
	if(leftSize != inData.Length)
		return -1;

	inData.Buffer = ConfigDate;

	poutFormat = (VORA_INIT_PARAM*)ConfigDate;
	pformat = poutFormat->format =(VORA_FORMAT_INFO*)( ConfigDate+sizeof(VORA_INIT_PARAM));
	if(pformat->ulOpaqueDataSize>0)
	{
		pformat->pOpaqueData = ( ConfigDate+sizeof(VORA_INIT_PARAM)+ sizeof(VORA_FORMAT_INFO));
	}
	else
		pformat->pOpaqueData = NULL;
	if(!(pformat->ulOpaqueDataSize + sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO) == inData.Length  && 
		(pformat->usNumChannels > 0 && pformat->usNumChannels <= 6) && 
		pformat->usAudioQuality <= 100))
	{
		return -1;
	}

	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingRA, &useData);
	if(returnCode < 0)
	{
		printf("#### VOI_Error2:fail to initialize the Encoderr###\n");
		return -1;
	}

	returnCode = AudioAPI.SetParam(hCodec, VOID_PID_RA_FMT_INIT,&inData);
	if(returnCode<0)	
	{
		printf("\nError %d: Decoder init SetParameter failed!\n", returnCode);
		return 1;
	}

	total = 0; 
	leftSize = ReadFile2Buf(inFile,inputBuf, pformat->ulGranularity + sizeof(ra_block));
	//if(leftSize < pformat->ulGranularity + sizeof(ra_block))
	//	break;
	currentPos -= leftSize;

	inSize = pformat->ulGranularity;

	pBlock = inputBuf;
	pBlock->pData = (unsigned char*)(inputBuf + sizeof(ra_block));
	inData.Length = inSize + sizeof(ra_block);
	inData.Buffer = (VO_PBYTE)pBlock;
	do{
		returnCode = AudioAPI.SetInputData(hCodec,&inData);
		do {
			outData.Buffer   = outputBuf;
			outData.Length = MAX_INPUT_BUFSIZE;

#ifdef _WIN32_WCE
			start = GetTickCount();
#else
			start = clock();
#endif
			returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outInfo);
#ifdef _WIN32_WCE
			end = GetTickCount();
			total += end - start;
#else
			end = clock();
			total += end - start;
#endif

			if(returnCode == 0)
				frameNumber++;

			if (Isoutput && returnCode == 0)
			{
				fwrite(outData.Buffer, 1, outData.Length, outFile);
			}
		} while(returnCode != (VO_ERR_INPUT_BUFFER_SMALL));

		leftSize = ReadFile2Buf(inFile,inputBuf, pformat->ulGranularity + sizeof(ra_block));

		currentPos -= leftSize;
		if (currentPos <= pformat->ulGranularity + sizeof(ra_block))
		{
			printf("Real Audio decoder finish \n");
			printf("Decode Time: %d clocks\n ", total);
			printf("Frame Number: %d\n", frameNumber);
			endOfFile = 1;
		}
		else
		{
			inSize = pformat->ulGranularity;
			pBlock = inputBuf;
			pBlock->pData = (unsigned char*)(inputBuf + sizeof(ra_block));
			inData.Length = inSize + sizeof(ra_block);
			inData.Buffer = (VO_PBYTE)pBlock;
		}

	}while(!endOfFile && returnCode);

#ifdef _WIN32_WCE
	wsprintf(msg, TEXT("Decode Time: %d clocks, Frame Number: %d"), total, frameNumber);
	MessageBox(NULL, msg, TEXT("RM Decode Finished"), MB_OK);
#endif

	printf("Decode Time: %d ms, Frame Number: %d", total, frameNumber);

	returnCode = AudioAPI.Uninit(hCodec);

	free(inputBuf);
	free(outputBuf);
	free(ConfigDate);
	fclose(inFile);
	inFile=NULL;
	if (outFile)
		fclose(outFile);
	outFile=NULL;

#ifdef LINUX
	dlclose(handle);
#endif

	return 0;
}

#elif EXAMPLEMODE == BUFMODE

unsigned char bbuf[16] = {1, 0, 0, 2, 4, 0 ,0, 47, 0, 0, 0, 0, 0, 2, 0, 4};

typedef struct ra_block_struct
{
	unsigned char*  pData;
	unsigned int ulDataLen;
	unsigned int ulTimestamp;
	unsigned int ulDataFlags;
} ra_block;

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv) 
#endif // _WIN32_WCE
{
	char *inputBuf,*outputBuf=NULL;
	char *paramBuf;
	int returnCode;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outInfo;

	double duration;
	int outTotalFrame = INT_MAX;
	int loopCount=1;
	int inSize = 0;
	int inFrame = 0;
	int	endOfFile = 0;
	int IsUseCallBack = 1;
	int leftSize=0;
	int size=0;
	int Isoutput = 1;


#define MAX_INPUT_BUFSIZE  128*1024
#define MAX_OUTPUT_BUFSIZE 2048*2
	// allocate  buffer;
	inputBuf = (char *)malloc(MAX_INPUT_BUFSIZE);
	outputBuf = (char *)malloc(MAX_INPUT_BUFSIZE);
	paramBuf = (char *)malloc(MAX_OUTPUT_BUFSIZE);
	memset(outputBuf, 0, MAX_INPUT_BUFSIZE);
	memset(paramBuf, 0, MAX_OUTPUT_BUFSIZE);
	outData.Buffer = outputBuf;


	inFile= fopen (argv[1], "rb");
	if (!inFile){
		printf("\nError: cannot open input RealAudio file!%s",argv[1]);
		exit(0);
	}

	outFile = fopen(argv[2], "wb");
	if (!outFile){
		printf("\nWarning: no output audio file!");
	}

	paramFile = fopen(argv[3], "rb");
	if (!outFile){
		printf("\nWarning: no param audio file!");
	}

	inSize = fread(paramBuf, 1, MAX_OUTPUT_BUFSIZE, paramFile);

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);

	returnCode  = voGetRADecAPI(&AudioAPI);
	if(returnCode<0)	
	{
		printf("\nError %d: Decoder init failed!\n", returnCode);
		return 1;
	}

	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingRA, &useData);
	if(returnCode < 0)
	{
		printf("#### VOI_Error2:fail to initialize the Encoderr###\n");
		return -1;
	}

	inData.Buffer = paramBuf;
	inData.Length = inSize;
	returnCode = AudioAPI.SetParam(hCodec, VOID_PID_RA_BUF_INIT, &inData);
	if(returnCode<0)	
	{
		printf("\nError %d: Decoder init SetParameter failed!\n", returnCode);
		return 1;
	}

	returnCode = AudioAPI.GetParam(hCodec, VOID_PID_RA_BLOCKSIZE, &inSize);
	if(returnCode<0)	
	{
		printf("\nError %d: Decoder init SetParameter failed!\n", returnCode);
		return 1;
	}

	while(1){

		frameNumber++;
		//printf("Input: %d(size=%d),total=%d\n",frameNumber,inSize,size+=inSize);
		outData.Length = MAX_INPUT_BUFSIZE;

		leftSize = fread(inputBuf,1,inSize,inFile);
		if(leftSize < inSize)
			break;
		inSize = inSize;

		inData.Length = inSize;
		inData.Buffer = inputBuf;	

		returnCode = AudioAPI.SetInputData(hCodec,&inData);

		do {
			outData.Buffer   = outputBuf;
			outData.Length = MAX_INPUT_BUFSIZE;

			returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outInfo);

			if(returnCode == 0)
				frameNumber++;

			if (Isoutput && returnCode == 0)
			{
				fwrite(outData.Buffer, 1, outData.Length, outFile);
				printf("Enocding frame: %5d\n", frameNumber);
			}
		} while(returnCode != (VO_ERR_INPUT_BUFFER_SMALL));
	}

	returnCode = AudioAPI.Uninit(hCodec);

	free(inputBuf);
	free(outputBuf);
	free(paramBuf);
	fclose(inFile);
	inFile=NULL;
	if (outFile)
		fclose(outFile);
	outFile=NULL;

	return 0;
}
#endif
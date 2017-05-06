	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2004				*
	*																		*
	************************************************************************/

#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<time.h>
#include		"voDRA.h"
#include		"cmnMemory.h"

#define READ_SIZE	(1024*4)	
short outBuf[1024*8];
unsigned char inBuf[READ_SIZE];

void Usage(void)
{
	printf("\nvoi_dra_decoder Usage:\n");
	printf(">> voi_dra_decoder infile.dra outfile.pcm\n");
       
}

int ReadFile2Buf(FILE* infile,unsigned char* dest,int readSize)
{
	int readBytes = 0;
	readBytes = fread(dest, 1, readSize, infile);
	return readBytes;
}

int frameCount =0;

#ifdef _WIN32_WCE

#define MAX_PATH          260
#define MAX_CMDLINE (MAX_PATH*2+16)
#define MAX_ARGV      10

LPCSTR g_parfile = "/DRADecapp.par";
FILE* fconfig;

static void ParseConfig(char* inFile,char* outFile,int* bwrite)
{
	if(fconfig)
	{
		//printf("file pos=%d\n",ftell(fconfig));
		fscanf(fconfig,"%*[^\"]");
		fscanf(fconfig,"\"%[^\"]",inFile);
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		fscanf(fconfig,"\"%[^\"]",outFile);               
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",bwrite);           
		fscanf(fconfig,"%*[^\n]");	
	}
}
#endif

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
{
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv)
{
#endif//_WIN32_WCE
	
	FILE *infile, *outfile;
	int		t1,t2;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outInfo;
    int firstWrite = 1;
	int eofFile = 0;
	int* info=(int*)inBuf;
	int bytesLeft, nRead;
	int sampleRate;
	int decodedFrame = 0;
	int makeError = 0;
	int total = 0;
	int   Isoutput = 1;
	int	returnCode;
#define  OUTPUT	  1
	
#ifdef _WIN32_WCE
	TCHAR msg[256];
	char infileName[MAX_PATH];
	char outfileName[MAX_PATH];

	if(!(fconfig = fopen(g_parfile, "rb")))
	{
		wsprintf(msg, TEXT("open DRA par file error!!"));
		MessageBox(NULL, msg, TEXT("DRA Decode error"), MB_OK);
		return 0;
	}
	
	ParseConfig(infileName,outfileName, &Isoutput);

#else// _WIN32_WCE
    const char *infileName = argv[1];
    const char *outfileName = argv[2];

#endif//_WIN32_WCE 
	/* open input file */
	infile = fopen(infileName, "rb");
	if (!infile) {
#ifdef _WIN32_WCE
		wsprintf(msg, TEXT("open dra file error!!"));
		MessageBox(NULL, msg, TEXT("DRA Decode error"), MB_OK);
#endif
		return -1;
	}

	/* open output file */
	if(Isoutput)
	{
		outfile = fopen(outfileName, "wb"); 
		if (!outfile) {
#ifdef _WIN32_WCE
			wsprintf(msg, TEXT("open pcm file error!!"));
			MessageBox(NULL, msg, TEXT("DRA Decode error"), MB_OK);
#endif
			return -1;
		}
	}

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);

	returnCode  = voGetDRADecAPI(&AudioAPI);
	if(returnCode)
		return -1;

//#######################################   Init Decoding Section   #########################################
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingDRA, &useData);
	//returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingDRA, NULL);
	if(returnCode < 0)
	{
		printf("#### VOI_Error2:fail to initialize the decoder###\n");
		return -1;
	}

	inData.Buffer = inBuf;
	bytesLeft = ReadFile2Buf(infile,inData.Buffer,READ_SIZE);//fread(inParam.inputStream, 1, READ_SIZE, infile);

//#######################################    Decoding Section   #########################################
	
	do {

		frameCount++;
		inData.Length    = bytesLeft;
		outData.Buffer   = outBuf;
		outData.Length = 1024*8;

#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		t1 = clock();
#endif
		
		/* decode one AAC frame *///
		returnCode = AudioAPI.SetInputData(hCodec,&inData);

		
		do {
			outData.Buffer   = outBuf;
			outData.Length = 1024*8;

			returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outInfo);

			if(returnCode == 0)
				decodedFrame++;

#if OUTPUT
			if (Isoutput && returnCode == 0)
			{
				fwrite(outData.Buffer, 1, outData.Length, outfile);
				//printf(outfile, "%d\n", decodedFrame);
			}
#endif
		} while(returnCode != (VO_ERR_INPUT_BUFFER_SMALL));

#ifdef _WIN32_WCE
		t2 = GetTickCount();
		total += t2 - t1;
#else
		t2 = clock();
		total += t2 - t1;
#endif


#define  MAX_REMAINED 2048
		if (!eofFile) {
			nRead = ReadFile2Buf(infile, inBuf,READ_SIZE);
			bytesLeft = nRead;
			inData.Buffer = inBuf;
			if (feof(infile))
				eofFile = 1;
		}

	} while (!eofFile && returnCode);

#ifdef _WIN32_WCE
	wsprintf(msg, TEXT("Decode Time: %d clocks, Decode Frames: %d"), total, decodedFrame);
	MessageBox(NULL, msg, TEXT("DRA Decode Finished"), MB_OK);
#endif	

//################################################  End Decoding Section  #######################################################
	returnCode = AudioAPI.Uninit(hCodec);
	
	fclose(infile);
	if (outfile)
    {
        fclose(outfile);
    }

	return 0;
}



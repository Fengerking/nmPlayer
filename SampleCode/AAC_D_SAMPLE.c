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
#include		"voAAC.h"
#include		"cmnMemory.h"

#define READ_SIZE	(1024*4)	
unsigned char outBuf[1024*16];
unsigned char inBuf[READ_SIZE];

void Usage(void)
{
	printf("\nvoi_aac_decoder Usage:\n");
	printf(">> voi_aac_decoder infile.aac outfile.pcm\n");
       
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

LPCSTR g_parfile = "/AACDecapp.par";
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

		fscanf(fconfig,"\"%[^\"]",outFile);               // RAW (YUV/RGB) output file
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",bwrite);           
		fscanf(fconfig,"%*[^\n]");	
	}
}
#endif

#ifdef _WIN32_WCE
//int WinMain()
int _tmain(int argc, TCHAR **argv) 
{
#elif defined _IOS
int enterTest(int argc, char **argv) 
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
	VO_AUDIO_FORMAT outFormat;
    int firstWrite = 1;
	int eofFile = 0;
	int* info=(int*)inBuf;
	int bytesLeft, nRead;
	int frameType;
	int sampleRate;
	int selectChs;
	int objectType;
	int decodedFrame = 0;
	unsigned char *readPtr;
	int tmp;
	int makeError = 0;
	int IsNewFormat = 0;		
	int IsBSAC = 0;
	int total = 0;
	int Isoutput = 1;
	int chanMode = 0;
	int	returnCode;
#define  OUTPUT	  1
	
#ifdef _WIN32_WCE
	TCHAR msg[256];
	char infileName[MAX_PATH];
	char outfileName[MAX_PATH];

	if(!(fconfig = fopen(g_parfile, "rb")))
	{
		wsprintf(msg, TEXT("open AAC par file error!!"));
		MessageBox(NULL, msg, TEXT("AAC Decode error"), MB_OK);
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
		wsprintf(msg, TEXT("open aac file error!!"));
		MessageBox(NULL, msg, TEXT("AAC Decode error"), MB_OK);
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
			MessageBox(NULL, msg, TEXT("AAC Decode error"), MB_OK);
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

	returnCode  = voGetAACDecAPI(&AudioAPI);
	if(returnCode)
		return -1;


//#######################################   Init Decoding Section   #########################################
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingAAC, &useData);
	if(returnCode < 0)
	{
		printf("#### VOI_Error2:fail to initialize the decoder###\n");
		return -1;
	}

	if(IsBSAC)
	{
		frameType = VOAAC_RAWDATA;//
		objectType = VOAAC_ER_BSAC;
		IsNewFormat = 1;
		objectType = VOAAC_ER_BSAC;

	}
	else
	{
		if(!IsNewFormat)
			frameType = VOAAC_ADTS;//
		else
			frameType = VOAAC_RAWDATA;//

		objectType = VOAAC_AAC_LC;
	}

	returnCode = AudioAPI.SetParam(hCodec,VO_PID_AAC_PROFILE,&objectType);
	returnCode = AudioAPI.SetParam(hCodec,VO_PID_AAC_FRAMETYPE,&frameType);

	tmp = 0;
	returnCode = AudioAPI.SetParam(hCodec,VO_PID_AAC_DISABLEAACPLUSV1,&tmp);

	tmp = 1;
	returnCode = AudioAPI.SetParam(hCodec,VO_PID_AAC_DISABLEAACPLUSV2,&tmp);

	//selectChs  = VO_CHANNEL_FRONT_LEFT | VO_CHANNEL_FRONT_RIGHT;
	//returnCode = AudioAPI.SetParam(hCodec,VO_PID_AAC_SELECTCHS,  &selectChs);

	//tmp = VO_AUDIO_CHAN_MULDOWNMIX2;
	//returnCode = AudioAPI.SetParam(hCodec,VO_PID_AAC_CHANNELSPEC, &tmp);

	bytesLeft = ReadFile2Buf(infile,inBuf, READ_SIZE);//fread(inParam.inputStream, 1, READ_SIZE, infile);
	inData.Buffer = inBuf;

	if(IsNewFormat)
	{
		//set inParam->channelNum and inParam->sampleRate
		{
			info=(int*)inBuf;
			returnCode = AudioAPI.SetParam(hCodec,VO_PID_AUDIO_CHANNELS,info);
			returnCode = AudioAPI.SetParam(hCodec,VO_PID_AUDIO_SAMPLEREATE,info+1);
			info+=2;
			bytesLeft-=8;
			inData.Buffer = inBuf + 8;
		}
	}

//#######################################    Decoding Section   #########################################
	
	do {

		frameCount++;
		if(IsNewFormat)
		{
			readPtr = inData.Buffer;
			inData.Length   = readPtr[0] + (readPtr[1] << 8) + (readPtr[2] << 16) + (readPtr[3] << 24);
			inData.Buffer   += 4;
		}
		else
			inData.Length    = bytesLeft;

		outData.Buffer   = outBuf;
		outData.Length = 1024*16;

		if(makeError&&(frameCount&3)==0)
		{
			memset(inData.Buffer+3,0,12);
		}

#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		t1 = clock();
#endif
		
		/* decode one AAC frame *///
		returnCode = AudioAPI.SetInputData(hCodec,&inData);

		
		do {
			outData.Buffer   = outBuf;
			outData.Length = 1024*16;	
			
			returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outInfo);

			if(returnCode == 0)
				decodedFrame++;

			AudioAPI.GetParam(hCodec,VO_PID_AUDIO_CHANNELMODE, &chanMode);
			if(chanMode == VO_AUDIO_CHANNEL_DUALMONO)
			{
				int chcon = VO_AUDIO_CHAN_DUALLEFT;
				AudioAPI.SetParam(hCodec,VO_PID_AUDIO_CHANNELCONFIG,  &chcon);
			}

#if OUTPUT
			if (Isoutput && returnCode == 0)
			{
				fwrite(outData.Buffer, 1, outData.Length, outfile);
				//printf(outfile, "%d\n", decodedFrame);
			}
#endif

		} while(returnCode != (VO_ERR_INPUT_BUFFER_SMALL) && frameType!=VOAAC_RAWDATA);

#ifdef _WIN32_WCE
		t2 = GetTickCount();
		total += t2 - t1;
#else
		t2 = clock();
		total += t2 - t1;
#endif


#define  MAX_REMAINED 2048
		
		if(IsNewFormat)
		{
			bytesLeft -= inData.Length+4;
			inData.Buffer += inData.Length;
			if (bytesLeft < MAX_REMAINED&&!eofFile) {
				memmove(inBuf, inData.Buffer, bytesLeft);
				nRead = ReadFile2Buf(infile,inBuf + bytesLeft,READ_SIZE - bytesLeft);
				bytesLeft += nRead;
				inData.Buffer = inBuf;
				
				if (feof(infile))
					eofFile = 1;
			}
		}
		else
		{
			if (!eofFile) {
				nRead = ReadFile2Buf(infile, inBuf,READ_SIZE);
				bytesLeft = nRead;
				inData.Buffer = inBuf;
				if (feof(infile))
					eofFile = 1;
			}
		}		
	} while (!eofFile);

#ifdef _WIN32_WCE
	wsprintf(msg, TEXT("Decode Time: %d clocks, Decode Frames: %d"), total, decodedFrame);
	MessageBox(NULL, msg, TEXT("AAC Decode Finished"), MB_OK);
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



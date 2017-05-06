#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "voADPCM.h"
#include "cmnMemory.h"

#define	MAKE_MARKER(a,b,c,d)	(((a) << 24) | ((b) << 16) | ((c) << 8) | (d))
#define RIFF_MARKER	 (MAKE_MARKER ('R', 'I', 'F', 'F'))
#define RIFX_MARKER	 (MAKE_MARKER ('R', 'I', 'F', 'X'))
#define WAVE_MARKER	 (MAKE_MARKER ('W', 'A', 'V', 'E'))
#define fmt_MARKER	 (MAKE_MARKER ('f', 'm', 't', ' '))
#define data_MARKER	 (MAKE_MARKER ('d', 'a', 't', 'a'))
#define fact_MARKER	 (MAKE_MARKER ('f', 'a', 'c', 't'))
#define PEAK_MARKER	 (MAKE_MARKER ('P', 'E', 'A', 'K'))

#ifdef _WIN32_WCE

#define MAX_PATH          260
#define MAX_CMDLINE (MAX_PATH*2+16)
#define MAX_ARGV      10

LPCSTR g_parfile = "/MP3Decapp.par";
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

#define INPUT_LEN  1*1024     //80*1024  small 1K than work buffer
#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else
int main(int argc, char *argv[])
#endif
{
    FILE *input_file = NULL;   //input file
	FILE *output_file = NULL;  //output file
	long dectime = 0,dectotal = 0; 
    int file_size = 0, length, decodedFrame;
	unsigned char input_buf[INPUT_LEN];
    unsigned char output_buf[4608*4];   //4608 samples number,2 channels,2 Bytes for one sample	
	unsigned long FrameCount = 0;
	unsigned int  Marker;
	int	returnCode;
 	
 	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outInfo;

	int read_size = 0,left_size = 0;
	int Isoutput = 1;
        
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

	if(!(input_file = fopen(infileName, "rb"))){
		printf("open source file error!");
		return 0;
	}

	/*open out put file*/
	if(!(output_file = fopen(outfileName, "w+b"))){
		printf("Open output file error!");
		return 0;
	}

#else
	if(argc <= 1)
		printf("please input decoder name\n");
	/*open input file*/
	if(!(input_file = fopen(argv[1], "rb"))){
		printf("open source file error!");
		return 0;
	}

	/*open out put file*/
	if(!(output_file = fopen(argv[2], "w+b"))){
		printf("Open output file error!");
		return 0;
	}
#endif 
	fseek(input_file,0,SEEK_END);
	file_size = ftell(input_file);   //get the size of file
	fseek(input_file,0,SEEK_SET);    //put the file ptr to begin
	memset(output_buf,0,4608*4);

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);

	returnCode  = voGetADPCMDecAPI(&AudioAPI);
	if(returnCode)
		return -1;

	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingADPCM, &useData);
	if(returnCode < 0)
	{
		printf("#### VOI_Error2:fail to initialize the decoder###\n");
		return -1;
	}

    read_size = fread(input_buf, 1, 12, input_file);
	file_size -=read_size;
	
	while(1) {		
		read_size = fread(input_buf, 1, 8, input_file);
		file_size -=read_size;
		length = (input_buf[7] << 24) | (input_buf[6] << 16) | (input_buf[5] << 8) | (input_buf[4] );
		Marker = MAKE_MARKER(input_buf[0], input_buf[1], input_buf[2], input_buf[3]);
		if(Marker == fmt_MARKER)
		{
			read_size = fread(input_buf, 1, length, input_file);  //get the input data from input file,stroe input_buf
			file_size -=read_size;

			inData.Buffer = input_buf;
			inData.Length = length;
   
			AudioAPI.SetParam(hCodec, VO_PID_COMMON_HEADDATA, &inData);
		}
		else if(Marker == data_MARKER)
		{
			break;
		}
		else
		{
			fseek(input_file, length, SEEK_CUR);
			file_size -=length;
		}
	}	

	read_size = fread(input_buf, 1, INPUT_LEN, input_file);  //get the input data from input file,stroe input_buf
    length -=read_size;

	inData.Buffer = input_buf;
	inData.Length = read_size;
	decodedFrame = 0;
     	
	do{
#ifdef  _WIN32_WCE
		dectime = GetTickCount();
#else
		dectime = clock();
#endif
		returnCode = AudioAPI.SetInputData(hCodec,&inData);

		do {
			outData.Buffer = (unsigned char *)output_buf;
			outData.Length = 4608*4;

			returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outInfo);

			if(returnCode == 0)
				decodedFrame++;

			if (Isoutput && returnCode == 0)
			{
				fwrite(outData.Buffer, 1, outData.Length, output_file);
			}
		} while(returnCode != (VO_ERR_INPUT_BUFFER_SMALL));

#ifdef _WIN32_WCE
		dectotal +=(GetTickCount() - dectime);
#else
		dectotal +=(clock() - dectime);
#endif
		read_size = fread(input_buf, 1, INPUT_LEN, input_file);  //one time give 1k data
		if(!read_size)
			break;
		length -= read_size;
		inData.Buffer = input_buf;
		inData.Length = read_size;
	}while(length>0);

	/* release the decoder */
	returnCode = AudioAPI.Uninit(hCodec);     
	
#ifdef _WIN32_WCE
	wsprintf(msg, TEXT("decode Time: %d clocks , Encode frames: %d"), dectotal,FrameCount);
	MessageBox(NULL, msg, TEXT("FLAC Decode Finished"), MB_OK);
#else
	printf("Finish decoding in %d ms", (int)dectotal);
#endif

	fclose(input_file);
	fclose(output_file);
	return 0;
}


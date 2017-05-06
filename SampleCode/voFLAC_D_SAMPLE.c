#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "voFLAC.h"
#include "cmnMemory.h"

#define INPUT_LEN  1*1024     //80*1024  small 1K than work buffer

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#elif _IOS
int beginTest_flac(char *inFile, char *outFile, int times)
#else
int main(int argc, char *argv[])
#endif
{
	FILE    *input_file = NULL;                      //input file
	FILE    *output_file = NULL;                     //output file
	long    dectime = 0;
	long    dectotal = 0; 
	int     framecount = 0;
	int     file_size = 0;
	int     length;
    int     returnCode;
	int     read_size = 0;
	int     left_size = 0;
	int     nEnable = 0;
	unsigned char input_buf[INPUT_LEN];
	unsigned char output_buf[4608*4];             //4608 samples number,2 channels,2 Bytes for one sample	
	unsigned long FrameCount = 0;

	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR   moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;	

#ifdef _WIN32_WCE
	TCHAR msg[256];
	/*open input file*/
	if(!(input_file = fopen("/ResidentFlash/t1.flac", "rb"))){
		printf("open source file error!");
		return 0;
	}
	/*open out put file*/
	if(!(output_file = fopen("/ResidentFlash/t1.flac.pcm", "w+b"))){
		printf("Open output file error!");
		return 0;
	}	
#elif _IOS
	input_file = fopen(inFile, "rb");
	output_file = fopen(outFile, "w+b");
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
	file_size = ftell(input_file);                           //get the size of file
	fseek(input_file,0,SEEK_SET);                            //put the file ptr to begin

	memset(output_buf,0,4608*4);
	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;
	moper.Move = cmnMemMove;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);	
	returnCode = voGetFLACDecAPI(&AudioAPI);

	if(returnCode)
	{
		goto END;
	}

	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingFLAC, &useData);;   //Init Flac decoder

	if(returnCode)
	{
		goto END;
	}

	fseek(input_file, 8, SEEK_SET);
	read_size = fread(input_buf, 1, 34, input_file);  //get the input data from input file,stroe input_buf
	file_size -=read_size;

	inData.Buffer = input_buf;
	inData.Length = 34;

	returnCode = AudioAPI.SetParam(hCodec,VO_PID_FLAC_HEADER_PARAMETER, (void *)&inData);

	//Enable 24bits
	//nEnable = 1;
	//returnCode = AudioAPI.SetParam(hCodec,VO_PID_FLAC_GET24BITS, (void *)&nEnable);

	while(1) {		
		read_size = fread(input_buf, 1, 4, input_file);
		length = (input_buf[1] << 16) | (input_buf[2] << 8) | (input_buf[3] );
		file_size -= length;
		fseek(input_file, length, SEEK_CUR);
		if((input_buf[0]&0x80))
		{
			break;
		}
	}	

	read_size = fread(input_buf, 1, INPUT_LEN, input_file);  //get the input data from input file,stroe input_buf
	file_size -= read_size;

	do {
		inData.Buffer    = input_buf;
		inData.Length    = read_size;
		outData.Buffer   = (unsigned char *)output_buf;

#ifdef _WIN32_WCE
		dectime = GetTickCount();
#else
		dectime = clock();
#endif
		/* decode one flac block */
		returnCode = AudioAPI.SetInputData(hCodec, &inData);

		do {
			outData.Length = 4608*4;
			returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outFormat);

			if(returnCode == 0)
			{
				fwrite(outData.Buffer, 1, outData.Length, output_file);
				fflush(output_file);
				framecount++;
			}
		} while(returnCode != VO_ERR_INPUT_BUFFER_SMALL);

#ifdef _WIN32_WCE
		dectotal +=(GetTickCount() - dectime);
#else
		dectotal +=(clock() - dectime);
#endif
		read_size = fread((void *)input_buf, 1, INPUT_LEN, input_file);
		if(!read_size)
			break;
		file_size -= read_size;

	} while (file_size>0);

END:
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

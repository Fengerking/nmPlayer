// voDolbyEffectTest.cpp : Defines the entry point for the console application.
//
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#ifdef LINUX
#include <dlfcn.h>
#endif

#include "voEFTDolby.h"
#include "cmnMemory.h"

typedef int (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);

#define INPUT_LEN  4*1024 
#define OUTPUT_LEN  8*INPUT_LEN 

int main(int argc, char* argv[])
{
	FILE    *input_file = NULL;                      //input file
	FILE    *output_file = NULL;                     //output file
	int     framecount = 0;
	int     file_size = 0;
    int     returnCode;
	int     read_size = 0;
	int     left_size = 0;
	unsigned char input_buf[INPUT_LEN];
	unsigned char output_buf[OUTPUT_LEN];             //4608 samples number,2 channels,2 Bytes for one sample	
	unsigned long FrameCount = 0;
	int		 Effect = 0;

	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR   moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;
	VO_AUDIO_FORMAT audioFormat;

	EFFECT_DOLBY_PARAM DolbyParam;
	
	void *handle;
	void *pfunc;
	VOGETAUDIODECAPI pGetAPI;

	if(argc <= 1)
		printf("please input decoder name\n");
	/*open input file*/
	if(!(input_file = fopen((char *)argv[1], "rb"))){
		printf("open source file error!");
		return 0;
	}
	/*open out put file*/
	if(!(output_file = fopen((char *)argv[2], "wb"))){
		printf("Open output file error!");
		return 0;
	}

	fseek(input_file,0,SEEK_END);
	file_size = ftell(input_file);                           //get the size of file
	fseek(input_file,0,SEEK_SET);                            //put the file ptr to begin

	memset(output_buf,0,OUTPUT_LEN);
	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;
	moper.Move = cmnMemMove;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);	


#ifdef LINUX
	handle = dlopen("/data/local/tmp/libvoDolbyEffect.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}
	
	pfunc = dlsym(handle, "voGetDolbyEffectAPI");	
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
	returnCode  = voGetDolbyEffectAPI(&AudioAPI);
	if(returnCode)
		return -1;
#endif


	if(returnCode)
	{
		goto END;
	}

	returnCode = AudioAPI.Init(&hCodec, (VO_AUDIO_CODINGTYPE)VO_AUDIO_DolbyMoblie, &useData);;   

	if(returnCode)
	{
		goto END;
	}

	audioFormat.Channels = 1;
	audioFormat.SampleRate = 16000;
	audioFormat.SampleBits = 16;

	returnCode = AudioAPI.SetParam(hCodec,VO_PID_AUDIO_FORMAT, (void *)&audioFormat);

/////////////////////////
///Common way to set the parameter

	//returnCode = AudioAPI.GetParam(hCodec, VO_PID_AUDIO_EFFECTCONFIG, (void *)&DolbyParam);

	//DolbyParam.HFEEnable = 1;
	//DolbyParam.M2SEnable = 1;
	//DolbyParam.UPSampleEnable = 1;
	//DolbyParam.GEQEnable = 1;
	//DolbyParam.NBEnable = 1;
	//DolbyParam.GEQEnable = 1;
	//DolbyParam.MSREnable = 1;

	//returnCode = AudioAPI.SetParam(hCodec, VO_PID_AUDIO_EFFECTCONFIG, (void *)&DolbyParam);

////////////////////////
////////simple way to set the parameter to the default value for enable or disable
	Effect = 1;
	returnCode = AudioAPI.SetParam(hCodec, VO_PID_EFFDOLBY_DEFAULT, (void *)&Effect);

///////////////////////////////////

	read_size = fread(input_buf, 1, INPUT_LEN, input_file);  //get the input data from input file,stroe input_buf
	file_size -= read_size;

	do {
		inData.Buffer    = input_buf;
		inData.Length    = read_size;
		outData.Buffer   = (unsigned char *)output_buf;

		/* decode one amr block */
		returnCode = AudioAPI.SetInputData(hCodec, &inData);

		do {
			outData.Length = OUTPUT_LEN;
			returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outFormat);

			if(returnCode == 0)
			{
				fwrite(outData.Buffer, 1, outData.Length, output_file);
				fflush(output_file);
				framecount++;
			}
		} while(returnCode != VO_ERR_INPUT_BUFFER_SMALL);

		read_size = fread((void *)input_buf, 1, INPUT_LEN, input_file);
		if(!read_size)
			break;
		file_size -= read_size;
	}while (file_size>=0);				//modified '>' to '>=' by Jonathon, 2013/09/12

END:
	/* release the decoder */
	returnCode = AudioAPI.Uninit(hCodec);       

	fclose(input_file);
	fclose(output_file);
	return 0;
}


#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include        "voHdEq.h"
#include        "cmnMemory.h"

#define       READ_SIZE	    (4096)	
short         outBuf[READ_SIZE];
unsigned char inBuf[READ_SIZE];


#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
int main(int argc, char **argv)        // for gcc compiler;
#endif//_WIN32_WCE
{
	char *infileName, *outfileName;
	FILE *infile, *outfile;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;

	int ret;
    int firstWrite = 1;
	int eofFile = 0, param;
	int frameCount =0;
	int bytesLeft, bytesUsed,nRead,idx=0;
	char wavhead[44];

#ifdef _WIN32_WCE
	infileName = "/Storage Card/test/AAC_TEST/aac_stereo.wav";
	outfileName = "/Storage Card/test/AAC_TEST/output/eq_stereo.wav";
#else// _WIN32_WCE
    infileName = argv[1];
    outfileName = argv[2];
#endif//_WIN32_WCE        
	/* open input file */
	infile = fopen(infileName, "rb");
	if (!infile) {
		printf(" #### VOI_Error0: can not open input file %s #### \n", infileName);
		return -1;
	}
		
	outfile = fopen(outfileName, "wb");
	if (!outfile) {
		printf(" #### VOI_Error1: can not open output file %s ###\n", outfileName);
		return -1;
	}

	printf("input file :%s\n",infileName);
	printf("output file :%s\n",outfileName);

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);

	ret = voGetHDEQAPI(&AudioAPI);
	if(ret)
	{
		ret = -1;
		goto END;
	}

	//#######################################   Init Encoding Section   #########################################
	ret = AudioAPI.Init(&hCodec, VO_AUDIO_CodingAC3, &useData);

	if(ret < 0)
	{
		ret = -1;
		goto END;
	}

    
	param = 2;
	ret = AudioAPI.SetParam(hCodec, VO_PID_HDEQ_CHANNELNUM, &param);
	if(ret < 0)
		goto END;

	param = sr_44100;
	ret = AudioAPI.SetParam(hCodec, VO_PID_HDEQ_SAMPLERATE, &param);
	if(ret < 0)
		goto END;
	
	param = JAZZ;
	ret = AudioAPI.SetParam(hCodec, VO_PID_HDEQ_MODE, &param);
	if(ret < 0)
		goto END;

	bytesLeft = fread(inBuf, 1, READ_SIZE, infile);
    if (bytesLeft != READ_SIZE)
    {
		goto END;
    }

	do 
	{
		inData.Buffer = inBuf;
		inData.Length = bytesLeft;

		ret = AudioAPI.SetInputData(hCodec, &inData);
		if (ret < 0)
		{
			goto END;
		}

		outData.Buffer = (unsigned char *)outBuf;
		ret = AudioAPI.GetOutputData(hCodec,&outData, &outFormat);

		if (ret < 0)
		{
			goto END;
		}
		else
		{
			fwrite(outData.Buffer, 1, outData.Length, outfile);
			fflush(outfile);
		}
		bytesLeft = fread(inBuf, 1, READ_SIZE, infile);
		if (bytesLeft != READ_SIZE)
		{
			eofFile = 1;
		}

	} while (!eofFile);

END:
	ret = AudioAPI.Uninit(hCodec);

	if(ret)
	{
		printf("#### VOI_Error4:fail to destroy the equalizer ###\n");
		return -1;
	}

	if (infile)
		fclose(infile);

	if (outfile)
		fclose(outfile);

	return 0;
}



#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		"voEqualizer.h"
#define READ_SIZE	(1024*16)	
short outBuf[READ_SIZE*2];
unsigned char inBuf[READ_SIZE];

int preSet[10][10]={
	{0,0,0,0,0,0,0,0,0,0},
	{-2,5,17,-5,-4,3,-9,12,14,11},
	{-15,-25,8,9,7,4,4,2,2,0},
	{11,12,13,15,10,9,-8,-7,-6,-18},
	{-25,-15,-14,-12,-7,-5,0,10,11,15},
	{0,-2,-3,-6,-9,-4,-8,8,6,4},
	{15,11,13,11,10,6,-5,-9,-9,-9},
	{18,12,1,2,0,-2,-9,12,13,14},
	{22,20,-18,11,15,0,-11,-9,-8,-5},
	{18,18,18,18,-18,-18,18,18,18,18},
};

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	char *infileName, *outfileName;
	FILE *infile, *outfile;
	HVOCODEC hCodec;
	VOCODECDATABUFFER InData; 
	VOCODECDATABUFFER OutData;
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

	ret = voEQInit(&hCodec);
	if(ret < 0)
		return -1;

	param = 1;
	ret = voEQSetParameter(hCodec, VO_EQ_ONOFF, &param);
	if(ret < 0)
		goto END;

	param = 2;
	ret = voEQSetParameter(hCodec, VO_CHANNELNUM, &param);
	if(ret < 0)
		goto END;

	param = sr_48000;
	ret = voEQSetParameter(hCodec, VO_SAMPLERATE, &param);
	if(ret < 0)
		goto END;
	
	param = POP;
	ret = voEQSetParameter(hCodec, VO_EQMODE, &param);
	if(ret < 0)
		goto END;

////////////////////////////////////////////////////////////////////////
//	param = OTHERS;
//	ret = voEQSetParameter(hCodec, VO_EQMODE, &param);
//	if(ret < 0)
//		goto END;
//	
//	param = 5; //0, 1, 2,.......
//	ret = voEQSetParameter(hCodec, VO_EA_EQUAT_COEF, preSet[param]);
//	if(ret < 0)
//		goto END;
///////////////////////////////////////////////////////////////////////
	bytesLeft = fread(inBuf, 1, READ_SIZE, infile);
	memcpy(wavhead,inBuf,sizeof(wavhead));
	bytesUsed = sizeof(wavhead);
	bytesLeft -=bytesUsed;
//#######################################    Decoding Section   #########################################

	do {
		if (bytesLeft < 8192) {//ASSUME,the smallest frame size is greater than 512 bytes
			
			memmove(inBuf, inBuf+bytesUsed, bytesLeft);
			nRead = fread(inBuf + bytesLeft, 1, bytesUsed, infile);
			bytesLeft += nRead;
			bytesUsed = 0;
			if (feof(infile)&&bytesLeft<8192)
				eofFile = 1;
		}
		InData.buffer = inBuf+bytesUsed;
		InData.length = bytesLeft;
		
		OutData.buffer = (char *)outBuf;
		OutData.length = bytesLeft;
		
		ret = voEQProcess(hCodec, &InData, &OutData);
		if(ret)
		{
			printf("#### VOI_Error3:fail to equalize the frame %d###\n",frameCount);
			//return -1;
		}
		//after decoding one frame,the inputSize is set as the actual consumed bytes by the decoder
		bytesLeft -= InData.length;
		bytesUsed += InData.length;
		frameCount++;
	
		if (firstWrite&&outfile)
		{
			
			// write_wav_header(outfile, &inParam);
			 fwrite(wavhead, 1, sizeof(wavhead), outfile); 
			 firstWrite = 0;
		}
		
#if 1
		if (outfile)
		{
			 fwrite(OutData.buffer, 1, OutData.length, outfile); 
		}
#endif			
		
	} while (!eofFile);	
	
//################################################  End Decoding Section  #######################################################
END:
	ret = voEQUnInit(hCodec);
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



/*
 *  Copyright 2008 by Visualon software Incorporated.
 *  All rights reserved. Property of Visualon software Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */

/***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver         Description             Author
;*    -----------     --------     ------------------        -----------
;*    11-25-2008        1.0        File imported from        Huaping Liu
;*                                             
;**********************************************************************/
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#ifdef _WIN32_WCE
#include 	<windows.h>
#include 	<objbase.h>
#include 	<Winbase.h>
#endif // _WIN32_WCE
#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>
#include	<limits.h>
#include	"g723enc.h"
typedef unsigned char UWord8;
typedef signed char Word8;
typedef unsigned short UWord16;
typedef short Word16;
typedef unsigned int UWord32;
typedef int Word32;
#define L_FRAME 240
#define MAX_SERIAL_SIZE 189 
#define MAX_PACKED_SIZE 32 //(MAX_SERIAL_SIZE / 8 + 6)


/* frame size in serial bitstream file (frame type + serial stream + flags) */
#define SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)

Word32 frame = 0;

#define  INPUT_SIZE   1024
#define  OUTPUT_SIZE  256
UWord8  InputBuf[INPUT_SIZE ];
UWord8  OutputBuf[OUTPUT_SIZE ];
int	usedSize=0;
int    encodedSize=0;
int  GetNextBuf(FILE* inFile,UWord8* dst,int size)
{
	int size2 = fread(dst, sizeof(Word8), size,inFile);
	return size2;
}
int encode(int mode, const char* srcfile, const char* dstfile)
{

	int			ret = 0;
	void			*hEnc = NULL;
	FILE			*fsrc = NULL;
	FILE			*fdst = NULL;
	int			endEncode=0;
	HVOCODEC		hCodec;
	VOG723ENCRETURNCODE	returnCode;
	VOCODECDATABUFFER inData;
	VOCODECDATABUFFER outData;
	VOCODECAUDIOFORMAT outFormat;
	UWord8 *inBuf = InputBuf;
	UWord8 *outBuf = OutputBuf;
	int remainedSize,size1,size2=0;
	int lastTime=0;
	VOG723FRAMETYPE frameType = VOG723_DEFAULT;
	Word16 new_speech[L_FRAME];         /* Pointer to new speech data        */
	UWord8 packed_bits[MAX_PACKED_SIZE];
	int packed_size;
    Word16 allow_dtx = 1;

#ifdef _WIN32_WCE
	TCHAR msg[256];
	int t1, t2 = 0;	
#else
	clock_t   start, finish;
	double    duration = 0.0;
#endif
	if ((fsrc = fopen (srcfile, "rb")) == NULL)
	{
		ret = 1;
		goto safe_exit;
	}

	if ((fdst = fopen (dstfile, "wb")) == NULL)
	{
		ret = 2;
		goto safe_exit;
	}

	//#######################################   Init Encoding Section   #########################################
	returnCode = voG723EncInit(&hCodec);

	if(!VORC_SUCCEEDED(returnCode))
	{
		//printf("#### VOI_Error2:fail to initialize the decoder###\n");
		ret = 3;
		goto safe_exit;
	}

	size1 = GetNextBuf(fsrc,InputBuf,INPUT_SIZE);

	if(size1!=INPUT_SIZE && !feof(fsrc))
	{
		ret = 5; //Invalid magic number
		goto safe_exit;
	}

	/* Process speech frame by frame */
	frame = 0;
	//set parameter
	voG723EncSetParameter(hCodec,VOID_G723_FRAMETYPE,frameType);
	voG723EncSetParameter(hCodec,VOID_G723_MODE,mode);
    voG723EncSetParameter(hCodec,VOID_G723_DTX, allow_dtx);

	//#######################################   Encoding Section   #########################################
	while (!endEncode || lastTime)//fread (new_speech, sizeof (Word16), L_FRAME, fsrc) == L_FRAME)
	{
		frame++;
		size1 = L_FRAME<<1;

		memcpy(new_speech, inBuf, size1);

		inBuf += size1;
		usedSize += size1; 

		inData.buffer	    = (BYTE*)new_speech;
		inData.length	    = L_FRAME<<1;
		outData.buffer      = packed_bits;
		/* encode one amr frame */
#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		start = clock();
#endif
		returnCode = voG723EncProcess(hCodec, &inData, &outData, NULL);

#ifdef _WIN32_WCE
		t2 += (GetTickCount() - t1);
#else
		finish = clock();
		duration += (double)(finish - start);
#endif
		packed_size = outData.length;
		size1 = packed_size;
		memcpy(outBuf,packed_bits,size1);
		outBuf += size1;
		encodedSize += size1; 

		if(encodedSize + 32 > OUTPUT_SIZE || (lastTime&&usedSize==size2))
		{
			//fwrite(OutputBuf, 1,encodedSize, fdst);	
			//fflush(fdst);
			encodedSize = 0;
			outBuf = OutputBuf;
		}
		if(lastTime&&usedSize==size2)//end
			break;
		if((remainedSize = INPUT_SIZE - usedSize)<240)
		{
			memcpy(InputBuf,inBuf,remainedSize);
			size2=GetNextBuf(fsrc,InputBuf+remainedSize,INPUT_SIZE-remainedSize);
			inBuf = InputBuf;
			usedSize = 0;
			if(size2!=INPUT_SIZE-remainedSize)
			{
				lastTime = 1;
				size2 += remainedSize;
				size2 -=size2%(L_FRAME<<1);
				endEncode = 1;
			}
		}		
	}

	//#######################################   End Encoding Section   #########################################

safe_exit:
	returnCode = voG723EncUninit(hCodec);

#ifdef _WIN32_WCE
	wsprintf(msg, TEXT("Encode Time: %d clocks, Encode frames: %d"), t2, frame);
	MessageBox(NULL, msg, TEXT("AMR Encode Finished"), MB_OK);
#else
	printf( "\n%2.5f seconds\n", (double)duration/1000);
#endif
    //fclose(log_file);
	if (fsrc)
		fclose(fsrc);
	if (fdst)
		fclose(fdst);
	return ret;
}

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
int main(int argc, char **argv)  // for gcc compiler;
#endif//_WIN32_WCE
{
	int   mode, r;
	char   *inFileName,*outFileName;

#ifdef _WIN32_WCE
	mode = VOG723_MD63;
	inFileName  = "/Storage Card/G723/DTX63.TIN";
	outFileName = "/Storage Card/G723/DTX53MIX1.out";
#else
	if (argc != 3)
	{
		fprintf(stderr, "\nUsage: %s mode srcfile.pcm dstfile.amr\n", argv[0]);
		return -1;
	}
	mode = VOG723_MD53;
	inFileName  = argv[1];
	outFileName = argv[2];
#endif
	r = encode(mode, inFileName, outFileName);
	if(r)
	{
		fprintf(stderr, "error: %d\n", r);
	}
	return r;
}


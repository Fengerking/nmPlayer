/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2006				*			
*																		*									
************************************************************************/
#ifdef _WIN32_WCE
#include 	<windows.h>
#include 	<objbase.h>
#include 	<Winbase.h>
#endif // _WIN32_WCE
#include 	<windows.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>
#include	<limits.h>
#include	"g723dec.h" 

typedef unsigned char UWord8;
typedef signed char Word8;
typedef unsigned short UWord16;
typedef short Word16;
typedef unsigned int UWord32;
typedef int Word32;
#define  L_FRAME 240
#define  INPUT_SIZE   1024
UWord8  InputBuf[INPUT_SIZE];
//UWord8  OutputBuf[INPUT_SIZE ];
int	usedSize=0;
int    decodedSize=0;

int  GetNextBuf(FILE* inFile,UWord8* dst,int size)
{
	int size2 = fread(dst, sizeof(Word8), size,inFile);
	return size2;
}

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	DWORD t1, t2;
	char *infileName, *outfileName;
	int  size1,size2,ret = 0;
	char* inBuf = InputBuf;
	int endDecode = 0;
	int lastTime = 0;
	int remainedSize;
	HVOCODEC hCodec = NULL;
	VOG723DECRETURNCODE returnCode;
	VOCODECDATABUFFER inData={0};
	VOCODECDATABUFFER outData={0};
	FILE *fsrc = NULL;
	FILE *fdst = NULL;
	Word16 synth[L_FRAME];             /* Synthesis*/
	Word32 frame = 0;
	int first = 1;
	//extern FILE *log_file;

#ifdef _WIN32_WCE
	DWORD total = 0;
	TCHAR msg[256];

	infileName = "/Storage Card/G723/DTX63_ref.RCO";//347548.amr";//f099-122.amr";//TODO,Please set the correct path and file name
	outfileName = "/Storage Card/G723/DTX63_dec1.out";//TODO,Please set the correct path and file name
#else//_WIN32_WCE
	if (argc != 3)
	{
		fprintf (stderr, " Usage:\n\n   %s  src.amr dst.pcm\n\n", argv[0]);
		exit (1);
	}
	infileName = argv[1];
	outfileName = argv[2];
#endif//_WIN32_WCE

	if ((fsrc = fopen (infileName, "rb")) == NULL)
	{
		ret = 1;
		goto safe_exit;
	}

	if ((fdst = fopen (outfileName, "wb")) == NULL)
	{
		ret = 2;
		goto safe_exit;
	}

	remainedSize = size1 = GetNextBuf(fsrc,InputBuf,INPUT_SIZE);
	if(size1<INPUT_SIZE)
	{
		lastTime = 1;
		endDecode = 1;
		size2 = size1;
	}

	if(size1!=INPUT_SIZE)
	{
		ret = 5; //Invalid magic number
		//goto safe_exit;
	}

	returnCode = voG723DecInit(&hCodec);
	if(!VORC_SUCCEEDED(returnCode))
	{
		printf("#### VOI_Error2:fail to initialize the decoder###\n");
		ret = 3;
		goto safe_exit;
	}

	while (!endDecode || lastTime)
	{

		frame++;

		inData.buffer	= inBuf;
		inData.length   = remainedSize;
		outData.buffer 	= synth;
		outData.length  = L_FRAME*2;
		/* decode one amr frame */
#ifdef _WIN32_WCE
		t1 = GetTickCount();
#endif

		returnCode = voG723DecProcess(hCodec, &inData, &outData, NULL);

#ifdef _WIN32_WCE
		t2 = GetTickCount();
		total += (t2 -t1);
#endif
		if(!VORC_SUCCEEDED(returnCode))
		{
			goto safe_exit;
		}


#ifdef _WIN32_WCE
		//if(outData.buffer!=NULL)
		//	fwrite(outData.buffer, 1,outData.length,fdst);	
#else
		if(outData.buffer!=NULL)
			fwrite(outData.buffer, 1,outData.length,fdst);	
#endif

		if(lastTime&&usedSize>=size2)//end
			break;
		usedSize += inData.length;
		inBuf += inData.length;
		remainedSize -= inData.length;

		if(remainedSize <= 30)
		{
			memcpy(InputBuf,inBuf,remainedSize);
			size2=GetNextBuf(fsrc,InputBuf+remainedSize,INPUT_SIZE-remainedSize);
			inBuf = InputBuf;
			usedSize = 0;
			if(size2 != INPUT_SIZE-remainedSize)
			{
				//if(endDecode)
				lastTime = 1;				
				endDecode = 1;
			}

			size2 += remainedSize;
			remainedSize = size2;
		}

		//if(frame>100) break;

	}

safe_exit:
	returnCode = voG723DecUninit(hCodec);
#ifdef _WIN32_WCE
	wsprintf(msg, TEXT("Decode Time: %d clocks, Decode Frames: %d, fps: %d"), total, frame, frame*1000/total);
	MessageBox(NULL, msg, TEXT("G723 Decoder Finished"), MB_OK);
#endif
	if(!VORC_SUCCEEDED(returnCode))
	{
		printf("#### VOI_Error4:fail to destroy the decoder ###\n");
	}

	if (fsrc)
		fclose(fsrc);
	if (fdst)
		fclose(fdst);

	return ret;
}

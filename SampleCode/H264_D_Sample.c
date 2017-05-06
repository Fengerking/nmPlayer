 
#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<sys/time.h>
#include		<limits.h>
#include		"voH264New.h"

#ifdef _MAC_OS
#include <dlfcn.h>
#endif
#ifdef _IOS
#import <Foundation/Foundation.h>
#endif

//#define DUMP_YUV

FILE* outFile=NULL;
int frameNumber=0;

static long int GetSysTime()
{
	long int nTime;
	struct timeval tval;
	gettimeofday(&tval, NULL);
	VO_U64 second = tval.tv_sec;
	second = second*1000 + tval.tv_usec/1000;
	nTime = second & 0x000000007FFFFFFF;
	return nTime;
}

int VO_API OutputOneFrame(VO_VIDEO_BUFFER *par,int width,int height,int frametype)
{
	int i;
	unsigned char* buf;
	frameNumber++;
	if (!outFile)
		return 0;

	buf  = par->Buffer[0];
	for(i=0;i<height;i++,buf+=par->Stride[0])
	{
		fwrite(buf, 1, width, outFile);
	}	
	width	/=2;
	height	/=2;
	buf	=  par->Buffer[1];
	for(i=0;i<height;i++,buf+=par->Stride[1])
	{
		fwrite(buf, 1, width, outFile); 
	}	
	
	buf	=  par->Buffer[2];
	for(i=0;i<height;i++,buf+=par->Stride[2])
	{
		fwrite(buf, 1, width, outFile); 
	}	
	fflush(outFile);
	return 0;
}

#define MAX_SINGLE_INPUT  (200*1024)

#define INBUFFER_SIZE (1024*1000)

#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))


static char* GetNextFrame(char* currPos,int size)
{

	char* p = currPos+3;  
	char* endPos = currPos+size-8;
	for (; p < endPos; p++)
	{
		if (XRAW_IS_ANNEXB(p)||XRAW_IS_ANNEXB2(p))
		{
				break;			
		}
	}
	if(p>=endPos)
		p = NULL;

	return p;
}

FILE *inFile, *speedFile;

#ifdef _IOS
int enterTest(int argc, char **argv) 
#else
int main(int argc, char **argv) 
#endif
{
	char *inputBuf;
	VO_HANDLE hCodec;
	int		returnCode;
	VO_CODECBUFFER inData;
	VO_VIDEO_BUFFER  outData;
	VO_VIDEO_OUTPUTINFO outFormat;
	VO_VIDEO_DECAPI decApi={0};
	char inFileName[256], outFileName[256], outSpeedName[256];
	double duration;
	int outTotalFrame = INT_MAX;
	int format = VO_H264_ANNEXB;
	int loopCount=1;
	int inSize = 0;
	int inFrame = 0;
	int	endOfFile = 0;
	int frameNum3[3]={0};
	int frameType;
	char* nextFrame,*currFrame;
	int leftSize=INBUFFER_SIZE;//maks no sense, just for consistent with !hasSizeHead case;
	int size;
	long param;
	
	int i, dataFrame;
	clock_t start, finish;
#ifdef _MAC_OS
	typedef VO_S32 (VO_API * VOGETVIDEODECAPI) (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);
	
	char libname[256];
	strcpy(libname, "./voH264Dec.dylib");
	void *m_hDll = dlopen (libname, RTLD_NOW);
	
	if (NULL == m_hDll) {
		return 0;
	}
	
	VOGETVIDEODECAPI pApi = (VOGETVIDEODECAPI) dlsym (m_hDll, ("voGetH264DecAPI"));
	
	if (NULL == pApi) {
		return 0;
	}
	pApi(&decApi,0);
#else
	voGetH264DecAPI(&decApi,0);
#endif
	
#ifdef WIN32
	strcpy(inFileName,"d:/BP_CIF.264");
	strcpy(outSpeedName,"d:/speed.txt");
	strcpy(outFileName,"d:/testOut.yuv");
#elif defined _MAC_OS
	strcpy(inFileName,"./test.264");
	strcpy(outSpeedName,"./speed.txt");
	strcpy(outFileName,"./testOut.yuv");
#elif defined _IOS
	NSString* documentsDir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0]; 
	strcpy(inFileName, [[NSString stringWithFormat:@"%@/test.264", documentsDir] UTF8String]);
	strcpy(outSpeedName, [[NSString stringWithFormat:@"%@/speed.txt", documentsDir] UTF8String]);
	strcpy(outFileName, [[NSString stringWithFormat:@"%@/testOut.yuv", documentsDir] UTF8String]);
#else//WIN32
	//TODO: assign the inFileName,outFileName,outSpeedName
	strcpy(inFileName,"/test.264");
	strcpy(outSpeedName,"/speed.txt");
	strcpy(outFileName,"/testOut.yuv");
#endif//WIN32
	if (outSpeedName[0] != '\0')
		speedFile = fopen(outSpeedName, "wb");
	else
		speedFile = NULL;

	if (!speedFile){
		printf("\nWarning: no output speed file!");
	}


	endOfFile = 0;
    printf("inFileName,%s",inFileName);
	inFile= fopen (inFileName, "rb");
	if (!inFile){
		printf("\nError: cannot open input H.264 file!");
		return 0;
	}
	else
		printf("\ninfile:%s\n",inFileName);
	
#ifdef DUMP_YUV
	if (outFileName[0] != '\0')
		outFile = fopen(outFileName, "wb");
	else
		outFile = NULL;

	if (!outFile){
		printf("\nWarning: no output video file!");
	}
#endif

	start = GetSysTime();

	returnCode = decApi.Init(&hCodec,VO_VIDEO_CodingUnused,0);//&userData);//voH264DecInit(&hCodec);
	if(returnCode)
	{
		printf("\nError %d: Decoder start failed!", returnCode);
		return(0); 
	}

	int nthreads = 2;
	decApi.SetParam(hCodec,VO_PID_COMMON_CPUNUM,(VO_PTR)&nthreads);//
	
	frameNumber = 0;
	param=VO_H264_ANNEXB;////
	//printf("init done\n");
	decApi.SetParam(hCodec,VO_ID_H264_STREAMFORMAT, &param);
	
	inputBuf = (char *)calloc(INBUFFER_SIZE ,1);
	nextFrame = currFrame = inputBuf;
	leftSize = fread(currFrame,1,INBUFFER_SIZE,inFile);
	currFrame = GetNextFrame(currFrame-3,leftSize);	

	while(frameNumber < outTotalFrame&&leftSize>4){
	

		do 
		{
			nextFrame = GetNextFrame(currFrame,leftSize);
			if(nextFrame)
			{
				inSize = nextFrame-currFrame;
				break;
			}
			else
			{
				if(leftSize>256*1024)
				{
					printf("\nthe next frame is too big to handle,return\n");
					return(0);
				}
				if(feof(inFile))
				{
					endOfFile = 1;
					inSize=leftSize;
					break;
				}
				else
				{
					int readSize;
					memmove(inputBuf,currFrame,leftSize);
					currFrame=inputBuf;
					readSize=fread(currFrame+leftSize,1,INBUFFER_SIZE-leftSize,inFile);
					leftSize+=readSize;
				}
			}
		} while(1);
			
		

	
		inFrame++;
		
		inData.Buffer           = (unsigned char *)currFrame;
		inData.Length 			= inSize;
		inData.Time				= inFrame;
	
		leftSize-=inSize;
		currFrame=nextFrame;

		returnCode = decApi.SetInputData(hCodec,&inData);//voH264DecProcess(hCodec, &inData,&outData, &outFormat);
		if(returnCode)
		{
			printf("\nError %d: Decod failed!\n", returnCode);
		}

	
		returnCode=decApi.GetOutputData(hCodec,&outData,&outFormat);
        if(outData.Buffer[0])
		{
			
			OutputOneFrame(&outData,outFormat.Format.Width,outFormat.Format.Height,outFormat.Format.Type );
			while(outFormat.Flag)	//there has next output
	    {
        returnCode=decApi.GetOutputData(hCodec,&outData,&outFormat);
		    if(outData.Buffer[0]) 
				  OutputOneFrame(&outData,outFormat.Format.Width,outFormat.Format.Height,outFormat.Format.Type );
		    else //the next output is not ready, just break
			    break;
	    }
		}		
	}
	
	decApi.Uninit(hCodec);


	finish = GetSysTime();
	duration = (double)(finish - start) / 1000;
	printf("\nH.264 decoder speed = %2.3f (fps) (%dx%d)", frameNumber/duration,outFormat.Format.Width,outFormat.Format.Height);
	printf("\n\ttotal frame = %d, total time = %2.3f seconds(%d,%d)\n", 
		frameNumber, duration,outFormat.Format.Width,outFormat.Format.Height);

	if (speedFile){
		fprintf(speedFile, "\nH.264 decoder speed = %2.3f (fps) (%dx%d)", frameNumber/duration,outFormat.Format.Width,outFormat.Format.Height);
		fprintf(speedFile, "\n\ttotal frame = %d, total time = %2.3f seconds\n", 
			frameNumber, duration);
	
		//fclose (speedFile);
	}

	free(inputBuf);
	fclose(inFile);
	if (outFile)
		fclose(outFile);
	inFile=outFile=NULL;

	if(speedFile)
		fclose (speedFile);
	speedFile=NULL;



	return 0;
}


/************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
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
#include		<limits.h>

#include <voRealVideo.h>

static int	outTotalFrame = INT_MAX;
static int inputWidth,inputHeight;
static int rv_version=4;
static char  configfile[256];
static FILE* fconfig=NULL;
static int nDecodeNextFile=0;
//static char tmpBuf[1024];

#define RV_RAW_DATA
static void ParseConfig(char* inFile,char* outFile,char* speedFile,int* loopCount,int* outFrameNumber)
{
	nDecodeNextFile = 0;//default is 0
	if(fconfig)
	{
		//printf("file pos=%d\n",ftell(fconfig));
		fscanf(fconfig,"%*[^\"]");
		fscanf(fconfig,"\"%[^\"]",inFile);
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		fscanf(fconfig,"\"%[^\"]",outFile);               // RAW (YUV/RGB) output file
		fscanf(fconfig,"%*[^\n]");
		fscanf(fconfig,"%*[^\"]");

		fscanf(fconfig,"\"%[^\"]",speedFile);               // speed file
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",outFrameNumber);           
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",loopCount);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&inputWidth);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&inputHeight);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&rv_version);          
		fscanf(fconfig,"%*[^\n]");

		fscanf(fconfig,"%d",&nDecodeNextFile);          
		fscanf(fconfig,"%*[^\n]");

	}
}

FILE* outFile=NULL;
int frameNumber=0;
int  OutputOneFrame(VO_VIDEO_BUFFER *outData,VO_VIDEO_FORMAT* outFormat)
{

	int i;
	char* buf;
	int width  = outFormat->Width;
	int height = outFormat->Height;

	int size = width*height;
	int stride = outData->Stride[0];
	frameNumber++;
	if (!outFile)
		return 0;

	printf("output frame %d(%dx%d)\n",frameNumber,width,height);

	// write Y, U, V;
	buf  = outData->Buffer[0];
	for(i=0;i<height;i++,buf+=stride)
	{
		fwrite(buf, 1, width, outFile);
	}	

	width /=2;
	height/=2;
	stride = outData->Stride[1];
	buf	 = outData->Buffer[1];

	for(i=0;i<height;i++,buf+=stride)
	{
		fwrite(buf, 1, width, outFile); 
	}	

	stride = outData->Stride[2];
	buf	   = outData->Buffer[2];

	for(i=0;i<height;i++,buf+=stride)
	{
		fwrite(buf, 1, width, outFile); 
	}	
	fflush(outFile);
	return 0;
}
#define XRAW_IS_PSC_RV8(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1) \
	&& !(*((p)+3)&170) && !(*((p)+4)&170) && !(*((p)+5)&170) \
	&& ((*((p)+6)&170)==2))

#define XRAW_IS_PSC_G2(p) (!*(p) && !*((p)+1) && ((p)[2] & 0xFC) == 0x80)

/* need longer start code to avoid start code emulation */
#define XRAW_IS_PSC(p) ( \
	(*((p)+0)==85) && \
	(*((p)+1)==85) && \
	(*((p)+2)==85) && \
	(*((p)+3)==85) && \
	!(*((p)+4)&170) && \
	!(*((p)+5)&170) && \
	!(*((p)+6)&170) && \
	((*((p)+7)&170)==2))

static char* GetNextFrame(int version,char* currPos,int size)
{
	char* p = currPos + 1;  //Skip past first byte of picture header 
	char* endPos = currPos+size-8;
	for (; p < endPos; p++)
	{

		if (version==3)//RV8
		{
			if (XRAW_IS_PSC_RV8(p))
				break;
		}
		else if(version==4)
		{
			if (XRAW_IS_PSC(p))
				break;
		}
		else
		{
			if (XRAW_IS_PSC_G2(p))
				break;
		}
	}
	if(p>=endPos)
		p = NULL;

	return p;
}

#define READ_LEN 512
static char* sdcard[]={"storage card","sd memory","sdmmc"};
enum
{
	STORAGE_CARD=0,
	SD_MEMORY,
	SDMMC
};
FILE *inFile, *outFile, *speedFile;
int frameNumber;
#ifdef _WIN32_WCE
int WinMain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv) 
#endif // _WIN32_WCE
{
	char *inputBuf,*outputBuf=NULL;
	unsigned int j;
	VO_HANDLE			hCodec;
	VO_U32				returnCode;
	VO_CODECBUFFER		inData;
	VO_VIDEO_BUFFER		outData;
	VO_VIDEO_FORMAT 	outFormat;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI		voRVDec;

	VORV_FORMAT_INFO *rvFormatInfo;
	VORV_FRAME *rvFrame;

	VORV_FORMAT_INFO rv_info;
	VORV_FRAME rv_frame;

	char inFileName[256], outFileName[256], outSpeedName[256];
	double duration;
	int outTotalFrame = INT_MAX;
	int loopCount=1;
	int inSize = 0;
	int inFrame = 0;
	int	endOfFile = 0;
	int IsUseCallBack = 1;
	char *nextFrame,*currFrame;
	int  leftSize=0;
	int  size=0;
#ifdef _WIN32_WCE
	DWORD start, finish;
	//#define TEST_SPEED 0
#define TEST_SPEED 1

	int sdPath= SDMMC;//STORAGE_CARD;
	//int sdPath= STORAGE_CARD;//STORAGE_CARD;
	/*if(TEST_SPEED)
	sprintf(configfile,"/%s/rv_ppc/%s/rv_cfg_s.txt",sdcard[sdPath],sdcard[sdPath]);
	else
	sprintf(configfile,"/%s/rv_ppc/%s/rv_cfg.txt",sdcard[sdPath],sdcard[sdPath]);*/
	if(TEST_SPEED)
		sprintf(configfile,"\\SDMMC\\RV_cfg_new.txt",sdcard[sdPath],sdcard[sdPath]);
	else
		sprintf(configfile,"rv_cfg.txt",sdcard[sdPath],sdcard[sdPath]);
	fconfig = fopen(configfile,"r");
#else
	clock_t start, finish;
	strcpy(configfile,argv[1]);
	fconfig = fopen(configfile,"r");
#endif


	do
	{
		ParseConfig(inFileName,outFileName,outSpeedName,&loopCount,&outTotalFrame);
		if (outSpeedName[0] != '\0')
			speedFile = fopen(outSpeedName, "wb");
		else
			speedFile = NULL;

		if (!speedFile){
			printf("\nWarning: no output speed file!");
		}
#define MAX_INPUT_BUFSIZE  128*1024
#define MAX_OUTPUT_BUFSIZE 640*480*3
		// allocate  buffer;
		inputBuf = (char *)malloc(MAX_INPUT_BUFSIZE);//

		do 
		{
			endOfFile = 0;
			inFile= fopen (inFileName, "rb");
			if (!inFile){
				printf("\nError: cannot open input RealVideo file!%s",inFileName);
				exit(0);
			}

			if (loopCount<2&&outFileName[0] != '\0')
				outFile = fopen(outFileName, "wb");
			else
				outFile = NULL;

			if (!outFile){
				printf("\nWarning: no output video file!");
			}

			/* get decoder api*/
			returnCode = voGetRVDecAPI(&voRVDec, 0);

			returnCode = voRVDec.Init(&hCodec, VO_VIDEO_CodingRV, NULL);

			if(returnCode<0){
				printf("\nError %d: Decoder init failed!\n", returnCode);
				return 1;
			}
			frameNumber = 0;
			currFrame = inputBuf;
			leftSize = fread(currFrame,1,MAX_INPUT_BUFSIZE,inFile);
#ifdef RV_RAW_DATA
			//memset(&rv_info,0, sizeof(VORV_FORMAT_INFO));
			//memset(&rv_frame, 0, sizeof(VORV_FRAME));

			//rv_info.ulSubMOFTag = 0x54524F4D;
			//if(rv_version == 3){
			//	rv_info.ulLength = 0x38000000;
			//}else{
			//	rv_info.ulLength = 0x48000000;
			//}
			//rv_info.usWidth = inputWidth;
			//rv_info.usHeight = inputHeight;
			//rvInitParam.format = &rv_info;

			//inData.Buffer = (unsigned char*)&rvInitParam;
			//inData.Length = sizeof(VORV_INIT_PARAM);
			//returnCode = voRVDec.SetParam(hCodec,VO_PID_DEC_RealVideo_INIT_PARAM,(VO_PTR)&inData);
			{
				memset(&rv_info,0, sizeof(VORV_FORMAT_INFO));
				memset(&rv_frame, 0, sizeof(VORV_FRAME));

				rv_info.ulSubMOFTag = 0x54524F4D;
				if(rv_version == 3){
					rv_info.ulLength = 0x38000000;
				}else{
					rv_info.ulLength = 0x48000000;
				}
				rv_info.usWidth = inputWidth;
				rv_info.usHeight = inputHeight;

				returnCode = voRVDec.SetParam(hCodec,VO_PID_DEC_RealVideo_INIT_PARAM,(VO_PTR)&rv_info);

			}
#else
			//rvFormatInfo = (VORV_FORMAT_INFO*)currFrame;
			////skip initial data
			//currFrame += sizeof(VORV_FORMAT_INFO);
			//leftSize -= sizeof(VORV_FORMAT_INFO);

			//rvFormatInfo->pOpaqueData = (unsigned char*)currFrame;
			//currFrame += rvFormatInfo->ulOpaqueDataSize;
			//leftSize -= rvFormatInfo->ulOpaqueDataSize;

			//rvInitParam.format = rvFormatInfo;

			//inData.Buffer = (unsigned char*)&rvInitParam;
			//inData.Length = sizeof(VORV_INIT_PARAM);
			//returnCode = voRVDec.SetParam(hCodec,VO_PID_COMMON_HEADDATA,(VO_PTR)&inData);

			rvFormatInfo = (VORV_FORMAT_INFO*)currFrame;
			//skip initial data
			currFrame += sizeof(VORV_FORMAT_INFO);
			leftSize -= sizeof(VORV_FORMAT_INFO);

			rvFormatInfo->pOpaqueData = (unsigned char*)currFrame;
			currFrame += rvFormatInfo->ulOpaqueDataSize;
			leftSize -= rvFormatInfo->ulOpaqueDataSize;

			returnCode = voRVDec.SetParam(hCodec,VO_PID_COMMON_HEADDATA,(VO_PTR)rvFormatInfo);
#endif	
			if(returnCode<0){
				printf("\nError %d: Decoder init SetParameter failed!\n", returnCode);
				return 1;
			}


#ifdef _WIN32_WCE
			start = GetTickCount();
#else
			start = clock();
#endif
			while(frameNumber < outTotalFrame&&leftSize>4){
				do 
				{
#ifdef RV_RAW_DATA
					if(rv_version == 3){
						if (!(XRAW_IS_PSC_RV8(currFrame))){
							nextFrame = GetNextFrame(rv_version,currFrame,leftSize);
							leftSize = leftSize - (nextFrame - currFrame);
							currFrame = nextFrame;
						}
					}else{
						if (!(XRAW_IS_PSC(currFrame))){
							nextFrame = GetNextFrame(rv_version,currFrame,leftSize);
							leftSize = leftSize - (nextFrame - currFrame);
							currFrame = nextFrame;
						}
					}
#else
					if (!(XRAW_IS_PSC(currFrame))){
						nextFrame = GetNextFrame(rv_version,currFrame,leftSize);
						leftSize = leftSize - (nextFrame - currFrame);
						currFrame = nextFrame;
					}
#endif

					nextFrame = GetNextFrame(rv_version,currFrame,leftSize);
					if(nextFrame){
						inSize = nextFrame-currFrame;
						break;
					}else{
						if(feof(inFile)){
							endOfFile = 1;
							inSize=leftSize;
							break;
						}else{
							int readSize;
							memmove(inputBuf,currFrame,leftSize);
							currFrame=inputBuf;
							readSize = fread(currFrame+leftSize,1,MAX_INPUT_BUFSIZE-leftSize,inFile);
							leftSize+=readSize;
						}
					}
				} while(1);

				leftSize-=inSize;
#ifdef RV_RAW_DATA
				rv_frame.pData = (unsigned char*)currFrame;
				rv_frame.ulDataLen = inSize;

				inData.Buffer = (VO_PBYTE *)(&rv_frame);
#else
				inData.Length = inSize;
				//skip PSC
				currFrame += 8;
				inSize -= 8;

				rvFrame = (VORV_FRAME*)currFrame;
				currFrame += sizeof(VORV_FRAME);
				inSize -= sizeof(VORV_FRAME);	

				rvFrame->pSegment = (VORV_SEGMENT*)calloc(1, rvFrame->ulNumSegments*sizeof(VORV_SEGMENT));

				while(!rvFrame->pSegment);


				if(rvFrame->ulNumSegments>1)
					rvFrame->ulNumSegments= rvFrame->ulNumSegments;
				for(j = 0; j < rvFrame->ulNumSegments; j++){
					rvFrame->pSegment[j].bIsValid = ((VORV_SEGMENT*)currFrame)->bIsValid;
					rvFrame->pSegment[j].ulOffset = ((VORV_SEGMENT*)currFrame)->ulOffset;
					currFrame += sizeof(VORV_SEGMENT);
					inSize -= sizeof(VORV_SEGMENT);
				}

				rvFrame->pData = (unsigned char*)currFrame;
				rvFrame->ulDataLen = inSize;

				inData.Buffer = (unsigned char*)rvFrame;
				inData.Length = inSize;
#endif		

				//if(6 == inFrame){
				//	voRVDec.SetParam(hCodec,VO_PID_DEC_RealVideo_FLUSH,1);
				//}

				returnCode = voRVDec.SetInputData(hCodec, &inData);
				returnCode = voRVDec.GetOutputData(hCodec, &outData, &outInfo);

				//returnCode = voRVDec.SetInputData(hCodec, &inData);
				//if(5 == inFrame){
				//	voRVDec.GetParam(hCodec, VO_PID_VIDEO_FRAMETYPE, (VO_PTR*)&inData);
				//}else{
				//	returnCode = voRVDec.GetOutputData(hCodec, &outData, &outInfo);
				//}

				outFormat =outInfo.Format;

				if(returnCode < 0){
					printf("\nFrame=%d,Error %d: Decoder data receiving failed!\n", inFrame,returnCode);
					continue;//go on decoding next frame
				}
#ifndef RV_RAW_DATA
				if(rvFrame->pSegment) {
					free(rvFrame->pSegment);
					rvFrame->pSegment = 0;
				}
#endif
				if(outFormat.Type!=VO_VIDEO_FRAME_NULL){
					OutputOneFrame(&outData, &outFormat);
				}

				//if(NULL_FRAME != outFormat.frame_type){
				//	OutputOneFrame(&outData,outFormat.width,outFormat.height);
				//}

				inFrame++;	
				currFrame=nextFrame;
			}

			voRVDec.GetParam(hCodec, VO_PID_DEC_RealVideo_GETLASTOUTVIDEOBUFFER, (VO_PTR*)&outData);
			voRVDec.GetParam(hCodec, VO_PID_DEC_RealVideo_GETLASTOUTVIDEOFORMAT, (VO_PTR*)&outFormat);

			outFormat =outInfo.Format;

			if(outFormat.Type!=VO_VIDEO_FRAME_NULL){
				OutputOneFrame(&outData, &outFormat);
			}

			voRVDec.Uninit(hCodec);

#ifdef _WIN32_WCE
			finish = GetTickCount();
			duration = (finish - start) * 0.001;
#else
			finish = clock();
			duration = (double)(finish - start) / CLOCKS_PER_SEC;
			printf("\nRealVideo decoder speed = %2.3f (fps)", frameNumber/duration);
			printf("\n\ttotal frame = %d, total time = %2.3f seconds\n", 
				frameNumber, duration);
#endif //_WIN32_WCE

			if (speedFile){
				fprintf(speedFile, "\nRealVideo decoder speed = %2.3f (fps) (%dx%d)", frameNumber/duration,outFormat.Width,outFormat.Height);
				fprintf(speedFile, "\n\ttotal frame = %d, total time = %2.3f seconds\n", 
					frameNumber, duration);
				//fclose (speedFile);
			}

			fclose(inFile);
			inFile=NULL;
			if (outFile)
				fclose(outFile);
			outFile=NULL;
		} while(--loopCount>0);
		free(inputBuf);
		//free(outputBuf);

		fclose (speedFile);
		speedFile=NULL;
	}while (nDecodeNextFile);
	fclose(fconfig);
	fconfig= NULL;
	return 0;
}

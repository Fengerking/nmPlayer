/************************************************************************
      VisualOn, Inc. Confidential and Proprietary, 2008
      File: rv89decsample.c

Notes: This is a sample application which demonstrates how to properly exercise the RealVideo API
       set. This is not considered production quality code.

Usage Notes:
 
  Input :   RV dump file. It is video data dumped from rm & rmvb file by a VisualOn tool.
  	
     VisualOn dump file format
       
     Start code = 0x55555555 52040000
     --------------------------------------------------------------------------------------
     |          |                       |          |                    |   
     |start code| frame data  (0)       |start code| frame data  (1)    |................... 
     |          |                       |          |                    |
     ---------------------------------------------------------------------------------------
     
     NOTICE: Start code only is used in VisualOn dump file, and it can't be found in other format file.
     	       Frame data is availability for all format file, and it should be transfered to decoder.
     	       
  Output:   Binary file with YUV420 data. 		            
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

#include        "voRealVideo.h"

static int	outTotalFrame = INT_MAX;
static int inputWidth,inputHeight;
static int rv_version=4;
static char  configfile[256];
static FILE* fconfig=NULL;
static int nDecodeNextFile=0;

//#define RV_RAW_DATA

static void ParseConfig(char* inFile,char* outFile,char* speedFile,int* loopCount,int* outFrameNumber)
{
	nDecodeNextFile = 0;//default is 0

	if(fconfig){
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

	int size = width * height;
	int stride = outData->Stride[0];
	frameNumber++;
	if (!outFile)
		return 0;

	printf("output frame %d(%dx%d)\n",frameNumber,width,height);

	// write Y;
	buf  = (char*)outData->Buffer[0];
	for(i=0;i<height;i++,buf+=stride){
		fwrite(buf, 1, width, outFile);
	}	

	// write U;
	width /=2;
	height/=2;
	stride = outData->Stride[1];
	buf	 = (char*)outData->Buffer[1];

	for(i=0;i<height;i++,buf += stride){
		fwrite(buf, 1, width, outFile); 
	}	

	// write V;
	stride = outData->Stride[2];
	buf	   = (char*)outData->Buffer[2];

	for(i = 0;i < height;i++,buf += stride){
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

	for (; p < endPos; p++){

		if (version == 3){//RV3
			if (XRAW_IS_PSC_RV8(p))
				break;
		}else if(version == 4){//RV4
			if (XRAW_IS_PSC(p))
				break;
		}else{//RV2
			if (XRAW_IS_PSC_G2(p))
				break;
		}
	}

	if(p >= endPos)
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

FILE *inFile, *speedFile;
//int frameNumber;
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

	VORAW_INITParam initParam;
	//VORV_INIT_PARAM rvInitParam;
	VORV_FORMAT_INFO *rvFormatInfo;
	VORV_FRAME *rvFrame;
	VORV_FORMAT_INFO rv_info;
	VORV_FRAME rv_frame;
	void* state;

	char inFileName[256], outFileName[256], outSpeedName[256];
	double duration;
	int outTotalFrame = INT_MAX;
	int format;
	int loopCount=1;
	int inSize = 0;
	int inFrame = 0;
	int	endOfFile = 0;
	int IsUseCallBack = 1;
	char* nextFrame,*currFrame;
	int leftSize=0;
	int size=0;
#ifdef _WIN32_WCE
	DWORD start, finish;
#define TEST_SPEED 1

	int sdPath= SDMMC;//STORAGE_CARD;

	if(TEST_SPEED)
		sprintf(configfile,"\\RV_cfg_new.txt",sdcard[sdPath],sdcard[sdPath]);
	else
		sprintf(configfile,"rv_cfg.txt",sdcard[sdPath],sdcard[sdPath]);
	fconfig = fopen(configfile,"r");
#else
	int i;
	clock_t start, finish;
	strcpy(configfile,argv[1]);
	fconfig = fopen(configfile,"r");
#endif


	do{/* Test files loop */
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
		/* allocate input buffer*/
		inputBuf = (char *)malloc(MAX_INPUT_BUFSIZE);
		rvFrame = (VORV_FRAME*)malloc(sizeof(VORV_FRAME));

		do{/* Test count loop*/
			endOfFile = 0;
			inFile= fopen (inFileName, "rb");

			if (!inFile){
				printf("\nError: cannot open input RealVideo file!%s",inFileName);
				exit(0);
			}

			if (loopCount < 2 && outFileName[0] != '\0')
				outFile = fopen(outFileName, "wb");
			else
				outFile = NULL;

			if (!outFile){
				printf("\nWarning: no output video file!");
			}

			/* Get decoder api*/
			returnCode = voGetRVDecAPI(&voRVDec, 0);
			if(returnCode != VO_ERR_NONE){/* Get API error */ 
				goto End;
			}

			/* Initialize decoder handle*/
			returnCode = voRVDec.Init(&hCodec, VO_VIDEO_CodingRV, NULL);
			if(returnCode != VO_ERR_NONE){
				goto End;
			}


			frameNumber = 0;
			currFrame = inputBuf;
			
			/* Read raw video data */
			leftSize = fread(currFrame, 1, MAX_INPUT_BUFSIZE, inFile);
			
#ifdef RV_RAW_DATA
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

#ifndef NXPVER
				inData.Buffer = (unsigned char*)&rv_info;
				inData.Length = sizeof(VORV_FORMAT_INFO);

				/* Set rv format to initialize decoder */
				returnCode = voRVDec.SetParam(hCodec,VO_PID_DEC_RealVideo_INIT_PARAM,(VO_PTR)&inData);
#else
				returnCode = voRVDec.SetParam(hCodec,VO_PID_DEC_RealVideo_INIT_PARAM,(VO_PTR)&rv_info);
#endif
				

			}
#else		
			/**************************************************************
			 Prepare rv format for decoder initialization 
			****************************************************************/			
			rvFormatInfo = (VORV_FORMAT_INFO*)currFrame;
			/* Skip initial data */
			currFrame += sizeof(VORV_FORMAT_INFO);
			leftSize -= sizeof(VORV_FORMAT_INFO);
			
			/* Set opaque data in rv format*/
			rvFormatInfo->pOpaqueData = (unsigned char*)currFrame;
			currFrame += rvFormatInfo->ulOpaqueDataSize;
			leftSize -= rvFormatInfo->ulOpaqueDataSize;
#ifndef NXPVER
			inData.Buffer = (unsigned char*)rvFormatInfo;
			inData.Length = sizeof(rvFormatInfo);

			/* Set rv format to initialize decoder */
			returnCode = voRVDec.SetParam(hCodec,VO_PID_COMMON_HEADDATA,(VO_PTR)&inData);

#else
			/* Set rv format to initialize decoder */
			returnCode = voRVDec.SetParam(hCodec,VO_PID_COMMON_HEADDATA,(VO_PTR)&rvFormatInfo);
#endif /*NXPVER end*/

#endif /*RV_RAW_DATA*/

			if(returnCode != VO_ERR_NONE){
				goto Finish;
			}


#ifdef _WIN32_WCE
			start = GetTickCount();
#else
			start = clock();
#endif
			while(frameNumber < outTotalFrame&&leftSize>4){/* Test frames loop */

				do{/* Get one completed rv frame from VO dumped file */
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
					if (!(XRAW_IS_PSC(currFrame))){/* Look for start code of current rv frame */
						nextFrame = GetNextFrame(rv_version, currFrame, leftSize);
						leftSize  = leftSize - (nextFrame - currFrame);
						currFrame = nextFrame;
					}
#endif					
					/* Look for start code of next rv frame */
					nextFrame = GetNextFrame(rv_version, currFrame, leftSize);

					if(nextFrame){/* Find the start code of next rv frame*/
						inSize = nextFrame - currFrame;
						
						break;/* Fined one rv frame*/
					}else{/* Didn't find the start code of next rv frame */
						if(feof(inFile)){/* End of file*/
							endOfFile = 1;
							inSize = leftSize;
							break;
						}else{/* Read more data from VO dumped file */
							int readSize;
							memmove(inputBuf, currFrame, leftSize);
							currFrame = inputBuf;
							readSize = fread(currFrame + leftSize, 1, MAX_INPUT_BUFSIZE - leftSize, inFile);
							leftSize += readSize;
						}
					}
				} while(1);

				leftSize -= inSize;
#ifdef RV_RAW_DATA
				rv_frame.pData = (unsigned char*)currFrame;
				rv_frame.ulDataLen = inSize;

				inData.Buffer = (VO_PBYTE)(&rv_frame);
#else
				inData.Length = inSize;

				/* Skip PSC */
				currFrame += 8;/*Start code 8 byte in dump file*/
				inSize -= 8;
				
				/***************************************************************************************
				Prepare rv frame strucure for decodeing one rv frame
				*****************************************************************************************/
				//rvFrame = (VORV_FRAME*)currFrame;
				memcpy(rvFrame,currFrame,sizeof(VORV_FRAME)); 
				currFrame += sizeof(VORV_FRAME);
				inSize -= sizeof(VORV_FRAME);	

				if(rvFrame->ulNumSegments){
					unsigned char *pSegment,*pSegmentTemp;
					rvFrame->pSegment = (VORV_SEGMENT*)calloc(1, rvFrame->ulNumSegments * sizeof(VORV_SEGMENT));

					if(!rvFrame->pSegment)/* Malloc buffer error*/
						goto Finish;

					pSegment = (unsigned char *)malloc(sizeof(VORV_SEGMENT)*rvFrame->ulNumSegments);
					memcpy(pSegment,currFrame,sizeof(VORV_SEGMENT)*rvFrame->ulNumSegments);
					pSegmentTemp = pSegment;
						
					/* Get valid and length of segment*/
					for(j = 0; j < rvFrame->ulNumSegments; j++){
						//rvFrame->pSegment[j].bIsValid = ((VORV_SEGMENT*)currFrame)->bIsValid;
						//rvFrame->pSegment[j].ulOffset = ((VORV_SEGMENT*)currFrame)->ulOffset;
						rvFrame->pSegment[j].bIsValid = ((VORV_SEGMENT*)pSegmentTemp)->bIsValid;
						rvFrame->pSegment[j].ulOffset = ((VORV_SEGMENT*)pSegmentTemp)->ulOffset;
						currFrame += sizeof(VORV_SEGMENT);
						pSegmentTemp += sizeof(VORV_SEGMENT);
						inSize -= sizeof(VORV_SEGMENT);
					}	

					free(pSegment);
				}

				rvFrame->pData = (unsigned char*)currFrame;
				rvFrame->ulDataLen = inSize;


				/* Set input data structure*/
				inData.Buffer = (unsigned char*)rvFrame;
				inData.Length = inSize;
#endif	

				/* Set one frame rv frame data*/
				inData.Time = inFrame;
				returnCode = voRVDec.SetInputData(hCodec, &inData);//set input data for decoder
				if(returnCode == VO_ERR_NONE){
					returnCode = voRVDec.GetOutputData(hCodec, &outData, &outInfo);/* Decode one rv frame */
					
					if(outInfo.Format.Type != VO_VIDEO_FRAME_NULL){/* Get YUV data*/
						OutputOneFrame(&outData, &outInfo.Format);
					}					
#ifndef RV_RAW_DATA
					if(rvFrame->pSegment){ /* Free malloced buffer of rv frame segment*/
						free(rvFrame->pSegment);
						rvFrame->pSegment = 0;
					}
#endif
					inFrame++;	
					/* Update raw video data buffer*/
					currFrame = nextFrame;
				}
			}

Lastframe:
			/* Get last decoded frame */
			voRVDec.GetParam(hCodec, VO_PID_DEC_RealVideo_GETLASTOUTVIDEOBUFFER, (VO_PTR*)&outData);
			voRVDec.GetParam(hCodec, VO_PID_DEC_RealVideo_GETLASTOUTVIDEOFORMAT, (VO_PTR*)&outFormat); 
			
			if(outFormat.Type != VO_VIDEO_FRAME_NULL){
				OutputOneFrame(&outData, &outInfo.Format);
			}
Finish:
			/* Uninitialize decoder handle*/
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
			}
End:
			fclose(inFile);
			inFile = NULL;
			if (outFile)
				fclose(outFile);
			outFile = NULL;
		} while(--loopCount > 0);

		free(rvFrame);
		free(inputBuf);
		fclose (speedFile);
		speedFile = NULL;
	}while (nDecodeNextFile);

	fclose(fconfig);
	fconfig= NULL;
	return 0;
}

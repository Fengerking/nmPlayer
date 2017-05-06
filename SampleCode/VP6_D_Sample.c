
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
#include		"voVP6.h"

#define OUTPUT_YUV_DATA  1		/* Flag to Write YUV data into file*/

#ifndef	RVDS
#define INPUT_FRAME_NUM        100
#else
#define INPUT_FRAME_NUM        1000
#endif
#ifdef _WIN32_WCE
#define INPUT_FILE_NAME        "/Storage Card/vp6/1920x1088'24f'2c'44KHz.vp6"/*  PHDec.mp4v\\SD Card\\*/
#define OUT_FILE_NAME           "/Storage Card/vp6/1920x1088'24f'2c'44KHz.yv12"
#define OUT_SPEED_FILE_NAME  "/Storage Card/vp6/peed.txt"
#else
#define INPUT_FILE_NAME        "E:/MyResource/Video/clips/VP6/FLV_192x154_6fps_MP3_56KBps_MS-Race.vp6"/*  PHDec.mp4v\\SD Card\\*/
#define OUT_FILE_NAME          "E:/MyResource/Video/clips/VP6/FLV_192x154_6fps_MP3_56KBps_MS-Race.yuv"
#define OUT_SPEED_FILE_NAME    "E:/MyResource/Video/clips/VP6/Speed_vp6rv.txt"
#endif

#define IN_BUFF_LEN 1024*1024
FILE *inFile, *outFile, *speedFile;
int frameNumber, nullFrame, unNullFrame;


int OutputOneFrame(VO_VIDEO_BUFFER *par, VO_VIDEO_FORMAT* outFormat)
{
	VO_S32 i, width,height;
	VO_U8 * out_src;
	
	//printf("Output frame %d type 0x%x\n", frameNumber, outFormat->Type);
	
	if (!outFile)
		return 0;

	width = outFormat->Width;
	height= outFormat->Height;
	/* output decoded YUV420 frame */
	/* Y */
	out_src = par->Buffer[0];
	for(i = 0;i < height; i++){
		fwrite(out_src, width, 1, outFile);
		out_src += par->Stride[0];
	}

	/* U */
	out_src = par->Buffer[1];
	for(i = 0; i< height/2; i++){
		fwrite(out_src, width/2, 1, outFile);
		out_src += par->Stride[1];
	}

	/* V */
	out_src = par->Buffer[2];
	for(i = 0; i < height/2; i++){
		fwrite(out_src, width/2, 1, outFile);
		out_src += par->Stride[2];
	}

	return 0;
}


#ifdef RVDS
__asm void init_cpu() {
// Set up CPU state
		MRC p15,0,r4,c1,c0,0
	ORR r4,r4,#0x00400000 // enable unaligned mode (U=1)
	BIC r4,r4,#0x00000002 // disable alignment faults (A=0) // MMU not enabled: no page tables
	MCR p15,0,r4,c1,c0,0
#ifdef __BIG_ENDIAN
SETEND BE
#endif
	MRC p15,0,r4,c1,c0,2 // Enable VFP access in the CAR -
	ORR r4,r4,#0x00f00000 // must be done before any VFP instructions
	MCR p15,0,r4,c1,c0,2
	MOV r4,#0x40000000 // Set EN bit in FPEXC
	MSR FPEXC,r4

	IMPORT __main
	B __main
}
#endif

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv) 
#endif // _WIN32_WCE
{
	VO_U8	*readBuf, *inputBuf ;
	VO_U8 inFileName[256], outFileName[256], outSpeedName[256];
	double duration;
	VO_S32 outTotalFrame = INT_MAX;
	VO_S32 tempLen, leftLen = 0, fileSize = 0;
	VO_S32 frameLen = 0;

	VO_HANDLE			hCodec;
	VO_S32				returnCode;
	VO_CODECBUFFER		inData;
	VO_VIDEO_BUFFER		outData;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI		voVP6Dec;


#ifdef _WIN32_WCE
	DWORD start, finish;
	outFileName[0] = outSpeedName[0] = '\0';
	sprintf(inFileName, INPUT_FILE_NAME);
	sprintf(outFileName, OUT_FILE_NAME);
	sprintf(outSpeedName, OUT_SPEED_FILE_NAME);
	outTotalFrame = 20;//INPUT_FRAME_NUM;
	//start = GetTickCount();
#else
	VO_S32 i;
	clock_t start, finish;
	outFileName[0] = outSpeedName[0] = '\0';
#ifdef RVDS
	sprintf(inFileName, INPUT_FILE_NAME);
	sprintf(outFileName, OUT_FILE_NAME);
	sprintf(outSpeedName, OUT_SPEED_FILE_NAME);
	outTotalFrame = INPUT_FRAME_NUM;
#else
	if (argc > 1){
		for (i = 1; i < argc; i++){
			if (0 == strncmp (argv[i], "-i", 2)){
				strcpy((char*)inFileName, argv[i+1]);
				printf("\n INPUT: input file: %s", inFileName);
			}
			else if (0 == strncmp (argv[i], "-o", 2)){
				strcpy((char*)outFileName, argv[i+1]);
				printf("\n INPUT: output video YUV420 file: %s", outFileName);
			}
			else if (0 == strncmp (argv[i], "-s", 2)){
				strcpy((char*)outSpeedName, argv[i+1]);
				printf("\n INPUT: test speed file: %s", outSpeedName);
			}
			else if (0 == strncmp (argv[i], "-f", 2)){
				outTotalFrame = atoi(argv[i+1]);
				printf("\n INPUT: user specified total decode frames: %d", outTotalFrame);
			}
			else {
				printf("Usage: %s [-i inputFileName] {[-f totalFrames] [-o outVideoFileName] [-s outSpeedFileName]} \n",argv[0]);
				exit(0);
			}
			i++;
		}	
	}
	else { 
		//printf("Usage: %s [-i inputFileName] {[-f totalFrames] [-o outVideoFileName] [-s outSpeedFileName]} \n",argv[0]);
		sprintf(inFileName, INPUT_FILE_NAME);
		sprintf(outFileName, OUT_FILE_NAME);
		sprintf(outSpeedName, OUT_SPEED_FILE_NAME);
		outTotalFrame = INT_MAX;//INT_MAX;//INT_MAX;// INPUT_FRAME_NUM;
		//exit(0);
	}
#endif
#endif

	frameNumber = 0;
	nullFrame = 0;
	unNullFrame = 0;
	inFile= fopen ((char*)inFileName, "rb");
	if (!inFile){
		printf("\nError: cannot open input VP6 file!");
		exit(0);
	}

	if (outFileName[0] != '\0')
		outFile = fopen((char*)outFileName, "wb");
	else
		outFile = NULL;


	if (!outFile){
		printf("\nWarning: no output video file!");
	}

	if (outSpeedName[0] != '\0')
		speedFile = fopen((char*)outSpeedName, "wb");
	else
		speedFile = NULL;

	if (!speedFile){
		printf("\nWarning: no output speed file!\n");
	}


	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	/* allocate input buffer*/
	readBuf = (VO_U8 *)malloc(IN_BUFF_LEN * sizeof (VO_S8));

	/* get decoder api*/
	returnCode = voGetVP6DecAPI(&voVP6Dec, 0);
	if(returnCode != VO_ERR_NONE){
		goto End;
	}

	/* initialize decoder handle*/
	returnCode = voVP6Dec.Init(&hCodec, VO_VIDEO_CodingVP6, NULL);
	if(returnCode != VO_ERR_NONE){
		goto End;
	}


	/* Read video raw data*/
	if(!(leftLen = fread(readBuf, 1, IN_BUFF_LEN, inFile)))
		goto End; 


	//voVP6Dec.SetParam(hCodec,VO_PID_DEC_VP6_SET_VIDEO_WIDTH, (VO_PTR)192); //zou
	//voVP6Dec.SetParam(hCodec,VO_PID_DEC_VP6_SET_VIDEO_HEIGHT, (VO_PTR)154);	
	//returnCode = voVP6Dec.SetParam(hCodec, VO_PID_COMMON_HEADDATA, (VO_PTR)&inData);
	if(returnCode != VO_ERR_NONE)
		goto End;

#ifdef _WIN32_WCE
	start = GetTickCount();
#else
	start = clock();
#endif

	inputBuf = readBuf;

	while((fileSize>0) && (outTotalFrame>0)){

		/* Make sure there is one frame data*/
		if( (leftLen >3) && ((leftLen - 4) >= (inputBuf[3]<<24|inputBuf[2]<<16|inputBuf[1]<<8|inputBuf[0])))
		{
			frameLen = inputBuf[3]<<24|inputBuf[2]<<16|inputBuf[1]<<8|inputBuf[0];
			/* Initialize input buffer for one frame*/
			inData.Length = frameLen;
			inData.Buffer = (VO_PBYTE)(inputBuf + 4);		
		}
		else
		{
			/* if the leftdata is less than this frameLen, read more data to input buffer*/
			memmove(readBuf, inputBuf,  leftLen);
			tempLen = fread(readBuf + leftLen, 1, (IN_BUFF_LEN - leftLen), inFile);
			if(tempLen >0)
			{
				leftLen = leftLen + tempLen;
				inputBuf = readBuf;
				continue;
			}
			else
			{
				/* end of the inFile*/
					break;
			}
		}		

		if(frameNumber==2)
			outTotalFrame = outTotalFrame;
				
		/* Set frames of raw video data*/
		voVP6Dec.SetInputData(hCodec, &inData); 
		/* Get one frame decoded data*/
		returnCode = voVP6Dec.GetOutputData(hCodec, &outData, &outInfo);

		printf("outTotalFrame = %d  %d %d %d\n",outTotalFrame,frameNumber,outInfo.Format.Width,outInfo.Format.Height);


		/* subtract current frame lenth from input buffer */
		if(returnCode == VO_ERR_NONE)
		{
			/*No error in current frame*/
			outTotalFrame--;
			if(outInfo.Format.Type != VO_VIDEO_FRAME_NULL){
#if OUTPUT_YUV_DATA
				OutputOneFrame(&outData, &outInfo.Format);
#endif
				frameNumber++;
				unNullFrame++;
			}else{
				if(frameNumber){
					nullFrame++;
				}
				frameNumber++;
			}
		}
		/*update input structure*/
		inputBuf += (frameLen + 4);
		fileSize -= (frameLen + 4);
		leftLen  -= (frameLen + 4);
	}


	/* Get the last decoded frame*/
/*
	voVP6Dec.GetParam(hCodec, VO_PID_DEC_VP6_GET_LASTVIDEOBUFFER, (VO_PTR*)&outData);
	voVP6Dec.GetParam(hCodec, VO_PID_DEC_VP6_GET_LASTVIDEOINFO, (VO_PTR*)&outInfo);

	if(outInfo.Format.Type != VO_VIDEO_FRAME_NULL){
#if OUTPUT_YUV_DATA
		OutputOneFrame(&outData, &outInfo.Format);
#endif
		frameNumber++;
		unNullFrame++;
	}else{
		nullFrame++;
	}
*/
	/* free decoder* handle*/
	voVP6Dec.Uninit(hCodec);

#ifdef _WIN32_WCE
	finish = GetTickCount();
	duration = (finish - start) * 0.001;
#else
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf("\nVP6 decoder speed = %2.3f (fps)", unNullFrame/duration);
    printf("\n\ttotal frame = %d, NULL frame = %d, total time = %2.3f seconds\n", 
		frameNumber,  nullFrame,  duration);
#endif //_WIN32_WCE

	if (speedFile){
		fprintf(speedFile, "\nVP6 decoder speed = %2.3f (fps)", unNullFrame/duration);
		fprintf(speedFile, "\n\ttotal frame = %d, NULL frame = %d, total time = %2.3f seconds\n", 
			frameNumber,  nullFrame,  duration);
		fclose (speedFile);
	}
End:
	free(readBuf);
	fclose(inFile);
	if (outFile)
		fclose(outFile);
#ifndef _WIN32_WCE
	printf("Null frame = %d, Other frame = %d\n", nullFrame, unNullFrame);
	printf("Decoder finish!");
#endif
	return 0;
}
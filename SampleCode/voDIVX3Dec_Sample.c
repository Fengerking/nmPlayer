
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
#include		"voDIVX3.h"
//#include		"viMem.h" /* Shared frame buffer operator*/


#define OUTPUT_YUV_DATA  1 		/* Flag to Write YUV data into file*/
#define INPUT_FRAME_NUM        10000	/* default input frames*/


#ifdef _WIN32_WCE
#define INPUT_FILE_NAME        "\\SX313_Video_R2.dat"/*  PHDec.mp4v\\SD Card\\*/
#define OUT_FILE_NAME          "\\out.yuv"
#define OUT_SPEED_FILE_NAME    "\\Speed_f2_new.txt"
#endif


#define IN_BUFF_LEN 1024*1024 
#define IN_BUFF_LEN2  IN_BUFF_LEN/2

FILE *inFile, *outFile, *speedFile, *rgbFile;
int frameNumber, nullFrame, unNullFrame;


int OutputOneFrame(VO_VIDEO_BUFFER *par, VO_VIDEO_FORMAT* outFormat)
{
	int i, width,height;
	unsigned char* out_src;
	
	//printf("Output frame %d type %d\n", frameNumber, outFormat->Type);
	
	//if(outFormat->Type!=2)
	//	return;
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
#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv) 
#endif // _WIN32_WCE
{
	unsigned char		*inputBuf;
	char inFileName[256], outFileName[256], outSpeedName[256], outFileRgb[256];
	double duration;
	int outTotalFrame = INT_MAX;
	VO_U32 nThread = 2;
	int tempLen, leftLen = 0, fileSize = 0, len;

	VO_HANDLE			hCodec;
	VO_U32				returnCode;
	VO_CODECBUFFER		inData;
	VO_VIDEO_BUFFER		outData;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI		voDIVX3Dec;

	VO_U32				width, height;

	//VO_MEM_VIDEO_OPERATOR voVideoMemOperator;


#ifdef _WIN32_WCE
	unsigned char *mb_skip;
	DWORD start, finish;
	outFileName[0] = outSpeedName[0] = '\0';
	sprintf(inFileName, INPUT_FILE_NAME);
	sprintf(outFileName, OUT_FILE_NAME);
	sprintf(outSpeedName, OUT_SPEED_FILE_NAME);
	outTotalFrame = INPUT_FRAME_NUM;
	start = GetTickCount();
#else
	int i;

	unsigned char *mb_skip;
	clock_t start, finish;
	outFileName[0] = outSpeedName[0] = outFileRgb[0] = '\0';

//	freopen( "debug.dat", "wt", stdout );


	if (argc > 1){
		for (i = 1; i < argc; i++){
			if (0 == strncmp (argv[i], "-i", 2)){
				strcpy(inFileName, argv[i+1]);
				printf("\n INPUT: input file: %s", inFileName);
			}
			else if (0 == strncmp (argv[i], "-o", 2)){
				strcpy(outFileName, argv[i+1]);
				printf("\n INPUT: output video YUV420 file: %s", outFileName);
			}
			else if (0 == strncmp (argv[i], "-s", 2)){
				strcpy(outSpeedName, argv[i+1]);
				printf("\n INPUT: test speed file: %s", outSpeedName);
			}
			else if (0 == strncmp (argv[i], "-r", 2)){
				strcpy(outFileRgb, argv[i+1]);
				printf("\n INPUT: output video RGB file: %s", outFileRgb);
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
		printf("Usage: %s [-i inputFileName] {[-f totalFrames] [-o outVideoFileName] [-s outSpeedFileName]} \n",argv[0]);
		exit(0);
	}
	
	start = clock();
#endif
	frameNumber = 0;

	inFile= fopen (inFileName, "rb");
	if (!inFile){
		printf("\nError: cannot open input DIVX3 file!");
		exit(0);
	}

	if (outFileName[0] != '\0')
		outFile = fopen(outFileName, "wb");
	else
		outFile = NULL;

	if (outFileRgb[0] != '\0')
		rgbFile = fopen(outFileRgb, "wb");
	else
		rgbFile = NULL;

	if (!outFile){
		printf("\nWarning: no output video file!");
	}

	if (outSpeedName[0] != '\0')
		speedFile = fopen(outSpeedName, "wb");
	else
		speedFile = NULL;

	if (!speedFile){
		printf("\nWarning: no output speed file!\n");
	}
	printf("\n");

	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	/* allocate input buffer*/
	inputBuf = (char *)malloc(IN_BUFF_LEN * sizeof (char));

	/* get decoder api*/
	returnCode = voGetDIVX3DecAPI(&voDIVX3Dec, 0);
	if(returnCode != VO_ERR_NONE){
		goto End;
	}

	/* initialize decoder handle*/
	returnCode = voDIVX3Dec.Init(&hCodec, VO_VIDEO_CodingDIVX, NULL);
	if(returnCode != VO_ERR_NONE){
		goto End;
	}

	returnCode = voDIVX3Dec.SetParam(hCodec, VO_PID_COMMON_CPUNUM, (VO_PTR)&nThread);

	/* Set width and height of video */
	width  = 176;
	height = 144;
	voDIVX3Dec.SetParam(hCodec, VO_PID_DEC_DIVX3_VIDEO_WIDTH, &width);
	voDIVX3Dec.SetParam(hCodec, VO_PID_DEC_DIVX3_VIDEO_HEIGHT, &height);

	/* Read video raw data*/
	if(!(leftLen = fread(inputBuf, 1, IN_BUFF_LEN, inFile)))
		goto End; 

	/*initialize input structure*/
	inData.Length = leftLen;
	inData.Buffer = inputBuf;

	while((fileSize>0) && (outTotalFrame>0)){

		/* Set frames of raw video data*/
		voDIVX3Dec.SetInputData(hCodec, &inData);

		/* Get one frame decoded data*/
		returnCode = voDIVX3Dec.GetOutputData(hCodec, &outData, &outInfo);

		if(!returnCode){
			frameNumber++;
			outTotalFrame--;
				if(outInfo.Format.Type != VO_VIDEO_FRAME_NULL){
#if OUTPUT_YUV_DATA
					OutputOneFrame(&outData, &outInfo.Format);
#endif
					unNullFrame++;
				}
				else
				{
					nullFrame++;
				}

		}else{
			/*Skip 8 byte, to simulate skip current frame*/
			inData.Length = 8;
		}
		fileSize -= outInfo.InputUsed;
		leftLen -= outInfo.InputUsed;

		/*update input structure*/
		if((fileSize > leftLen) && (leftLen > 0) && (leftLen < IN_BUFF_LEN2)){
			memmove(inputBuf, inputBuf + (IN_BUFF_LEN - leftLen),  leftLen);
			tempLen = fread(inputBuf + leftLen, 1, (IN_BUFF_LEN - leftLen), inFile);
			leftLen = tempLen + leftLen;
			inData.Buffer = inputBuf;
			inData.Length = leftLen;
		}else{
			inData.Buffer += outInfo.InputUsed;
			inData.Length = leftLen;

		}
	}


	/* Get the last decoded frame*/
	voDIVX3Dec.GetParam(hCodec, VO_PID_DEC_DIVX3_GET_LASTVIDEOBUFFER, (VO_PTR*)&outData);
	voDIVX3Dec.GetParam(hCodec, VO_PID_DEC_DIVX3_GET_LASTVIDEOINFO, (VO_PTR*)&outInfo);

	if(outInfo.Format.Type != VO_VIDEO_FRAME_NULL){
#if OUTPUT_YUV_DATA
		OutputOneFrame(&outData, &outInfo.Format);
#endif
		frameNumber++;
		unNullFrame++;
	}else{
		nullFrame++;
	}

	/* free decoder* handle*/
	voDIVX3Dec.Uninit(hCodec);

#ifdef _WIN32_WCE
	finish = GetTickCount();
	duration = (finish - start) * 0.001;
#else
	finish = clock();
	//	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//    printf("\nDIVX3 decoder speed = %2.3f (fps)", frameNumber/duration);
	//    printf("\n\ttotal frame = %d, NULL frame = %d, total time = %2.3f seconds\n", 
	//		frameNumber,  allFrame -frameNumber, duration);
#endif //_WIN32_WCE

	if (speedFile){
//		fprintf(speedFile, "\nDIVX3 decoder speed = %2.3f (fps)", unNullFrame/duration);
//		fprintf(speedFile, "\n\ttotal frame = %d, total time = %2.3f seconds\n", 
//			frameNumber,  duration);
		fclose (speedFile);
	}
End:
	free(inputBuf);
	fclose(inFile);
	if (outFile)
		fclose(outFile);
	printf("Null frame = %d, Other frame = %d\n", nullFrame, unNullFrame);
	printf("Decoder finish!\n");

	return 0;
}


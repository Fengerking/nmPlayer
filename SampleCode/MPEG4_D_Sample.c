
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
#include		"voMPEG4.h"
//#include		"viMem.h" /* Shared frame buffer operator*/


#define OUTPUT_YUV_DATA  1 		/* Flag to Write YUV data into file*/
#define INPUT_FRAME_NUM        10000	/* default input frames*/

#ifdef _WIN32_WCE
#define INPUT_FILE_NAME        "\\SX313_Video_R2.dat"/*  PHDec.mp4v\\SD Card\\*/
#define OUT_FILE_NAME          "\\out.yuv"
#define OUT_SPEED_FILE_NAME    "\\Speed_f2_new.txt"
#endif


#define IN_BUFF_LEN 1024*1024 
FILE *inFile, *outFile, *speedFile, *rgbFile;
int frameNumber, nullFrame, unNullFrame;


int OutputOneFrame(VO_VIDEO_BUFFER *par, VO_VIDEO_FORMAT* outFormat)
{
	int i, width,height;
	unsigned char* out_src;
	
	//printf("Output frame %d type %d\n", frameNumber, outFormat->Type);
	
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

/* vop start code is 0x000001b6  4byte*/
#define MPEG4_SC_LEN 4		
#define LOST_DATA	0
int voSearchMpeg4VOPSC(unsigned char *input, int length)
{
	/*find 0x000001b6 */
	unsigned char *head = input, *end = input + length - MPEG4_SC_LEN;

	do {
		if (head[0]) {/* faster if most of bytes are not zero*/	
			head++;
			continue;
		}
		/* find 00xx*/
		if (head[1]){
			head += 2; // skip 2 bytes;
			continue;
		}
		/* find 0000xx */
		if (head[2] != 0x01){
			if(head[2] == 0){
				head += 1;
			}else{
				head += 3; /* skip 3 bytes */
			}
			continue;
		}
		/* find 000001xx*/
		head += 4; /* update pointer first, since it will be used in all cases below*/
		switch (head[-1]){
		case 0xb6:
			/* find vop*/
			return (int)(head - input) - 4;
		default:
			/* find 0000xx, xx > 1*/
			/* do nothing */
			break;
		}
	} while (head < end);
	/* not finding start code */
	return -1;
}

/* vop start code is 0000 0000 0000 0000 1000 00 22bits*/
#define H263_PSC_LEN 3		
int voSearchH263PSC(unsigned char *input, int length)
{
	/*find 0000 0000 0000 0000 1000 00 22bits*/
	unsigned char *head = input, *end = input + length - H263_PSC_LEN;

	do {
		if (head[0]) {/* faster if most of bytes are not zero*/
			head++;
			continue;
		}
		/* find 0x00xx */
		if (head[1]){
			head += 2; /* skip 2 bytes */
			continue;
		}
		/* find 0x0000xx */
		if ((head[2]&0xfc) != 0x80){
			if(head[2] == 0){
				head += 1;
			}else{
				head += 3; /* skip 3 bytes */
			}
			continue;
		}
		/* find PSC */
		head += 3;
		return (int)(head - input)-H263_PSC_LEN;
	} while (head < end);
	/* not finding start code */
	return -1;
}

#define MPEG4_DEC 0
#define H263_DEC 1

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
	int tempLen, leftLen = 0, fileSize = 0, len;

	VO_HANDLE			hCodec;
	VO_U32				returnCode;
	VO_CODECBUFFER		inData;
	VO_VIDEO_BUFFER		outData;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI		voMpeg4Dec;

	//VO_MEM_VIDEO_OPERATOR voVideoMemOperator;
	int iCodecID = MPEG4_DEC;//H263_DEC;//;


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
		printf("\nError: cannot open input mpeg4 file!");
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


	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	/* allocate input buffer*/
	inputBuf = (char *)malloc(IN_BUFF_LEN * sizeof (char));

	/* get decoder api*/
	returnCode = voGetMPEG4DecAPI(&voMpeg4Dec, 0);
	if(returnCode != VO_ERR_NONE){
		goto End;
	}

	/* initialize decoder handle*/
	returnCode = voMpeg4Dec.Init(&hCodec, VO_VIDEO_CodingMPEG4, NULL);
	if(returnCode != VO_ERR_NONE){
		goto End;
	}

	/* Get API for shared frame buffer operator*/
	//GetVideoMemOperatorAPI(&voVideoMemOperator);
	/* Set shared frame buffer operator*/
	//returnCode =voMpeg4Dec.SetParam(hCodec, VO_PID_VIDEO_VIDEOMEMOP, &voVideoMemOperator);

	/* Read video raw data*/
	if(!(leftLen = fread(inputBuf, 1, IN_BUFF_LEN, inFile)))
		goto End; 

	/*initialize input structure*/
	inData.Length = leftLen;
	inData.Buffer = (VO_PBYTE)inputBuf;


	if(MPEG4_DEC == iCodecID){
		/* Search VOP start code*/
		len = voSearchMpeg4VOPSC(inData.Buffer,leftLen);
		/*set VOL header data*/
		if(len >= 0){
			inData.Length = len;
			returnCode = voMpeg4Dec.SetParam(hCodec, VO_PID_COMMON_HEADDATA, (VO_PTR)&inData);
			if(returnCode != VO_ERR_NONE)
				goto End;
		}else{/*don't find mpeg4 VOP start code*/
			goto End;
		}
		/* Get video width and height*/
		voMpeg4Dec.GetParam(hCodec, VO_PID_DEC_MPEG4_GET_VIDEOFORMAT, (VO_PTR*)&outInfo);
		//printf("Video size %dx%d\n", outInfo.Format.Width, outInfo.Format.Height);


		/*updata input buffer*/
		inData.Buffer += len;	
		leftLen -= len;
	}

	while((fileSize>0) && (outTotalFrame>0)){
		int iPscLen;
		if(MPEG4_DEC == iCodecID){
			iPscLen = MPEG4_SC_LEN;
			len = voSearchMpeg4VOPSC(inData.Buffer + iPscLen, leftLen - iPscLen);/*skip the start code of current frame*/
		}else{
			iPscLen = H263_PSC_LEN;
			len = voSearchH263PSC(inData.Buffer + iPscLen, leftLen - iPscLen);/*skip the start code of current frame*/
		}
		if(len>0){
			/*add the start code of current frame
			/*LOST_DATA: to simulate no enough data input */
			inData.Length = (len + iPscLen) - LOST_DATA;
			//printf("length = %d\n", (len + MPEG4_SC_LEN));
				
			/* Get the frame type of input frame*/
			//returnCode = voMpeg4Dec.GetParam(hCodec, VO_PID_VIDEO_FRAMETYPE, (VO_PTR*)&inData);
			//printf("\n frame type = %d", *((VO_U32*)inData.UserData));

			/* Set frames of raw video data*/
			voMpeg4Dec.SetInputData(hCodec, &inData);
			/* Get one frame decoded data*/
			returnCode = voMpeg4Dec.GetOutputData(hCodec, &outData, &outInfo);


			/* subtract current frame lenth from input buffer */
			fileSize -= (len + iPscLen);
			inData.Buffer += (len + iPscLen);
			leftLen -= (len + iPscLen);
			outTotalFrame--;

			if(returnCode == VO_ERR_NONE){/*No error in current frame*/
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
		}else{/*Maybe there is no enough data, add more data*/
			  memmove(inputBuf, inData.Buffer, leftLen);
			  tempLen = fread(inputBuf + leftLen, 1, (IN_BUFF_LEN - leftLen), inFile);
			  if(tempLen > 0){
				inData.Buffer = (VO_PBYTE)inputBuf;
				leftLen += tempLen;
			  }else{
				  fileSize = 0;
			  }
		}
	}


	/* Get the last decoded frame*/
	voMpeg4Dec.GetParam(hCodec, VO_PID_DEC_MPEG4_GET_LASTVIDEOBUFFER, (VO_PTR*)&outData);
	voMpeg4Dec.GetParam(hCodec, VO_PID_DEC_MPEG4_GET_LASTVIDEOINFO, (VO_PTR*)&outInfo);

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
	voMpeg4Dec.Uninit(hCodec);

#ifdef _WIN32_WCE
	finish = GetTickCount();
	duration = (finish - start) * 0.001;
#else
	finish = clock();
	//	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//    printf("\nMPEG4 decoder speed = %2.3f (fps)", frameNumber/duration);
	//    printf("\n\ttotal frame = %d, NULL frame = %d, total time = %2.3f seconds\n", 
	//		frameNumber,  allFrame -frameNumber, duration);
#endif //_WIN32_WCE

	if (speedFile){
//		fprintf(speedFile, "\nMPEG4 decoder speed = %2.3f (fps)", unNullFrame/duration);
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
	printf("Decoder finish!");


	return 0;
}


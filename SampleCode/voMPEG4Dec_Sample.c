
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
//#include "voMpeg4Dec.h"
//#include		"viMem.h" /* Shared frame buffer operator*/


#define OUTPUT_YUV_DATA  1 		/* Flag to Write YUV data into file*/
#define INPUT_FRAME_NUM        100000	/* default input frames*/

#ifdef _WIN32_WCE
#define INPUT_FILE_NAME        "\\SX313_Video_R2.dat"/*  PHDec.mp4v\\SD Card\\*/
#define OUT_FILE_NAME          "\\out.yuv"
#define OUT_SPEED_FILE_NAME    "\\Speed_f2_new.txt"
#endif


#define IN_BUFF_LEN 1024*1024 
FILE *inFile, *outFile, *speedFile, *rgbFile;
int frameNumber, nullFrame, unNullFrame;

#define ENABLE_TIME_ST 0
#if ENABLE_TIME_ST
extern double start0,start1,start2,start3,end0,end1,end2,end3;
extern double tST0, tST1, tST2, tST3;
#ifndef MARKTIME

#ifdef WIN32

#include <windows.h>
#include <winbase.h>
#include <time.h>

static LARGE_INTEGER tmp1,tmp2;
#define MARKTIME(t)	\
	(QueryPerformanceCounter(&tmp2),t = tmp2.QuadPart/(double)tmp1.QuadPart)

#define MARKTIME_INIT(t)	\
	(QueryPerformanceFrequency(&tmp1),MARKTIME(t))

#else

#include		<time.h>

static struct timeval tval;
#define MARKTIME(t)	\
	(gettimeofday(&tval, NULL), t=tval.tv_sec + tval.tv_usec/1000000.0)

#define MARKTIME_INIT(t)	MARKTIME(t)

#endif

#endif

#endif

typedef struct
{
    VO_U32 nBuf;
    VO_S32 nPos;
    VO_S32 nLen;
    VO_U16 *pEnd;
}
BITSTREAM;

static VO_S32 volog2(VO_U32 nData)
{
	VO_S32 i;

	if (!nData) ++nData;
	for (i = 0;nData; ++i)
		nData >>= 1;

    return i;
}


void InitBits(BITSTREAM *pBS, VO_U8 *pBuf, VO_U32 nLen)
{
	pBS->pEnd = (VO_U16 *)pBuf;
	pBS->nLen = nLen;
	pBS->nBuf = (*pBS->pEnd++) << 16;
	pBS->nPos = 16;
}


void  FlushBits(BITSTREAM *pBS, VO_U32 nBits)  
{
	pBS->nBuf <<= nBits;
	pBS->nPos -= nBits;
	
	if((pBS->nPos - 16) <= 0){
		VO_U16 nTmp = 0;
		
		
		if(pBS->nLen < 2){
			return;
		}else{	
			pBS->nLen -= 2;
			nTmp = *pBS->pEnd++;
			
			pBS->nBuf |= (nTmp << (16 - pBS->nPos));
			pBS->nPos += 16;
		}
	}

}


VO_U32 ReadBits(BITSTREAM *pBS, VO_U32 nBits)
{
	VO_S32 nPos = 0;
	VO_U32 nBuf;
	VO_U16 nTmp;
	
	
	nBuf = pBS->nBuf >> (32 - nBits);

	pBS->nBuf <<= nBits;
	pBS->nPos -= nBits;
	
	nPos = 16 - pBS->nPos;	
	
	if(nPos < 0){
		return nBuf;
	}else{
		if(pBS->nLen < 2){
			return nBuf;
		}else{
			pBS->nLen -= 2;
			nTmp = *pBS->pEnd++;
			pBS->nBuf |= nTmp << nPos;
			pBS->nPos += 16;
			return nBuf;
		}
	}
}

void ParserVOL(BITSTREAM *pBS)
{
	VO_S32 nTimeIncResolution;
	VO_S32 nTimeIncBits;
	VO_S32 nWidth, nHeight;
	VO_S32 bInterlace;

	FlushBits(pBS, 16); // start_code
	FlushBits(pBS, 16); // start_code
	FlushBits(pBS, 1+8); // random_accessible_vol + video_object_type_indication



	if (ReadBits(pBS, 1)) {//is_object_layer_identifier
		FlushBits(pBS, 4);//visual_object_layer_verid
		FlushBits(pBS, 3);//video_object_layer_priority
	}
 
	if (15 == ReadBits(pBS, 4)){// aspect ratio
		FlushBits(pBS, 8); //aspect_width
		FlushBits(pBS, 8); //aspect_height
	}

	if (ReadBits(pBS, 1)){ // vol control parameters
		FlushBits(pBS, 2); // chroma_format
		FlushBits(pBS, 1); // b-frames
		if (ReadBits(pBS, 1)){ // vbv parameters
			FlushBits(pBS,16);
			FlushBits(pBS,16);
			FlushBits(pBS,16);
			FlushBits(pBS,15);
			FlushBits(pBS,16);
		}
	}


	FlushBits(pBS, 2+1); // shape + marker

	nTimeIncResolution = ReadBits(pBS, 16);
	if (nTimeIncResolution <= 0)
		nTimeIncResolution = 1;
	nTimeIncBits = volog2(nTimeIncResolution - 1);

	FlushBits(pBS, 1); // marker
	if (ReadBits(pBS, 1)) //fixed_vop_rate
		FlushBits(pBS, nTimeIncBits);

	FlushBits(pBS, 1); // marker
	nWidth = ReadBits(pBS,13); //nWidth

	FlushBits(pBS ,1);// marker
	nHeight = ReadBits(pBS,13); //nHeight

	FlushBits(pBS,1); // marker
//	bInterlaced = ReadBits(pBS,1);//interlace

}


int OutputOneFrame(VO_VIDEO_BUFFER *par, VO_VIDEO_FORMAT* outFormat)
{
	int i, width,height;
	unsigned char* out_src;
	
	printf("Output frame %d type %d\n", frameNumber, outFormat->Type);
	
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

	//printf("input buffer address = 0x%x, length = %d\n", input, length);

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

int voSearchMpeg4VOLSC(unsigned char *input, int length)
{
	/*find 0x000001b6 */
	unsigned char *head = input, *end = input + length - MPEG4_SC_LEN;

	//printf("input buffer address = 0x%x, length = %d\n", input, length);

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
		case 0x20:
			/* find voL*/
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
int voSearchH263PSC(unsigned char *input, int length, unsigned int bIsS263)
{
	/*find 0000 0000 0000 0000 1000 00 22bits*/
	unsigned char *head = input, *end = input + length - H263_PSC_LEN;
	unsigned char scFlag = 0xFC;
	if (bIsS263)
	{
		scFlag = 0x80;
	}
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
		if ((head[2]&scFlag) != 0x80){
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

void GetDefaultVOLHeader(VO_CODECBUFFER* codec_buf, VO_U32 width, VO_U32 height)
{
	VO_U8 default_vol_header[28] = {
	0x00, 0x00, 0x01, 0xB0, 0x08, 0x00, 0x00, 0x01,
	0xB5, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
	0x01, 0x20, 0x00, 0x84, 0x40, 0x03, 0xE8, 0x00, 
	0x20, 0x00, 0xA2, 0x1F};

	VO_U32 w_h = (width << 14) | height;//27 bits

	default_vol_header[22] |= (VO_U8)(w_h>>24);//3bits
	default_vol_header[23] |= (VO_U8)(w_h>>16);//8btis
	default_vol_header[24] |= (VO_U8)(w_h>>8);//8btis
	default_vol_header[25] |= (VO_U8)(w_h);//8btis

	memcpy(codec_buf->Buffer, default_vol_header, 28);
	codec_buf->Length = 28;

	
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
	int outTotalFrame = INPUT_FRAME_NUM;
	int tempLen, leftLen = 0, fileSize = 0, len;
	
	VO_U8 defaut_header[128];
	VO_CODECBUFFER header;

	VO_HANDLE			hCodec;
	VO_U32				returnCode;
	VO_CODECBUFFER		inData;
	VO_VIDEO_BUFFER		outData;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI		voMpeg4Dec;

	VO_VIDEO_CODINGTYPE iCodecID = VO_VIDEO_CodingMPEG4;//VO_VIDEO_CodingH263;//VO_VIDEO_CodingMPEG4;//VO_VIDEO_CodingS263;//H263_DEC;//;
	VO_U32 nThread = 1;

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

// 	freopen( "debug.dat", "wt", stdout );

	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	/* Allocate input buffer*/
	inputBuf = (char *)malloc(IN_BUFF_LEN * sizeof (char));

	/* Get decoder api*/
	returnCode = yyGetMPEG4DecFunc(&voMpeg4Dec, 0);
	if(returnCode != VO_ERR_NONE){
		goto End;
	}

	/* Initialize decoder handle*/
	returnCode = voMpeg4Dec.Init(&hCodec, iCodecID, NULL);
	if(returnCode != VO_ERR_NONE){
		goto End;
	}

	returnCode = voMpeg4Dec.SetParam(hCodec, VO_PID_COMMON_CPUNUM, (VO_PTR)&nThread);
// 	if(returnCode != VO_ERR_NONE)
// 		goto End;

	///* Get API for shared frame buffer operator*/
	//GetVideoMemOperatorAPI(&voVideoMemOperator);
	///* Set shared frame buffer operator*/
	////returnCode =voMpeg4Dec.SetParam(hCodec, VO_PID_VIDEO_VIDEOMEMOP, &voVideoMemOperator);

	/* Read conpressed video data*/
	if(!(leftLen = fread(inputBuf, 1, IN_BUFF_LEN, inFile)))
		goto End; 

	/*Fill input buffer*/
	inData.Length = leftLen;
	inData.Buffer = (VO_PBYTE)inputBuf;


	/* Set VOL header data for mpeg4*/
	if(VO_VIDEO_CodingMPEG4 == iCodecID){
#if 1
		/* Search VOP start code*/
		len = voSearchMpeg4VOPSC(inData.Buffer,leftLen);
		/*Call ID VO_PID_COMMON_HEADDATA to set VOL header data*/
		if(len >= 0){
		//	BITSTREAM bs;
			

			inData.Length = len;
			//printf("VOL length = %d \n", len);

		//	InitBits(&bs, inData.Buffer, inData.Length);

		//	ParserVOL(&bs);
			returnCode = voMpeg4Dec.SetParam(hCodec, VO_PID_COMMON_HEADDATA, (VO_PTR)&inData);
			if(returnCode != VO_ERR_NONE)
				goto End;
		}else{/*Don't find mpeg4 VOP start code*/
			goto End;
		}
#else
		header.Buffer = defaut_header;
		GetDefaultVOLHeader(&header, 1280, 720);
		returnCode = voMpeg4Dec.SetParam(hCodec, VO_PID_COMMON_HEADDATA, (VO_PTR)&header);
		if(returnCode != VO_ERR_NONE)
			goto End;
#endif

		/* Get video width and height*/
		voMpeg4Dec.GetParam(hCodec, VO_PID_DEC_MPEG4_GET_VIDEOFORMAT, (VO_PTR*)&outInfo);
		//printf("Video size %dx%d\n", outInfo.Format.Width, outInfo.Format.Height);

		///*Updata input buffer*/
		inData.Buffer += len;	
		leftLen -= len;
	}
#if ENABLE_TIME_ST
// 	MARKTIME_INIT(start1);
// 	MARKTIME_INIT(start3);
#endif

	/* Main loop for decoding frames*/
	while((fileSize>0) && (outTotalFrame>0)){
		int iPscLen;
		/* Search picture header*/
		if(VO_VIDEO_CodingMPEG4 == iCodecID){
			iPscLen = MPEG4_SC_LEN;
			len = voSearchMpeg4VOPSC(inData.Buffer + iPscLen, leftLen - iPscLen);/*skip the start code of current frame*/
		}else{
			iPscLen = H263_PSC_LEN;
			len = voSearchH263PSC(inData.Buffer + iPscLen, leftLen - iPscLen, VO_VIDEO_CodingS263 == iCodecID);/*skip the start code of current frame*/
		}
		if (len <= 0 && leftLen > 0 && feof(inFile))
		{
			len = leftLen - iPscLen;
		}
		if(len>0){


			/*LOST_DATA: To simulate no enough data input */
			inData.Length = (len + iPscLen) - LOST_DATA;
// 			printf("frame = %d, length = %d\n", frameNumber+1, (len + iPscLen));
				
			/* Get the frame type of input frame*/
			//returnCode = voMpeg4Dec.GetParam(hCodec, VO_PID_VIDEO_FRAMETYPE, (VO_PTR*)&inData);

// 			if(frameNumber==154)
// 				frameNumber=154;
			/* Set frames of compressed video data*/
			voMpeg4Dec.SetInputData(hCodec, &inData);

			/* Get one frame decoded data, outInfo.InputUsed returens the used length of input buffer*/
			returnCode = voMpeg4Dec.GetOutputData(hCodec, &outData, &outInfo);

			/* Subtract current frame lenth from input buffer */
			fileSize -= (len + iPscLen);
			inData.Buffer += (len + iPscLen);
			leftLen -= (len + iPscLen);
			outTotalFrame--;

			if(returnCode == VO_ERR_NONE){/*No error in current frame*/
				if(outInfo.Format.Type != VO_VIDEO_FRAME_NULL){
#if OUTPUT_YUV_DATA
// if (frameNumber == 247)
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
			}else{
				frameNumber++;
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

	/* Free decoder handle*/
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
	printf("Decoder finish!\n");

#if ENABLE_TIME_ST
	printf("VOP time=%fs\n", tST0);
	printf("MB wait time=%f\n", tST1);
	printf("Frame wait time=%f\n", tST2);
	printf("Frame ready time=%f\n", tST3);
#endif
	return 0;
}


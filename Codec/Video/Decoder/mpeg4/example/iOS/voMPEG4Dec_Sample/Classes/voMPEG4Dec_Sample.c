
#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<limits.h>

#include		"voMPEG4.h"

/// config input & output ///////////////////////////////////
const int outTotalFrame = INT_MAX;
const int nCPUNum = 1;
const int iCodecID = VO_VIDEO_CodingMPEG4; //VO_VIDEO_CodingH263; VO_VIDEO_CodingS263
const char *inFileName = "test.mpeg4";
const char *outFileName = "test.yuv";
/// end of config ///////////////////////////////////////////

#define IN_BUFF_LEN 1024*1024 
FILE *inFile = NULL, *outFile = NULL;
int nullFrame, unNullFrame;

int OutputOneFrame(VO_VIDEO_BUFFER *par, VO_VIDEO_FORMAT* outFormat)
{
	int i, width,height;
	unsigned char* out_src;
	
	printf("Output frame %d type %d\n", unNullFrame, outFormat->Type);
	
	if (!outFile)
	{
		return 0;
	}
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
int voSearchH263PSC(unsigned char *input, int length, unsigned int bIsS263)
{
	/*find 0000 0000 0000 0000 1000 00 22bits*/
	unsigned char *head = input, *end = input + length - H263_PSC_LEN;
	unsigned char scFlag = 0xFC;
	if (bIsS263) {
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

int beginTest(const char *workDir)
{
	unsigned char		*inputBuf;
	char inFilePath[256], outFilePath[256];
	int tempLen, leftLen = 0, fileSize = 0, len;

	VO_HANDLE			hCodec;
	VO_U32				returnCode;
	VO_CODECBUFFER		inData;
	VO_VIDEO_BUFFER		outData;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI		voMpeg4Dec;

	unNullFrame = 0;
	nullFrame = 0;
	
	strcpy(inFilePath, workDir);
	strcat(inFilePath, "/");
	strcat(inFilePath, inFileName);
	inFile= fopen (inFilePath, "rb");
	if (!inFile){
		printf("\nError: cannot open input mpeg4 file: %s\n", inFilePath);
		return -1;
	}
	printf("file path: %s\n", inFilePath);
	
	if (strlen(outFileName) > 0)
	{
		strcpy(outFilePath, workDir);
		strcat(outFilePath, "/");
		strcat(outFilePath, outFileName);
		outFile = fopen(outFilePath, "wb");
	}
	else
		outFile = NULL;
	
	if (!outFile){
		printf("\nWarning: no output video file!\n");
	}

	/* get decoder api*/
	returnCode = voGetMPEG4DecAPI(&voMpeg4Dec, 0);
	if(returnCode != VO_ERR_NONE){
		printf("Error: cannot get MPEG4 decoder API\n");
		return -1;
	}
	
	/* initialize decoder handle*/
	returnCode = voMpeg4Dec.Init(&hCodec, iCodecID, NULL);
	if(returnCode != VO_ERR_NONE){
		printf("Error: cannot init the decoder\n");
		return -1;
	}

	/* allocate input buffer*/
	inputBuf = (unsigned char *)malloc(IN_BUFF_LEN * sizeof (char));
	if (inputBuf == NULL) {
		printf("Error: cannot allocate input buffer, size %d\n", IN_BUFF_LEN);
		voMpeg4Dec.Uninit(hCodec);
		return -1;
	}
	
	returnCode = voMpeg4Dec.SetParam(hCodec, VO_PID_COMMON_CPUNUM, (VO_PTR)&nCPUNum);
	if(returnCode != VO_ERR_NONE)
		goto End;
	
	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);
	printf("fileSize:%d\n", fileSize);

	/* Read video raw data*/
	if(!(leftLen = fread(inputBuf, 1, IN_BUFF_LEN, inFile)))
		goto End; 
	
	/*initialize input structure*/
	inData.Length = leftLen;
	inData.Buffer = (VO_PBYTE)inputBuf;
	
	while((fileSize>0) && (outTotalFrame > unNullFrame)){
		int iPscLen;
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
			/*add the start code of current frame*/
			inData.Length = len + iPscLen;
			//printf("length = %d\n", (len + MPEG4_SC_LEN));
			
			/* Set frames of raw video data*/
			voMpeg4Dec.SetInputData(hCodec, &inData);
			/* Get one frame decoded data*/
			
			returnCode = voMpeg4Dec.GetOutputData(hCodec, &outData, &outInfo);
			
			/* subtract current frame lenth from input buffer */
			fileSize -= (len + iPscLen);
			inData.Buffer += (len + iPscLen);
			leftLen -= (len + iPscLen);
			
			if(returnCode == VO_ERR_NONE){/*No error in current frame*/
				if(outInfo.Format.Type != VO_VIDEO_FRAME_NULL){
					if (outFile)
					{
						OutputOneFrame(&outData, &outInfo.Format);
					}
					unNullFrame++;
				}else{
					if(unNullFrame > 0){
						nullFrame++;
					}
				}
			}else{
				printf("return error: 0x%X\n", returnCode);
			}
			
		}else{/*Maybe there is no enough data, add more data*/
			
			memmove(inputBuf, inData.Buffer, leftLen);
			tempLen = fread(inputBuf + leftLen, 1, (IN_BUFF_LEN - leftLen), inFile);
			if(tempLen > 0){
				inData.Buffer = (VO_PBYTE)inputBuf;
				leftLen += tempLen;
			}else{
				fileSize = 0;
				printf("End of file\n");
			}
		}
	}
	
	printf("\n last frame");
	/* Get the last decoded frame*/
	voMpeg4Dec.GetParam(hCodec, VO_PID_DEC_MPEG4_GET_LASTVIDEOBUFFER, (VO_PTR*)&outData);
	voMpeg4Dec.GetParam(hCodec, VO_PID_DEC_MPEG4_GET_LASTVIDEOINFO, (VO_PTR*)&outInfo);
	
	if(outInfo.Format.Type != VO_VIDEO_FRAME_NULL){
		if (outFile)
		{
			OutputOneFrame(&outData, &outInfo.Format);
		}
		
		unNullFrame++;
	}else{
		if(unNullFrame > 0){
			nullFrame++;
		}
	}

End:	
	/* free decoder* handle*/
	voMpeg4Dec.Uninit(hCodec);

	free(inputBuf);
	fclose(inFile);
	inFile = NULL;
	if (outFile)
		fclose(outFile);
	outFile = NULL;
	printf("\nNull frame = %d, Other frame = %d\n", nullFrame, unNullFrame);
	printf("Decoder finish!\n");
	
	return 0;
}


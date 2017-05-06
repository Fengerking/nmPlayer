
#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<limits.h>

#include		"voDIVX3.h"

/// config input & output ///////////////////////////////////
const int outTotalFrame = INT_MAX;
const int nCPUNum = 2;		//1:single-core 2:dual-core
const int iCodecID = VO_VIDEO_CodingDIVX;
const char *inFileName = "test1_320x240.divx3"; //"test2_176x144.divx3";
const char *outFileName = "test1_320x240.yuv"; //"test2_176x144.yuv";
const VO_U32 uWidth = 320; //176;
const VO_U32 uHeight = 240; //144;
/// end of config ///////////////////////////////////////////

#define IN_BUFF_LEN 1024*1024 
#define IN_BUFF_LEN2 IN_BUFF_LEN/2 

FILE *inFile = NULL, *outFile = NULL;
int nullFrame, unNullFrame;

int OutputOneFrame(VO_VIDEO_BUFFER *par, VO_VIDEO_FORMAT* outFormat)
{
	int i, width,height;
	unsigned char* out_src;
	
	printf("Output frame %d type %d\n", unNullFrame, outFormat->Type);
	
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

int beginTest(const char *workDir)
{
	unsigned char		*inputBuf;
	char inFilePath[256], outFilePath[256];
	int tempLen, leftLen = 0, fileSize = 0;

	VO_HANDLE			hCodec;
	VO_U32				returnCode;
	VO_CODECBUFFER		inData;
	VO_VIDEO_BUFFER		outData;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI		voDIVX3Dec;

	unNullFrame = 0;
	nullFrame = 0;
	
	strcpy(inFilePath, workDir);
	strcat(inFilePath, "/");
	strcat(inFilePath, inFileName);
	inFile= fopen (inFilePath, "rb");
	if (!inFile){
		printf("\nError: cannot open input divx3 file: %s\n", inFilePath);
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
	returnCode = voGetDIVX3DecAPI(&voDIVX3Dec, 0);
	if(returnCode != VO_ERR_NONE){
		printf("Error: cannot get Divx3 decoder API\n");
		return -1;
	}

	/* initialize decoder handle*/
	returnCode = voDIVX3Dec.Init(&hCodec, iCodecID, NULL);
	if(returnCode != VO_ERR_NONE){
		printf("Error: cannot init the decoder\n");
		return -1;
	}

	/* allocate input buffer*/
	inputBuf = (unsigned char *)malloc(IN_BUFF_LEN * sizeof (char));
	if (inputBuf == NULL) {
		printf("Error: cannot allocate input buffer, size %d\n", IN_BUFF_LEN);
		voDIVX3Dec.Uninit(hCodec);
		return -1;
	}
	
	returnCode = voDIVX3Dec.SetParam(hCodec, VO_PID_COMMON_CPUNUM, (VO_PTR)&nCPUNum);
	if(returnCode != VO_ERR_NONE)
		goto End;
	
	/* Set width and height of video */
	voDIVX3Dec.SetParam(hCodec, VO_PID_DEC_DIVX3_VIDEO_WIDTH, &uWidth);
	voDIVX3Dec.SetParam(hCodec, VO_PID_DEC_DIVX3_VIDEO_HEIGHT, &uHeight);

	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);
	printf("fileSize:%d\n", fileSize);

	/* Read video raw data*/
	if(!(leftLen = fread(inputBuf, 1, IN_BUFF_LEN, inFile)))
		goto End; 

	/*initialize input structure*/
	inData.Length = leftLen;
	inData.Buffer = inputBuf;

	while((fileSize>0) && (outTotalFrame > unNullFrame)){

		/* Set frames of raw video data*/
		voDIVX3Dec.SetInputData(hCodec, &inData);

		/* Get one frame decoded data*/
		returnCode = voDIVX3Dec.GetOutputData(hCodec, &outData, &outInfo);

		if(!returnCode){
			if(outInfo.Format.Type != VO_VIDEO_FRAME_NULL){
				if (outFile) {
					OutputOneFrame(&outData, &outInfo.Format);					
				}
				unNullFrame++;
			}
			else {
				if (unNullFrame > 0) {
					nullFrame++;
				}
			}
		}else{
			printf("return error: 0x%X\n", returnCode);
			/*Skip 8 byte, to simulate skip current frame*/
			outInfo.InputUsed = 8;
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

	printf("\n last frame");
	/* Get the last decoded frame*/
	voDIVX3Dec.GetParam(hCodec, VO_PID_DEC_DIVX3_GET_LASTVIDEOBUFFER, (VO_PTR*)&outData);
	voDIVX3Dec.GetParam(hCodec, VO_PID_DEC_DIVX3_GET_LASTVIDEOINFO, (VO_PTR*)&outInfo);

	if(outInfo.Format.Type != VO_VIDEO_FRAME_NULL){
		if (outFile) {
			OutputOneFrame(&outData, &outInfo.Format);		
		}
		unNullFrame++;
	}else{
		if (unNullFrame > 0) {
			nullFrame++;		
		}
	}
	
End:
	/* free decoder* handle*/
	voDIVX3Dec.Uninit(hCodec);

	free(inputBuf);
	fclose(inFile);
	if (outFile)
		fclose(outFile);
	printf("\nNull frame = %d, Other frame = %d\n", nullFrame, unNullFrame);
	printf("Decoder finish!");

	return 0;
}


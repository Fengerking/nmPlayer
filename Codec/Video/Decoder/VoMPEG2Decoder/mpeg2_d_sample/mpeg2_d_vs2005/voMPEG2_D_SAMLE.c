//#define PPC2005
//#define PPC2003
#ifdef PPC2005
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<time.h>
#include		<limits.h>
#include        <windows.h>

#include		"voMPEG2.h"

#include <Windows.h>
//#include "irdetoCallbacks.h"

#define IN_BUFF_LEN 1024*1024*2
#define IN_BUFF_LEN2  IN_BUFF_LEN/2
FILE *inFile, *outFile, *speedFile, *rgbFile;
int frameNumber, nullFrame, unNullFrame;

static void MakeDataDirty(VO_CODECBUFFER *inBuffer,int nErrorSize,int randomErrorDataRate)
{
	int	nErrorPos = 0;
	VO_U8 vu;
	int i;

	if(inBuffer->Length== 0xffffffff)
		return;

	
	nErrorSize = inBuffer->Length * nErrorSize / 1000;
	//printf(" for nErrorSize %d; inBuffer.Length =%d\r\n", nErrorSize , inBuffer.Length );
#ifdef _WIN32
	srand(GetTickCount());
#else
	srand(time(NULL));
#endif
	for (i = 0; i < nErrorSize; i++)
	{
		nErrorPos = rand () % inBuffer->Length;
		vu = inBuffer->Buffer[nErrorPos];
		
		inBuffer->Buffer[nErrorPos] = rand () % 256;
		if(nErrorPos<0)
			printf(" vu %d; 222 =%d; %d;--;inBuffer.Length=%d\r\n", vu , inBuffer->Buffer[nErrorPos], nErrorPos,inBuffer->Length );
	}
}

int OutputOneFrame(VO_VIDEO_BUFFER *par,VO_VIDEO_FORMAT* outFormat)
{
	int i, width,height;
	unsigned char* out_src;
	printf("Output frame %d type %d\n", frameNumber, outFormat->Type);
	printf("Width = %d  Height = %d\n", outFormat->Width, outFormat->Height);
	if (!outFile)
		return 0;

	width = outFormat->Width;
	height= outFormat->Height;
	printf("Output YUV data of frame %d\n", frameNumber);
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


#if defined (PPC2005)
int _tmain(int argc, _TCHAR* argv[])
#elif defined (PPC2003)
int WinMain(int argc, char **argv)
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv) 
#endif // _WIN32_WCE
{
	char *inputBuf, *rgbbuf;
	VO_HANDLE			hCodec;
	VO_U32				returnCode;
	VO_CODECBUFFER		inData;
	VO_VIDEO_BUFFER		outData;
	VO_VIDEO_FORMAT 	outFormat;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI		voMpeg2Dec;

	int ds_flage;
	int aspectRatio;
	int timeSampleTest = 0;
	int timeSampeOut = 0;

	char inFileName[256], outFileName[256], outSpeedName[256];
	double duration;
	int outTotalFrame = 4000;
	int tempLen, leftLen = 0, fileSize = 0,len;
	int in_width, in_height;
	int header = 1;
	float frame_rate;
	unsigned int j;

	//	RTTYPE rt;
	//	CC_HND hnd;
#if defined (PPC2005)
	unsigned char *mb_skip;
	DWORD start, finish;
	outFileName[0] = outSpeedName[0] = '\0';
	//sprintf(inFileName, "\\SDMMC\\football.mpeg2");
	//sprintf(outFileName, "outYUV.yuv");
	//sprintf(inFileName, "\\Memory Card\\football.mpeg2");
	//sprintf(outFileName, "\\Memory Card\\outYUV.yuv");
	sprintf(inFileName, "\\Storage Card\\football.mpeg2");
	sprintf(outFileName, "\\Storage Card\\outYUV.yuv");
	sprintf(outSpeedName, "MP2decSpeed.txt");
	outTotalFrame = 20;
	start = GetTickCount();
#elif defined(PPC2003)
	double start, finish;
	outFileName[0] = outSpeedName[0] = '\0';
	//sprintf(inFileName, "\\Storage Card\\football.mpeg2");
	//sprintf(outFileName, "\\Storage Card\\outYUV.yuv");
	//sprintf(inFileName, "football.mpeg2");
	//sprintf(outFileName, "outYUV.yuv");
	//sprintf(inFileName, "\\SDMMC\\football.mpeg2");
	//sprintf(outFileName, "\\SDMMC\\outYUV.yuv");
	//sprintf(outSpeedName, "MP2decSpeed.txt");
	sprintf(inFileName, "\\SDMMC\\1_MPEGV_MPEGV1_994K'40s960ms'352x288'25f.m1v");
	sprintf(outFileName, "\\SDMMC\\outYUV.yuv");
	sprintf(outSpeedName, "MP2decSpeed.txt");
	outTotalFrame = 10;
	start = GetTickCount();
#else
	int i;
	unsigned char *mb_skip;
	clock_t start, finish;
	outFileName[0] = outSpeedName[0] = '\0';
#if 1
	if (argc > 1){
		for (i = 1; i < argc; i++){
			if (0 == strncmp (argv[i], "-i", 2)){
				strcpy(inFileName, argv[i+1]);
				printf("\n INPUT: input file: %s", inFileName);
			}
			else if (0 == strncmp (argv[i], "-o", 2)){
				strcpy(outFileName, argv[i+1]);
				printf("\n INPUT: output video file: %s", outFileName);
			}
			else if (0 == strncmp (argv[i], "-s", 2)){
				strcpy(outSpeedName, argv[i+1]);
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
		printf("Usage: %s [-i inputFileName] {[-f totalFrames] [-o outVideoFileName] [-s outSpeedFileName]} \n",argv[0]);
		exit(0);
	}
#else
	//sprintf(inFileName, "H:\\test\\mpeg2\\clips\\football.mpeg2");
	//sprintf(outFileName, "H:\\test\\mpeg2\\clips\\outYUV.yuv");
	//sprintf(outSpeedName, "H:\\test\\mpeg2\\clips\\MP2decSpeed.txt");
	sprintf(inFileName, "E:\\test\\testbase\\football.mpeg2");
	sprintf(outFileName, "E:\\outYUV.yuv");
	sprintf(outSpeedName, "E:\\test\\testbase\\MP2decSpeed.txt");
#endif
	start = clock();
#endif
	frameNumber = 0;

	inFile= fopen (inFileName, "rb");
	if (!inFile){
		printf("\nError: cannot open input MPEG2 file!");
		exit(0);
	}

	if (outFileName[0] != '\0')
		outFile = fopen(outFileName, "wb");
	else
		outFile = NULL;

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

	ds_flage = 0;

	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	// allocate input buffer;
	inputBuf = (char *)malloc(IN_BUFF_LEN * sizeof (char));

	/* get decoder api*/
	returnCode = voGetMPEG2DecAPI(&voMpeg2Dec, 0);

	if(returnCode != VO_ERR_NONE){
		goto End;
	}

	returnCode = voMpeg2Dec.Init(&hCodec, VO_VIDEO_CodingMPEG2, NULL);

	if(returnCode != VO_ERR_NONE){
		goto End;
	}

	voMpeg2Dec.SetParam(hCodec,VO_PID_DEC_MPEG2_DOWMSAMPLE,(VO_PTR)&ds_flage);
	{
		int huwei = 0;
		voMpeg2Dec.SetParam(hCodec,VO_PID_VIDEO_OUTPUTMODE,(VO_PTR)&huwei);

	}
	

	if(!(leftLen = fread(inputBuf, 1, IN_BUFF_LEN, inFile)))
		goto End; 

	/*initialize input structure*/
	inData.Length = leftLen;
	inData.Buffer = (VO_PBYTE)inputBuf;

	while((fileSize > 0) && (outTotalFrame > 0)){
		//set parameters
		int iPscLen;

		if(header){

			returnCode = voMpeg2Dec.SetParam(hCodec, VO_PID_COMMON_HEADDATA, (VO_PTR)&inData);

			if(returnCode != VO_ERR_NONE)
				goto End;

			voMpeg2Dec.GetParam(hCodec, VO_PID_DEC_MPEG2_GET_VIDEOFORMAT, (VO_PTR*)&outFormat);
			voMpeg2Dec.GetParam(hCodec, VO_PID_DEC_MPEG2_ASPECT_RATIO,(VO_PTR *)&aspectRatio);
			voMpeg2Dec.GetParam(hCodec, VO_PID_DEC_MPEG2_GET_FRAME_RATE, (VO_PTR*)&frame_rate);
			printf("Video size %dx%d\n", outFormat.Width, outFormat.Height);
			header = 0;
			//{
			//	HMODULE					m_hDll;
			//	DRMclientData           DRMData;
			//	m_hDll = LoadLibrary("E:\\Work\\trunk\\Codec\\Video\\Decoder_bak\\FixerCallback\\dll\\irdeto\\Debug\\irdeto.dll");
			//	if (m_hDll == NULL)
			//		return 0;
			//	DRMData.vendorID = IRDETO_DRM;
			//	DRMData.vendorDRMtype = IDCT_DRM;
			//	DRMData.callback1 = (VO_PTR*)GetProcAddress (m_hDll, "irdeto_postIDCTCallback");

			//	voMpeg2Dec.SetParam(hCodec, VO_PID_VIDEO_DRM_FUNC, (VO_PTR*)(&DRMData));
			//}

		}else{
			//inData.Length = leftLen;
			//inData.Time  = timeSampleTest;
			//returnCode =  voMpeg2Dec.SetInputData(hCodec, &inData);
			//timeSampleTest++;//huwei 20090701 test
			//voMpeg2Dec.GetOutputData(hCodec, &outData, &outInfo);
			//voMpeg2Dec.GetParam(hCodec,VO_PID_VIDEO_FRAMETYPE, (VO_PTR *)&timeSampeOut);

			//{
			//	VO_CODECBUFFER		inData2;
			//	inData2.Buffer = inData.Buffer;
			//	inData2.Length = inData.Length;
			//	MakeDataDirty(&inData2,80,0);

			//}
			
			inData.Length = leftLen;
			returnCode = voMpeg2Dec.SetInputData(hCodec, &inData);			
			returnCode = voMpeg2Dec.GetOutputData(hCodec, &outData, &outInfo);

			outFormat =outInfo.Format;
			if(returnCode == VO_ERR_NONE){
				outTotalFrame--;

				if(outFormat.Type != VO_VIDEO_FRAME_NULL){
					OutputOneFrame(&outData, &outFormat);
					frameNumber++;
					unNullFrame++;
				}else{
					if(frameNumber){
						printf("Null frame %d\n", frameNumber);
						nullFrame++;
					}
					//frameNumber++;
				}
			}else{
				/*skip 8 byte*/
				inData.Length = 8;
			}

			fileSize -= outInfo.InputUsed;
			leftLen  -= outInfo.InputUsed;

			/*update input structure*/
			if((fileSize > leftLen) && (leftLen > 0) && (leftLen < IN_BUFF_LEN2)){
				memmove(inputBuf, inputBuf + (IN_BUFF_LEN - leftLen),  leftLen);
				tempLen = fread(inputBuf + leftLen, 1, (IN_BUFF_LEN - leftLen), inFile);
				leftLen = tempLen + leftLen;
				inData.Buffer = inputBuf;
			}else{
				//inData.Buffer += inData.Length;
				inData.Buffer += outInfo.InputUsed;

			}		
		}
	}

	voMpeg2Dec.GetParam(hCodec, VO_PID_DEC_MPEG2_GETLASTOUTVIDEOBUFFER, (VO_PTR*)&outData);
	voMpeg2Dec.GetParam(hCodec, VO_PID_DEC_MPEG2_GETLASTOUTVIDEOFORMAT, (VO_PTR*)&outFormat);

	if(outFormat.Type!= VO_VIDEO_FRAME_NULL){
		OutputOneFrame(&outData, &outFormat);
		unNullFrame++;
	}else{
		nullFrame++;
	}

	voMpeg2Dec.Uninit(hCodec);

#if defined (PPC2005)
	finish = GetTickCount();
	duration = (finish - start) * 0.001;
#elif defined(PPC2003)
	finish = GetTickCount();
	duration = (finish - start) * 0.001;
#else
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("\nMPEG2 decoder speed = %2.3f (fps)", frameNumber/duration);
	printf("\n\ttotal frame = %d, total time = %2.3f seconds\n", 
		frameNumber, duration);
#endif 

	if (speedFile){
		fprintf(speedFile, "\nMPEG2 decoder speed = %2.3f (fps)", frameNumber/duration);
		fprintf(speedFile, "\n\ttotal frame = %d, total time = %2.3f seconds\n", 
			frameNumber, duration);
		fclose (speedFile);
	}
End:
	free(inputBuf);
	fclose(inFile);
	if (outFile)
		fclose(outFile);

	printf("Null frame = %d, Other frame = %d\n", nullFrame, unNullFrame);
	printf("Decoder finish!");
#ifdef _DEBUG
	//_CrtDumpMemoryLeaks();
#endif//_DEBUG
	return 0;
}





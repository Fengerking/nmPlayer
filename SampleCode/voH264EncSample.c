	
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2010				*
	*																		*
	************************************************************************/

/*!
**************************************************************************************
*
* \file		voH264EncSample.c
*
* \brief
*		The main function of the h.264 encoder executable proejct
*
**************************************************************************************
*/

//! include files

/*!
************************************************************************************* 
*
* \brief 
*	The main function for the VideoTao MPEG-4 Part 10: AVC / H.264 video encoder
*
* \param
*	int		argc[IN]:				number of agruments, >= 2
*	char	*argv[][IN]:			value of arguments in character strings, argc of them
* 
* \return 
*	Positive integers for successses, zero and negative integers for fatal errors
*   
* \note 
*	The manadatory configuration file (.von) specifies all the coding parameters and controls, 
* as well as input/output files. The input/output files can be replaced on the command line in 
* the same order they appear in the configuration file (and on the command line)
*	
************************************************************************************* 
*/
#include "stdio.h"
#include "stdlib.h"
#include "voH264.h"
#include "time.h"
//#include <Windows.h>

#ifdef CAL_PSNR
extern float total_psnr_y, total_psnr_u,total_psnr_v;
#endif
#ifdef CAL_MEM
extern long total_mem;
#endif


#define OUTPUT			1	//If output bitstream or not.


static int      inputWidth,inputHeight;
static int      nBitRate = 256*1024;
static char     configfile[256];
static FILE*    fconfig = NULL;
static int      isContinue = 0;
static int      nCPUNum = 1;


static void ParseConfig(char* inFile,char* outFile,char* speedFile,int* loopCount,int* outFrameNumber)
{
	isContinue = 0;//default is 0
    
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
        
		fscanf(fconfig,"%d",&nBitRate);          
		fscanf(fconfig,"%*[^\n]");
        
		fscanf(fconfig,"%d",&nCPUNum); 
		fscanf(fconfig,"%*[^\n]");
        
		fscanf(fconfig,"%d",&isContinue);          
		fscanf(fconfig,"%*[^\n]");
        
	}
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

#ifdef RVDS    
int	main()
#elif defined (_IOS)
int beginTest(int argc, char **argv)
#else
int	__cdecl main(int argc, char *argv[])
#endif
{
	FILE *fpRead = NULL, *fpWrite = NULL, *ft = NULL, *fpSpeed = NULL, *fpPSNR = NULL;
    FILE *inFile, *speedFile;
    char inFileName[256], outFileName[256], outSpeedName[256];
    
	int encodeLen, totalCodedSize=0, n, size, j;
	VO_HANDLE				hCodec; 
	VO_VIDEO_ENCAPI			voH264Enc;
	VO_VIDEO_BUFFER			InData;
	VO_CODECBUFFER			OutData;
	VO_IV_COLORTYPE			nVideoType;
	VO_VIDEO_FRAMETYPE		FrameType;
	VO_U32					nEncRC;
	VO_S32					nFrameNum = 300, nFrameSize, nOutBufLen, nFrameIndex = 0, i = 0, loopCount = 1;
	VO_U8					*pInBuf = NULL, *pOutBuf = NULL;

	VO_S32 nBitRate, nWidth, nHeight, nKeyFrmInterval;
	VO_IV_QUALITY			nEncQuality;
	float	FrameRate;

#ifdef _WIN32_WCE
	double  duration = 0;
	VO_S64  start,finish;
#else
    double  duration = 0;
    clock_t start,finish;
#endif
    
    memset(configfile, 0, 256);
#ifdef _IOS
	strcpy(configfile,argv[0]);
#else
    strcpy(configfile, "Z:/yurenjie/Developer/test/rm/RV_cfg_win.txt");
#endif    
    printf("%s",argv[0]); 
    
    fconfig = fopen(configfile,"r");    
    
    ParseConfig(inFileName,outFileName,outSpeedName,&loopCount,&nFrameNum);
    
    

    do{
    do{

	nBitRate        = nBitRate;
	nWidth          = inputWidth;
	nHeight         = inputHeight;
	FrameRate       = 15;
	nKeyFrmInterval = 30; 
	nVideoType		= VO_COLOR_YUV_PLANAR420;//VO_COLOR_YUV_420_PACK_2;//VO_COLOR_YUV_PLANAR420;
	nEncQuality     = VO_ENC_MID_QUALITY;//support LOW,MID,HIGH default is LOW

	printf("\n>>>>>>>>>>>>>>>> H264 encoder Start >>>>>>>>>>>>>>>>111\n");


    duration = 0;
    nFrameIndex = 0;
        
	fpRead = fopen(inFileName, "rb");
	if (fpRead == NULL) {
		goto LFinish;
	}
#if OUTPUT
	fpWrite = fopen(outFileName, "wb");
	if (fpWrite == NULL) {
		goto LFinish; 
	}
#endif
#ifdef _WIN32_WCE
	if(!(fpSpeed = fopen(speedFileName, "a+")))
		goto LFinish;
#endif

#ifdef CAL_PSNR
	if(!(fpPSNR= fopen(PSNRFILE, "a+")))
		goto LFinish;
#endif

#ifdef CAL_MEM
	total_mem = 0;
#endif

	/* Get encoder api*/
	nEncRC = voGetH264EncAPI(&voH264Enc);
	if(nEncRC != VO_ERR_NONE){
		goto LFinish;
	}


	/* Initialize encoder handle*/
	nEncRC = voH264Enc.Init(&hCodec, VO_VIDEO_CodingH264, NULL); 

	if(nEncRC != VO_ERR_NONE){
		goto LFinish;
	}

	/* Set encoding parameters*/
	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_BITRATE,			    &nBitRate);
	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_FRAMERATE,				&FrameRate);
	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_WIDTH,					&nWidth);
	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_HEIGHT,			    &nHeight);
	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_KEYFRAME_INTERVAL,	    &nKeyFrmInterval);
	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_VIDEO_QUALITY,			&nEncQuality);
//	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_INPUT_ROTATION,		&VO_IV_RTTYPE);

	if(nEncRC)
		goto LFinish;

	/* Malloc input buffer */
	if (nVideoType == VO_COLOR_YUV_PLANAR420||
		nVideoType == VO_COLOR_YUV_420_PACK_2||nVideoType == VO_COLOR_YUV_420_PACK){
		nFrameSize = nWidth * nHeight * 3 / 2;
	}
	else if(nVideoType == VO_COLOR_UYVY422_PACKED)
	{
		nFrameSize = nWidth * nHeight * 2;
	}
	else{
		goto LExit;
	}

	pInBuf = (unsigned char *)malloc(nFrameSize * sizeof(unsigned char));
	if (!pInBuf)
		goto LFreeMem;

	/* Initialize input structure*/
	switch(nVideoType){
		case VO_COLOR_YUV_PLANAR420:
			InData.Buffer[0]	= pInBuf;
			InData.Buffer[1]	= pInBuf + nWidth * nHeight;
			InData.Buffer[2]	= InData.Buffer[1] + nWidth * nHeight / 4;
			InData.Stride[0]	= nWidth;
			InData.Stride[1]	= nWidth/2;
			InData.Stride[2]	= nWidth/2;
			break;
		case VO_COLOR_YUV_420_PACK:
		case VO_COLOR_YUV_420_PACK_2:
			InData.Buffer[0]	= pInBuf;
			InData.Buffer[1]	= pInBuf + nWidth * nHeight;
			InData.Buffer[2]	= NULL;
			InData.Stride[0]	= nWidth;
			InData.Stride[1]	= nWidth;
			InData.Stride[2]	= 0;
			break;
		case VO_COLOR_UYVY422_PACKED:
			InData.Buffer[0]	= pInBuf;
			InData.Buffer[1]	= NULL;
			InData.Buffer[2]	= NULL;
			InData.Stride[0]	= nWidth*2;
			InData.Stride[1]	= 0;
			InData.Stride[2]	= 0;
			break;
	}

	InData.ColorType 	= nVideoType;
	
	/* Malloc output buffer*/
	/* Get default output buffer length */
	nEncRC = voH264Enc.GetParam(hCodec, VO_PID_ENC_H264_OUTBUF_SIZE, &nOutBufLen);
	
	pOutBuf = (unsigned char *)malloc(nOutBufLen);
	if (!pOutBuf)
		goto LFreeMem;

	OutData.Buffer = pOutBuf;

	/* Main loop for encoding frames*/
	do {
		/* Read a frame source data */
		if(fread(pInBuf, 1, nFrameSize, fpRead) != nFrameSize)
			goto LFinish;

		/* Change input&output buffer if necessary*/
		InData.Buffer[0] = pInBuf;
		InData.ColorType = nVideoType;
		OutData.Buffer = pOutBuf;
		OutData.Length = nOutBufLen;

		/*if( nFrameIndex == 249 ) {
			nFrameIndex ++;
			nFrameIndex --;
		}*/

#ifdef _WIN32_WCE
		start = GetTickCount();
#else
        start = clock();
#endif
		/* Encode a frame */
		nEncRC = voH264Enc.Process(hCodec, &InData, &OutData, &FrameType);
#ifdef _WIN32_WCE
		finish = GetTickCount();
#else
		finish = clock();
#endif
		duration += (finish - start);   
		/* Output encoded data */
		if (OutData.Length) {
#ifndef TEST_MPEG4ENC_SPEED
			//	printf("\n length = %d,", OutData.Length);
#if OUTPUT
			fwrite(pOutBuf, 1, OutData.Length, fpWrite);
#endif
#endif
		}

#ifndef TEST_MPEG4ENC_SPEED
	//	printf("\nFrame %d finished!", nFrameIndex);
#endif
		nFrameIndex ++;
		
	} while (nFrameIndex < nFrameNum );


LFinish:

	/* Finish mpeg4 encoder */
	nEncRC = voH264Enc.Uninit(hCodec);


    
	if(fpSpeed) {
        duration *= 0.001;		
		fprintf( fpSpeed,"\nTotal Frame:%d, Total Time:%2.3f (s), Speed:%2.3f (fps), bitrate:%d, framerate:%d\n", nFrameIndex, duration, nFrameIndex/duration ,nBitRate,FrameRate);
	}else{
        duration /= CLOCKS_PER_SEC;	
        printf("\nTotal Frame:%d, Total Time:%2.3f (s), Speed:%2.3f (fps), bitrate:%d, framerate:%d\n", nFrameIndex, duration, nFrameIndex/duration ,nBitRate,FrameRate);
    }


#ifdef CAL_PSNR
	if (nFrameIndex > 0) {
		total_psnr_y /= nFrameIndex;
		total_psnr_u /= nFrameIndex;
		total_psnr_v /= nFrameIndex;
	}

	if(fpPSNR)
	{
		fprintf(fpPSNR,OUTFILENAME);
		fprintf(fpPSNR,"\n****Finished frame %d :PSNRY:%4f, PSNRU:%4f, PSNRV:%4f, bitrate:%d, framerate:%d\n", nFrameIndex, 
		    total_psnr_y, total_psnr_u, total_psnr_v,nBitRate,FrameRate);
	}
	
#endif	


LExit:
	if (fpRead)
		fclose(fpRead);
#if OUTPUT
	if (fpWrite)
		fclose(fpWrite);
#endif
#ifdef _WIN32_WCE
	if(fpSpeed)
		fclose(fpSpeed);
#endif
#ifdef CAL_PSNR
    if(fpPSNR)
		fclose(fpPSNR);
#endif
LFreeMem:
	if(pInBuf)
		free(pInBuf);
	if(pOutBuf)
		free(pOutBuf);

	if (nEncRC != 0)
		printf("\nERROR: nEncRC=%d!\n", nEncRC);

	printf("\n>>>>>>>>>>>>>>>> H264 Encoder End >>>>>>>>>>>>>>>>");
	printf("\n");
    }while (loopCount--);
    }while (isContinue);
  
	return(0);


} //!< main()


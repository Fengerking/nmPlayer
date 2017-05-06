	
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2005				*
	*																		*
	************************************************************************/

/*!
**************************************************************************************
*
* \file		main.c
*
* \brief
*		The main function of the VideoTao encoder executable proejct
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
#include "memory.h"

#include "voH264.h"


//#define QCIF15FPS128KBPS
//#define QVGA15FPS384KBPS



int	__cdecl main()
{
	FILE *fpRead = NULL, *fpWrite = NULL, *ft = NULL, *speed_file = NULL;
	int encodeLen, totalCodedSize=0, n, size, j, rc;
	double time0, time1, total_time;
	VO_HANDLE				hCodec; 
	VO_VIDEO_ENCAPI			voH264Enc;
	VO_VIDEO_BUFFER			InData;
	VO_CODECBUFFER			OutData;
	VO_IV_COLORTYPE			nVideoType;
	VO_VIDEO_FRAMETYPE		FrameType;
	VO_U32					nEncRC;
	VO_S32					nFrameNum, nFrameSize, nOutBufLen, nFrameIndex = 0, i = 0;
	VO_U8					*pInBuf = NULL, *pOutBuf = NULL;

	VO_S32 nBitRate, nWidth, nHeight, nKeyFrmInterval;
	float FrameRate;

	double duration = 0;


	nFrameNum = 300;
#if 1 //qcif
	nBitRate        = 0;//128*1024; //100*1024;
	nWidth          = 176;
	nHeight         = 144;
#else  //qvga
	nBitRate        = 384*1024; //100*1024;
	nWidth          = 320;
	nHeight         = 240;
#endif

	FrameRate       = 15.00f;
	nKeyFrmInterval = 30; 
	nVideoType		= VO_COLOR_YUV_PLANAR420;//VO_COLOR_YUV_PLANAR420;


	fpRead = fopen("H:\\test\\mpeg4\\foreman_qcif.yuv", "rb");
	if (fpRead == NULL) {
		goto LFinish;
	}

	fpWrite = fopen("H:\\test\\h264\\QCIF.264", "wb");
	if (fpWrite == NULL) {
		goto LFinish;
	}

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
//	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_BITRATE,			    &nBitRate);
//	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_FRAMERATE,				&FrameRate);
	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_WIDTH,					&nWidth);
	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_HEIGHT,			    &nHeight);
//	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_KEY_FRAME_INTERVAL,	&nKeyFrmInterval);
//	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_VIDEO_QUALITY,			&nEncQuality);
//	nEncRC |= voH264Enc.SetParam(hCodec, VO_PID_ENC_H264_INPUT_ROTATION,		&VO_IV_RTTYPE);

	if(nEncRC)
		goto LFinish;

	/* Malloc input buffer */
	if(nVideoType == VO_COLOR_UYVY422_PACKED || nVideoType == VO_COLOR_RGB565_PACKED ||
		nVideoType == VO_COLOR_VYUY422_PACKED_2 || nVideoType == VO_COLOR_YUYV422_PACKED || nVideoType == VO_COLOR_YUV_PLANAR422_12){
		nFrameSize = nWidth * nHeight * 2;
	}else if (nVideoType == VO_COLOR_YUV_PLANAR420||
		nVideoType == VO_COLOR_YVU_PLANAR420 ||
		VO_COLOR_YUV_420_PACK_2){
		nFrameSize = nWidth * nHeight * 3 / 2;
	}else{
		goto LExit;
	}

	pInBuf = (unsigned char *)malloc(nFrameSize * sizeof(unsigned char));
	if (!pInBuf)
		goto LFreeMem;

	/* Initialize input structure*/
	switch(nVideoType){
		case VO_COLOR_UYVY422_PACKED:
		case VO_COLOR_RGB565_PACKED:
		case VO_COLOR_VYUY422_PACKED_2:
		case VO_COLOR_YUYV422_PACKED:
			InData.Buffer[0]	= pInBuf;
			InData.Buffer[1]	= NULL;
			InData.Buffer[2]	= NULL;
			InData.Stride[0]	= nWidth*2;
			InData.Stride[1]	= 0;
			InData.Stride[2]	= 0;
			break;
		case VO_COLOR_YUV_PLANAR420:
		case VO_COLOR_YVU_PLANAR420:
			InData.Buffer[0]	= pInBuf;
			InData.Buffer[1]	= pInBuf + nWidth * nHeight;
			InData.Buffer[2]	= InData.Buffer[1] + nWidth * nHeight / 4;
			InData.Stride[0]	= nWidth;
			InData.Stride[1]	= nWidth/2;
			InData.Stride[2]	= nWidth/2;
			break;
		case VO_COLOR_YUV_420_PACK_2:
			InData.Buffer[0]	= pInBuf;
			InData.Buffer[1]	= pInBuf + nWidth * nHeight;
			InData.Buffer[2]	= NULL;
			InData.Stride[0]	= nWidth;
			InData.Stride[1]	= nWidth;
			InData.Stride[2]	= 0;
			break;
		case VO_COLOR_YUV_PLANAR422_12:
			InData.Buffer[0]	= pInBuf;
			InData.Buffer[1]	= pInBuf + nWidth * nHeight;
			InData.Buffer[2]	= InData.Buffer[1] + nWidth * nHeight / 2;
			InData.Stride[0]	= nWidth;
			InData.Stride[1]	= nWidth/2;
			InData.Stride[2]	= nWidth/2;

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

#ifdef _WIN32_WCE
		start = GetTickCount();
#endif
		/* Encode a frame */
		nEncRC = voH264Enc.Process(hCodec, &InData, &OutData, &FrameType);
#ifdef _WIN32_WCE
		finish = GetTickCount();
		duration += (finish - start);
#endif

		/* Output encoded data */
		if (OutData.Length) {
#ifndef TEST_MPEG4ENC_SPEED
			//	printf("\n length = %d,", OutData.Length);
			fwrite(pOutBuf, 1, OutData.Length, fpWrite);
#endif
		}

#ifndef TEST_MPEG4ENC_SPEED
		printf("\nFrame %d finished!", nFrameIndex);
#endif
		nFrameIndex ++;
		
	} while (--nFrameNum != 0);


LFinish:

	/* Finish mpeg4 encoder */
	nEncRC = voH264Enc.Uninit(hCodec);


	if(speed_file) {
		duration *= 0.001;
		fprintf( speed_file,"\nTotal Frame:%d, Total Time:%2.3f (s), Speed:%2.3f (fps)\n", nFrameIndex, duration, nFrameIndex/duration );
	}
		   
LExit:
	if (fpRead)
		fclose(fpRead);
	if (fpWrite)
		fclose(fpWrite);
	if(speed_file)
		fclose(speed_file);
LFreeMem:
	if(pInBuf)
		free(pInBuf);
	if(pOutBuf)
		free(pOutBuf);

	if (nEncRC != 0)
		printf("\nERROR: rc=%d!\n", rc);

	return(nEncRC);


} //!< main()


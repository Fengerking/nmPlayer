
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2005				*
	*																		*
	************************************************************************/

/*!
************************************************************************
*
* \file		voMPEG4Enc_Sample.c
*
* \brief
*		Sample file for VisualOn's MPEG-4 encoder
*
************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "voMPEG4.h"
#ifdef _WIN32_WCE
#include <windows.h>
#endif

#ifdef _WIN32_WCE
#define _VS2005			//define _VS2005 or _EVC4
#endif

#define TEST_MEM_OPERATER
#ifdef TEST_MEM_OPERATER
VO_U32 Alloc(VO_S32 uID, VO_MEM_INFO * pMemInfo);
VO_U32 Free (VO_S32 uID, VO_PTR pBuff);
#endif

//#define TEST_MPEG4ENC_SPEED		//there is no video output when testing speed.

#define H263_ENC
#define QCIF_INPUT				//QCIF_INPUT,QVGA_INPUT,VGA_INPUT
#define FRAME_NUM		150

#ifdef _WIN32_WCE
#define IN_FILE_NAME	"\\storage card\\siemens_vdo_car_qvga_1000f.yuv"    //foreman_qcif_300f.yuv, siemens_vdo_car_qvga_1000f.yuv, stageMTV_qcif_3000f.yuv
#define OUT_FILE_NAME	"\\storage card\\siemens_vdo_car_qvga_1000f.mpeg4" //
#define SPEED_FILE_NAME	"\\mpeg4_speed.txt"
#else
#define IN_FILE_NAME	"D:\\Data_Cmp\\422.yuv" 
#define OUT_FILE_NAME	"D:\\Data_Cmp\\out.h263"
#define SPEED_FILE_NAME	"D:\\Data_Cmp\\mpeg4_speed.txt"
#endif

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

#if defined (_VS2005)

int _tmain(int argc, char *argv[])
//int _tmain(int argc, _TCHAR* argv[])

#elif defined (_EVC4)

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPWSTR     lpCmdLine,
                   int        CmdShow)
#else        
				   
#ifdef RVDS    
int	main()   
#else  
int	main(int argc, char *argv[])
#endif

#endif 
{
	int rc=0;
	FILE *in_file = NULL, *out_file = NULL, *speed_file = NULL;
	unsigned char *pInBuf = NULL, *pOutBuf = NULL;

	VO_HANDLE				hCodec; 
	VO_U32					nEncRC;
	VO_VIDEO_BUFFER			InData;
	VO_CODECBUFFER			OutData;
	VO_VIDEO_FRAMETYPE		FrameType;
	VO_IV_RTTYPE			VO_IV_RTTYPE;
	VO_IV_QUALITY			nEncQuality;
	VO_IV_COLORTYPE			nVideoType;
	VO_VIDEO_ENCAPI			voMpeg4Enc;
	VO_CODEC_INIT_USERDATA  *pUserData = NULL;

#ifdef _WIN32_WCE
	clock_t start, finish;
#endif
	VO_S32 nBitRate, nWidth, nHeight, nKeyFrmInterval;
	float FrameRate;
	double duration = 0;
	int nFrameNum, nFrameSize, nOutBufLen, nFrameIndex = 0, i = 0;

	nFrameNum		= FRAME_NUM;
	
	/* Default encoding parameters for encoder lib */
#ifdef QCIF_INPUT
	nBitRate        = 3*176*144; //128*1024;
	nWidth          = 176;
	nHeight         = 144;
#endif
#ifdef QVGA_INPUT
	nBitRate        = 384*1024; //384*1024;
	nWidth          = 320;
	nHeight         = 240;
#endif
#ifdef VGA_INPUT
	nBitRate        = 384*4*1024; //384*4*1024;
	nWidth          = 640;
	nHeight         = 480;
#endif
	FrameRate       = 15.00f;
	nKeyFrmInterval = 15; 
	nEncQuality     = VO_ENC_MID_QUALITY;
	nVideoType		= VO_COLOR_YUYV422_PACKED;//VO_COLOR_YUV_PLANAR420;
	VO_IV_RTTYPE 	= VO_RT_DISABLE;

	/* Custome parameters setting */

	/* source file YUV420 */
	if (!(in_file = fopen(IN_FILE_NAME, "rb"))){
		return 0;
	}
	
	/* mpeg4 encoded data */
	if (!(out_file = fopen(OUT_FILE_NAME, "wb"))){
		return 0;
	}

	/* mpeg4 encoded data */
	if (!(speed_file = fopen(SPEED_FILE_NAME, "a+"))){
		return 0;
	}

	/* Get encoder api*/
	nEncRC = voGetMPEG4EncAPI(&voMpeg4Enc);
	if(nEncRC != VO_ERR_NONE){
		goto LFinish;
	}

	/* Initialize encoder handle*/

#ifdef TEST_MEM_OPERATER
	{
		VO_CODEC_INIT_USERDATA userData;
		VO_MEM_OPERATOR memOp;
		memOp.Alloc = Alloc;
		memOp.Free  = Free;
		userData.memData = (VO_PTR*)&memOp;
		userData.memflag = 0 | VO_IMF_USERMEMOPERATOR;
		pUserData = &userData;
	}
#else
	pUserData = NULL;
#endif

#ifdef H263_ENC
	nEncRC = voMpeg4Enc.Init(&hCodec, VO_VIDEO_CodingH263, pUserData); 
#else
	nEncRC = voMpeg4Enc.Init(&hCodec, VO_VIDEO_CodingMPEG4, pUserData); 
#endif
	if(nEncRC != VO_ERR_NONE){
		goto LFinish;
	}

	/* Set encoding parameters*/
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_BITRATE,			    &nBitRate);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_FRAMERATE,			&FrameRate);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_WIDTH,			    &nWidth);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_HEIGHT,			    &nHeight);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_KEY_FRAME_INTERVAL,  &nKeyFrmInterval);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_VIDEO_QUALITY,		&nEncQuality);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_INPUT_ROTATION,		&VO_IV_RTTYPE);

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
	nEncRC = voMpeg4Enc.GetParam(hCodec, VO_PID_ENC_MPEG4_OUTBUF_SIZE, &nOutBufLen);
	
	pOutBuf = (unsigned char *)malloc(nOutBufLen);
	if (!pOutBuf)
		goto LFreeMem;

	OutData.Buffer = pOutBuf;

	/* Get VOL data if necessary*/
	nEncRC = voMpeg4Enc.GetParam(hCodec, VO_PID_ENC_MPEG4_VOL_HEADER, &OutData); 

	/* Main loop for encoding frames*/
	do {
		VO_U32 uForceIFrame = 1;
		/* Read a frame source data */
		if(fread(pInBuf, 1, nFrameSize, in_file) != nFrameSize)
			goto LFinish;

		/* Change input&output buffer if necessary*/
		InData.Buffer[0] = pInBuf;
		InData.ColorType = nVideoType;
		OutData.Buffer = pOutBuf;

		/* Force to encode I frame */		
//		nEncRC = voMPEG4ENCSetParameter(hCodec, VO_PID_ENC_MPEG4_FORCE_KEY_FRAME, &uForceIFrame); 
#ifdef _WIN32_WCE
		start = GetTickCount();
#endif

		/* Encode a frame */
		nEncRC = voMpeg4Enc.Process(hCodec, &InData, &OutData, &FrameType);
#ifdef _WIN32_WCE
		finish = GetTickCount();
		duration += (finish - start);
#endif

		/* Output encoded data */
		if (OutData.Length) {
#ifndef TEST_MPEG4ENC_SPEED
			//	printf("\n length = %d,", OutData.Length);
			fwrite(pOutBuf, 1, OutData.Length, out_file);
#endif
		}

#ifndef TEST_MPEG4ENC_SPEED
		printf("\nFrame %d finished!", nFrameIndex);
#endif
		nFrameIndex ++;
		
	} while (--nFrameNum != 0);


LFinish:

	/* Finish mpeg4 encoder */
	nEncRC = voMpeg4Enc.Uninit(hCodec);


	if(speed_file) {
		duration *= 0.001;
		fprintf( speed_file,"\nTotal Frame:%d, Total Time:%2.3f (s), Speed:%2.3f (fps)\n", nFrameIndex, duration, nFrameIndex/duration );
	}
		   
LExit:
	if (in_file)
		fclose(in_file);
	if (out_file)
		fclose(out_file);
	if(speed_file)
		fclose(speed_file);
LFreeMem:
	if(pInBuf)
		free(pInBuf);
	if(pOutBuf)
		free(pOutBuf);

	if (rc != 0)
		printf("\nERROR: rc=%d!\n", rc);

	return(rc);

} //!< main()


#ifdef TEST_MEM_OPERATER
VO_U32 Alloc(VO_S32 uID, VO_MEM_INFO * pMemInfo)
{
	pMemInfo->VBuffer = (void*)malloc(pMemInfo->Size);
	return 0;
}
VO_U32 Free(VO_S32 uID, VO_PTR pBuff)
{
	free(pBuff);
	return 0;
}
#endif
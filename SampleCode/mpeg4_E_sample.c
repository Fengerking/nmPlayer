
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2005				*
	*																		*
	************************************************************************/

/*!
************************************************************************
*
* \file		mpeg4_E_sample.c
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


static	show_usage(void)
{
	printf("\nUsage: mp4e [configuration_file_name] [-option_name option_value]*\n");
	printf("\n\t-i|I input_file_name");
	printf("\n\t-o|O output_file_name");
	printf("\n\t-w|W width");
	printf("\n\t-h|H height");
	printf("\n\t-b|B bitrate");
	printf("\n\t-r|R frame_rate");
	printf("\n\t-g|G maximum_GOP_size");
	printf("\n\t-q|Q encoding_quality");
	printf("\n\t-t|T video_type");
	printf("\n\t-n|N number_of_frames_to_encode");
} //!< show_usage()

//int WinMain(int argc, char *argv[])
int	main(int argc, char *argv[])
{
	int k, rc=0;
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

	long int nBitRate, nWidth, nHeight, nKeyFrmInterval;
	float FrameRate;
	clock_t start, finish;
	double duration = 0;

	int nFrameNum, nFrameSize, nOutBufLen, nFrameIndex = 0, i = 0;
	nFrameNum		= 300;
	
	/* Default paremeters setting for encoder lib */
	nBitRate        = 150*1024;
	FrameRate       = 15.00f;
	nWidth          = 176;
	nHeight         = 144;
	nKeyFrmInterval = 30; 
	nEncQuality     = VO_ENC_MID_QUALITY;
	nVideoType		= VO_COLOR_YUV_PLANAR420;
	VO_IV_RTTYPE 	= VO_RT_DISABLE;

	/* customer parameters setting */
	/* command line parameters override defaults and those in configuration file (if present)*/
#if 1
	k = 1;
	if (argc > 1) {
		for ( ; k < argc; k += 2) {
			switch (argv[k][1]) {
				case 'i':
				case 'I':
					if (in_file != NULL)
						fclose(in_file);

					if ((in_file=fopen(argv[k+1], "rb")) == NULL) {
						rc = -11;
						goto LExit;
					}

					break;

				case 'o':
				case 'O':
					if (out_file != NULL)
						fclose(out_file);
		
					if ((out_file=fopen(argv[k+1], "w+b")) == NULL) {
						rc = -22;
						goto LExit;
					}

					break;

				case 'w':
				case 'W':
					nWidth = strtol(argv[k+1], NULL, 10);
					break;

				case 'h':
				case 'H':
					nHeight = strtol(argv[k+1], NULL, 10);
					break;

				case 'b':
				case 'B':
					nBitRate = strtol(argv[k+1], NULL, 10);
					break;

				case 'r':
				case 'R':
					FrameRate = strtod(argv[k+1], NULL);
					break;

				case 'g':
				case 'G':
					nKeyFrmInterval = strtol(argv[k+1], NULL, 10);
					break;

				case 'q':
				case 'Q':
					nEncQuality = strtol(argv[k+1], NULL, 10);
					break;

				case 'n':
				case 'N':
					nFrameNum = strtol(argv[k+1], NULL, 10);
					break;

				case 't':
				case 'T':
					nVideoType = strtol(argv[k+1], NULL, 10);
					break;

				default:
					rc = 33333;
					goto LExit;
			} //!< switch(argv[k][1])
		} //!< for(k)
	}else{
		show_usage();
	}
#else
		/* source file YUV420 */
	if (!(in_file = fopen("\\SDMMC\\foreman_qcif.yuv", "rb"))){
		return 0;
	}
	
	/* mpeg4 encoded data */
	if (!(out_file = fopen("\\bb.mpeg4", "w+b"))){
		return 0;
	}

	/* mpeg4 encoded data */
	if (!(speed_file = fopen("\\mpeg4Enc_speed.txt", "w+b"))){
		return 0;
	}
#endif
	/* Initialize input structure*/
	if(nVideoType == VO_COLOR_UYVY422_PACKED || nVideoType == VO_COLOR_RGB565_PACKED ||
		nVideoType == VO_COLOR_VYUY422_PACKED_2 || nVideoType == VO_COLOR_YUYV422_PACKED || nVideoType == VO_COLOR_YUV_PLANAR422_12){
		nFrameSize = nWidth * nHeight * 2;
	}else if (nVideoType == VO_COLOR_YUV_PLANAR420||nVideoType == VO_COLOR_YVU_PLANAR420){
		nFrameSize = nWidth * nHeight * 3 / 2;
	}else{
		goto LExit;
	}

	/* get encoder api*/
	nEncRC = voGetMPEG4EncAPI(&voMpeg4Enc, 0);
	if(nEncRC != VO_ERR_NONE){
		goto LFinish;
	}

	nEncRC = voMpeg4Enc.Init(&hCodec, VO_VIDEO_CodingMPEG4, NULL);
	if(nEncRC != VO_ERR_NONE){
		goto LFinish;
	}

	
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_BITRATE,			    &nBitRate);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_FRAMERATE,			&FrameRate);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_WIDTH,			    &nWidth);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_HEIGHT,			    &nHeight);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_KEY_FRAME_INTERVAL,  &nKeyFrmInterval);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_VIDEO_QUALITY,		&nEncQuality);
	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_INPUT_ROTATION,		&VO_IV_RTTYPE);
//	nEncRC |= voMpeg4Enc.SetParam(hCodec, VO_PID_ENC_MPEG4_VP_SIZE,			224);

	if(nEncRC)
		goto LFinish;

	/* input buffer */
	pInBuf = (unsigned char *)malloc(nFrameSize * sizeof(unsigned char));
	if (!pInBuf)
		goto LFreeMem;

	InData.Buffer[0]	= pInBuf;
	InData.ColorType 	= nVideoType;
	
	/* output buffer */
	nEncRC = voMpeg4Enc.GetParam(hCodec, VO_PID_ENC_MPEG4_OUTBUF_SIZE, &nOutBufLen);
	
	pOutBuf = (unsigned char *)malloc(nOutBufLen);
	if (!pOutBuf)
		goto LFreeMem;

	OutData.Buffer = pOutBuf;

	nEncRC = voMpeg4Enc.GetParam(hCodec, VO_PID_ENC_MPEG4_VOL_HEADER, &OutData); 

	/* encoder main loop*/
	do {
		/* read a frame source data */
		if(fread(pInBuf, 1, nFrameSize, in_file) != nFrameSize)
			goto LFinish;

		/* Change input&output buffer if necessary*/
		InData.Buffer[0] = pInBuf;
		InData.ColorType = nVideoType;
		OutData.Buffer = pOutBuf;

		/* set parameters for a frame if need */		
//		nEncRC = voMPEG4ENCSetParameter(hCodec, VO_PID_ENC_MPEG4_FORCE_KEY_FRAME, 0); 
#ifdef _WIN32_WCE
		start = GetTickCount();
#endif
		/* encode a frame */
		nEncRC = voMpeg4Enc.Process(hCodec, &InData, &OutData, &FrameType);
#ifdef _WIN32_WCE
		finish = GetTickCount();
		duration += (finish - start);
#endif

		/* Output encoded data */
		if (OutData.Length) {
			printf("\n length = %d,", OutData.Length);
		//fprintf(speed_file,"\n length = %d,", OutData.length);
			fwrite(pOutBuf, 1, OutData.Length, out_file);
		}

		//printf("Frame %d finished!\n", nFrameIndex++);
		
	} while (--nFrameNum != 0);


LFinish:
	/* finish mpeg4 encoder */
	nEncRC = voMpeg4Enc.Uninit(hCodec);


	if(speed_file)
		fprintf(speed_file,"\n****Finished frame %d duration:%4f\n", nFrameIndex,  duration*0.001);
		   
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


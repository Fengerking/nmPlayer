/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

/*!
************************************************************************
*
* \file		color_conversion_sample.c
*
* \brief
*		Sample file for VisualOn's color conversion
*
************************************************************************
*/

#ifdef _WIN32_WCE
#include <windows.h>
#include <commctrl.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#endif

#include "voColorConversion.h"
#include "voCCRRR.h"

#ifdef  COLCONVENABLE

static	show_usage(void)
{
	printf("\nUsage: voCCSample -i input_file_name -o output_file_name \n");
	printf("\n");
} //!< show_usage()

int TestNormal(int argc, char *argv[])
{
	int k,i,l, in_size[3], out_size[3], out_height, rc = 0;
	int	frame_size;

	FILE *in_file = NULL, *out_file = NULL,*stats_file = NULL ;
	unsigned char *pInBuf[3] = {NULL, NULL, NULL}, *pOutBuf[3] = {NULL, NULL, NULL};

	double enctime_start, enctime=0;
	double totalenctime = 0.;

	VO_HANDLE			hCodec;
	VOCCRETURNCODE		nCcRC;
	VO_CODECBUFFER		InData;
	VO_VIDEO_BUFFER		OutData;
	ClrConvData			ClrConvData;

	VO_CLRCONV_DECAPI   voClrConv;

	/* customer parameters setting */
	/* command line parameters override defaults and those in configuration file (if present)*/
#ifdef _WIN32_WCE

	/* source file YUV420 */
	if (!(in_file = fopen("\\SDMMC\\test.yuv", "rb"))){
		return 0;
	}

	//if (!(in_file = fopen("\\800X824_src.RGB888", "rb"))){
	//	return 0;
	//}

	//if (!(in_file = fopen("\\bigpic.yuv", "rb"))){
	//	return 0;
	//}

	//if (!(in_file = fopen("\\out_arm.yuv", "rb"))){
	//	return 0;
	//}
	
	/* out put data */
	if (!(out_file = fopen("\\test_o.yuv", "w+b"))){
		return 0;
	}

	if(!(stats_file = fopen("\\color_conversion.txt", "wb"))){
		printf("Open statistic file error!");
	}
#else
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

				default:
					rc = 33333;
					goto LExit;
			} //!< switch(argv[k][1])
		} //!< for(k)
	}else{
		show_usage();
		return;
	}
#endif

//	for(l=0; l< 800; l++)
	{
//		if(l==290)
//			l+=2;
	
	/* initialize color conversion encoder */
	voGetClrConvAPI(&voClrConv,NULL);
	nCcRC = voClrConv.CCInit(&hCodec, NULL);
	
	if(nCcRC){
		goto LFinish;
	}

//	frame_size = 176 * 144 * 2 / 3;

	//ClrConvData.nInType		= VOYUV_420_PACK;//VORGB888_PACKED;//VORGB888_PLANAR;//VOYUV_PLANAR420;//VOYUYV422_PACKED;//VOYUV_420_PACK_2;//VOYUV_PLANAR420;//VOYUV_420_PACK;//VORGB565_PACKED;//VOYUV_PLANAR420;//VOYUYV422_PACKED;
	//ClrConvData.nInWidth		= 1344;
	//ClrConvData.nInHeight		= 1984;
	//ClrConvData.nInStride		= 1344;
	//ClrConvData.nOutType		= VORGB888_PACKED;//VORGB565_PACKED;//VOYUV_420_PACK;//VORGB565_PACKED;//VOYUV_PLANAR420;//VOYUYV422_PACKED;
	//ClrConvData.nOutWidth		= 1344;
	//ClrConvData.nOutHeight		= 1984;
	//ClrConvData.nOutStride		= 1344*3;
	//ClrConvData.nIsResize		= 0;
	//ClrConvData.nRotationType 	= ROTATION_DISABLE;//FLIP_Y;//ROTATION_180;//ROTATION_90R;//ROTATION_90L;//ROTATION_DISABLE;
	//ClrConvData.nAntiAliasLevel = NO_ANTIALIASING;//NO_ANTIALIASING;
	//ClrConvData.mb_skip         = NULL;
	//ClrConvData.nInUVStride     = 0;

	//ClrConvData.nInType			= VO_COLOR_YUV_420_PACK;//VO_COLOR_RGB888_PACKED;//VO_COLOR_RGB888_PLANAR;//VO_COLOR_YUV_PLANAR420;//VO_COLOR_YUYV422_PACKED;//VO_COLOR_YUV_420_PACK_2;//VO_COLOR_YUV_PLANAR420;//VO_COLOR_YUV_420_PACK;//VO_COLOR_RGB565_PACKED;//VO_COLOR_YUV_PLANAR420;//VO_COLOR_YUYV422_PACKED;
	//ClrConvData.nInWidth		= 352;
	//ClrConvData.nInHeight		= 288;
	//ClrConvData.nInStride		= 352;
	//ClrConvData.nOutType		= VO_COLOR_YUV_420_PACK;//VO_COLOR_RGB565_PACKED;//VO_COLOR_YUV_420_PACK;//VO_COLOR_RGB565_PACKED;//VO_COLOR_YUV_PLANAR420;//VO_COLOR_YUYV422_PACKED;
	//ClrConvData.nOutWidth		= 176;
	//ClrConvData.nOutHeight		= 144;
	//ClrConvData.nOutStride		= 144;
	//ClrConvData.nIsResize		= 0;
	//ClrConvData.nRotationType 	= ROTATION_90L;//FLIP_Y;//ROTATION_180;//ROTATION_90R;//ROTATION_90L;//ROTATION_DISABLE;
	//ClrConvData.nAntiAliasLevel = NO_ANTIALIASING;//NO_ANTIALIASING;
	//ClrConvData.mb_skip         = NULL;
	//ClrConvData.nInUVStride     = 0;

	//ClrConvData.nInType			= VO_COLOR_YUV_PLANAR420;//VORGB888_PACKED;//VORGB888_PLANAR;//VOYUV_PLANAR420;//VOYUYV422_PACKED;//VOYUV_420_PACK_2;//VOYUV_PLANAR420;//VOYUV_420_PACK;//VORGB565_PACKED;//VOYUV_PLANAR420;//VOYUYV422_PACKED;
	//ClrConvData.nInWidth		= 1280 - 6;
	//ClrConvData.nInHeight		= 720 - 6;
	//ClrConvData.nInStride		= 1280 - 6;
	//ClrConvData.nOutType		= VO_COLOR_RGB565_PACKED;//VORGB565_PACKED;//VOYUV_420_PACK;//VORGB565_PACKED;//VOYUV_PLANAR420;//VOYUYV422_PACKED;
	//ClrConvData.nOutWidth		= 1280 - 6;
	//ClrConvData.nOutHeight		= 720 - 6;
	//ClrConvData.nOutStride		= (1280 - 6)*2;
	//ClrConvData.nIsResize		= 0;
	//ClrConvData.nRotationType 	= ROTATION_180;//FLIP_Y;//ROTATION_180;//ROTATION_90R;//ROTATION_90L;//ROTATION_DISABLE;
	//ClrConvData.nAntiAliasLevel = NO_ANTIALIASING;//NO_ANTIALIASING;
	//ClrConvData.mb_skip         = NULL;
	//ClrConvData.nInUVStride     = 0;

	ClrConvData.nInType			= VO_COLOR_YUV_420_PACK;//VORGB888_PACKED;//VORGB888_PLANAR;//VOYUV_PLANAR420;//VOYUYV422_PACKED;//VOYUV_420_PACK_2;//VOYUV_PLANAR420;//VOYUV_420_PACK;//VORGB565_PACKED;//VOYUV_PLANAR420;//VOYUYV422_PACKED;
	ClrConvData.nInWidth		= 240;
	ClrConvData.nInHeight		= 320;
	ClrConvData.nInStride		= 240;
	ClrConvData.nOutType		= VO_COLOR_ARGB32_PACKED;//VORGB565_PACKED;//VOYUV_420_PACK;//VORGB565_PACKED;//VOYUV_PLANAR420;//VOYUYV422_PACKED;
	ClrConvData.nOutWidth		= 320;
	ClrConvData.nOutHeight		= 240;
	ClrConvData.nOutStride		= 480;
	ClrConvData.nIsResize		= 1;
	ClrConvData.nRotationType 	= ROTATION_DISABLE;//FLIP_Y;//ROTATION_180;//ROTATION_90R;//ROTATION_90L;//ROTATION_DISABLE;
	ClrConvData.nAntiAliasLevel = NO_ANTIALIASING;//NO_ANTIALIASING;
	ClrConvData.mb_skip         = NULL;
	ClrConvData.nInUVStride     = 0;

	nCcRC |= voClrConv.CCSetParam(hCodec, VO_PID_CC_INIINSTANCE, (VO_PTR)(&ClrConvData));

	if(nCcRC)
		goto LFinish;

	/* Initialize input size and malloc input buffer*/
	if(ClrConvData.nInType == VO_COLOR_YUV_PLANAR420 || ClrConvData.nInType == VO_COLOR_YVU_PLANAR420){
		in_size[0] = ClrConvData.nInStride * ClrConvData.nInHeight;
		in_size[1] = in_size[0] / 4;
		in_size[2] = in_size[0] / 4;
	}else if (ClrConvData.nInType == VO_COLOR_YUV_PLANAR422_12 || ClrConvData.nInType == VO_COLOR_YVU_PLANAR422_12 || ClrConvData.nInType == VO_COLOR_YUV_PLANAR422_21){
		in_size[0] = ClrConvData.nInStride * ClrConvData.nInHeight;
		in_size[1] = in_size[0] / 2;
		in_size[2] = in_size[0] / 2;
	}else if(ClrConvData.nInType == VO_COLOR_YUV_PLANAR444 || ClrConvData.nInType == VO_COLOR_RGB888_PLANAR){
		in_size[0] = ClrConvData.nInStride * ClrConvData.nInHeight;
		in_size[1] = in_size[0];
		in_size[2] = in_size[0];
	}else if (ClrConvData.nInType == VO_COLOR_YUV_PLANAR411 || ClrConvData.nInType == VO_COLOR_YUV_PLANAR411V){
		in_size[0] = ClrConvData.nInStride * ClrConvData.nInHeight;
		in_size[1] = in_size[0] / 4;
		in_size[2] = in_size[0] / 4;
	}else if(ClrConvData.nInType == VO_COLOR_RGB565_PACKED || ClrConvData.nInType == VO_COLOR_RGB555_PACKED ||
		ClrConvData.nInType == VO_COLOR_YUYV422_PACKED || ClrConvData.nInType == VO_COLOR_YVYU422_PACKED ||
		ClrConvData.nInType == VO_COLOR_UYVY422_PACKED || ClrConvData.nInType == VO_COLOR_VYUY422_PACKED ||
		ClrConvData.nInType == VO_COLOR_YUYV422_PACKED_2 || ClrConvData.nInType == VO_COLOR_YVYU422_PACKED_2 ||
		ClrConvData.nInType == VO_COLOR_UYVY422_PACKED_2 || ClrConvData.nInType == VO_COLOR_VYUY422_PACKED_2 ||
		ClrConvData.nInType == VO_COLOR_RGB888_PACKED){
		in_size[0] = ClrConvData.nInStride * ClrConvData.nInHeight;
		in_size[1] = 0;
		in_size[2] = 0;
	}else if(ClrConvData.nInType == VO_COLOR_YUV_420_PACK||ClrConvData.nInType == VO_COLOR_YUV_420_PACK_2){
		in_size[0] = ClrConvData.nInStride * ClrConvData.nInHeight;
		in_size[1] = in_size[0] / 2;
		in_size[2] = 0;
	}else{
		goto LExit;
	}

	/* input buffer */
	if(!(pInBuf[0] = (unsigned char *)malloc(in_size[0] * sizeof(unsigned char))))
		goto LFreeMem;

	if(!(pInBuf[1] = (unsigned char *)malloc(in_size[1] * sizeof(unsigned char))))
		goto LFreeMem;

	if(!(pInBuf[2] = (unsigned char *)malloc(in_size[2] * sizeof(unsigned char))))
		goto LFreeMem;

	if(ClrConvData.nRotationType == ROTATION_90L || ClrConvData.nRotationType == ROTATION_90R){
		out_height = ClrConvData.nOutWidth;
	}else{
		out_height = ClrConvData.nOutHeight;
	}

	/* output buffer */
	if(ClrConvData.nOutType == VO_COLOR_YUV_PLANAR420){
		out_size[0] = ClrConvData.nOutStride * out_height;
		out_size[1] = out_size[0] / 4;
		out_size[2] = out_size[0] / 4;
	}else if(ClrConvData.nOutType == VO_COLOR_RGB565_PACKED || ClrConvData.nOutType == VO_COLOR_RGB555_PACKED ||\
			ClrConvData.nOutType == VO_COLOR_YUYV422_PACKED || ClrConvData.nOutType == VO_COLOR_YVYU422_PACKED ||\
			ClrConvData.nOutType == VO_COLOR_UYVY422_PACKED || ClrConvData.nOutType == VO_COLOR_VYUY422_PACKED ||\
			ClrConvData.nOutType == VO_COLOR_YUYV422_PACKED_2 || ClrConvData.nOutType == VO_COLOR_YVYU422_PACKED_2 ||\
			ClrConvData.nOutType == VO_COLOR_UYVY422_PACKED_2 || ClrConvData.nOutType == VO_COLOR_VYUY422_PACKED_2 ||\
			ClrConvData.nOutType == VO_COLOR_RGB888_PACKED){
		out_size[0] = ClrConvData.nOutStride * out_height;
		out_size[1] = 0;
		out_size[2] = 0;
	}else if(ClrConvData.nOutType == VO_COLOR_YUV_420_PACK||ClrConvData.nOutType == VO_COLOR_YUV_420_PACK_2){
		out_size[0] = ClrConvData.nOutStride * out_height;
		out_size[1] = out_size[0] / 2;
		out_size[2] = 0;
	}else if (ClrConvData.nOutType == VO_COLOR_RGB32_PACKED){
		out_size[0] = ClrConvData.nOutStride * out_height;
		out_size[1] = 0;
		out_size[2] = 0;
	}else{
		goto LExit;
	}


	if(!(pOutBuf[0] = (unsigned char *)malloc(out_size[0] * sizeof(unsigned char))))
		goto LFreeMem;
	if(!(pOutBuf[1] = (unsigned char *)malloc(out_size[1] * sizeof(unsigned char))))
		goto LFreeMem;
	if(!(pOutBuf[2] = (unsigned char *)malloc(out_size[2] * sizeof(unsigned char))))
		goto LFreeMem;

	/* encoder main loop*/
	for (k = 0; k < 150; k++){
		/* can change input buffer address if necessary */
		ClrConvData.pInBuf[0] = pInBuf[0];
		ClrConvData.pInBuf[1] = pInBuf[1];
		ClrConvData.pInBuf[2] = pInBuf[2];

		/* can change output buffer address if necessary */
		ClrConvData.pOutBuf[0] = pOutBuf[0];
		ClrConvData.pOutBuf[1] = pOutBuf[1];
		ClrConvData.pOutBuf[2] = pOutBuf[2];

		fread(ClrConvData.pInBuf[0], 1, in_size[0], in_file);
		if(ClrConvData.nInType != VO_COLOR_YUYV422_PACKED && ClrConvData.nInType != VO_COLOR_YVYU422_PACKED &&
			ClrConvData.nInType != VO_COLOR_UYVY422_PACKED && ClrConvData.nInType != VO_COLOR_VYUY422_PACKED && 
			ClrConvData.nInType != VO_COLOR_YUYV422_PACKED_2 && ClrConvData.nInType != VO_COLOR_YVYU422_PACKED_2 &&
			ClrConvData.nInType != VO_COLOR_UYVY422_PACKED_2 && ClrConvData.nInType != VO_COLOR_VYUY422_PACKED_2 &&
			ClrConvData.nInType != VO_COLOR_RGB565_PACKED){
			fread(ClrConvData.pInBuf[1], 1, in_size[1], in_file);
			if(!(ClrConvData.nInType == VO_COLOR_YUV_420_PACK || ClrConvData.nInType == VO_COLOR_YUV_420_PACK_2))
				fread(ClrConvData.pInBuf[2], 1, in_size[2], in_file);
		}
//		if(k>500&&k<600)
		{
#ifdef _WIN32_WCE
		enctime_start = GetTickCount();
#endif
		for(i =0; i < 1; i++){
			voClrConv.CCProcess(hCodec, &ClrConvData);
		}
#ifdef _WIN32_WCE
		enctime_start = GetTickCount() - enctime_start;


		fprintf(stats_file, "\n cc %d frame, time: %d ms", i, (int)enctime_start);
#endif
		//fwrite(ClrConvData.pOutBuf[0], 1, out_size[0], out_file);


		if(ClrConvData.nOutType == VO_COLOR_YUV_420_PACK || ClrConvData.nOutType == VO_COLOR_YUV_420_PACK_2){
			int i;

			if (ClrConvData.nOutType == VO_COLOR_YUV_420_PACK_2){
				for(i=0; i < out_size[1]; i+=2){
					fwrite(&ClrConvData.pOutBuf[1][i+1], 1, 1, out_file);
				}
				for(i=0; i < out_size[1]; i+=2){
					fwrite(&ClrConvData.pOutBuf[1][i], 1, 1, out_file);
				}
			}else{
				for(i=0; i < out_size[1]; i+=2){
					fwrite(&ClrConvData.pOutBuf[1][i], 1, 1, out_file);
				}
				for(i=0; i < out_size[1]; i+=2){
					fwrite(&ClrConvData.pOutBuf[1][i+1], 1, 1, out_file);
				}
			}


		}else if(ClrConvData.nOutType == VO_COLOR_YUV_PLANAR420) {
			fwrite(ClrConvData.pOutBuf[0], 1, out_size[0], out_file);
			fwrite(ClrConvData.pOutBuf[1], 1, out_size[1], out_file);
			fwrite(ClrConvData.pOutBuf[2], 1, out_size[2], out_file);
		}

		printf("frame %i\n", k);
		}		
	}

LFinish:
	/* finish color conversion */
	nCcRC = voClrConv.CCUninit(hCodec);	

LExit:
	if (in_file)
		fclose(in_file);
	if (out_file)
		fclose(out_file);
LFreeMem:
	if(pInBuf[0])
		free(pInBuf[0]);
	if(pInBuf[1])
		free(pInBuf[1]);
	if(pInBuf[2])
		free(pInBuf[2]);
	if(pOutBuf[0])
		free(pOutBuf[0]);
	if(pOutBuf[1])
		free(pOutBuf[1]);
	if(pOutBuf[2])
		free(pOutBuf[2]);
	}

	if (rc != 0)
		printf("\nERROR: rc=%d!\n", rc);

	return(rc);
}


#ifdef _WIN32_WCE
int _tmain(int argc, _TCHAR* argv[])
#else
int	main(int argc, char *argv[])
#endif
{
	int rc;
	rc = TestNormal(argc, argv);
	return rc;
} //!< main()

#else

#ifdef _WIN32_WCE
int _tmain(int argc, _TCHAR* argv[])
#else
int	main(int argc, char *argv[])
#endif
{
	int  k, width, height, i, j;
	unsigned char mb_skip[1024*4];

	int in_width, in_height, in_stride, *x_scale_tab, *y_scale_tab, out_width, out_height, out_stride, mb_width, mb_height, out_size;
	unsigned char *in_buf, *out_buf, *dst_start, *dst;
	unsigned char *src_y, *src_u, *src_v;

	VO_IV_RTTYPE rt;
	VO_HANDLE hnd;

	FILE *in_file = NULL;
	FILE *out_file = NULL;
	FILE *stats_file = NULL ;
	VO_VIDEO_BUFFER  pInput;
	VO_VIDEO_BUFFER  pOutput;

	VO_VIDEO_CCRRRAPI voCCRRR;

#ifdef _WIN32_WCE
	double enctime_start, enctime;
	double totalenctime = 0.;

	if(!(stats_file = fopen("SPEED.txt", "wb"))){
		printf("Open statistic file error!");
	}
/*
	in_file=fopen("/yuv_640x480.yuv", "rb");
//	in_file=fopen("/yuv_320x240.yuv", "rb");
//	in_file=fopen("yuv_176x144.yuv", "rb");
	out_file=fopen("/yuv.raw", "w+b");
*/
#endif

#ifdef _WIN32_WCE
	//in_file=fopen("\\Storage Card\\480X230.yuv", "rb");
	//out_file=fopen("\\Storage Card\\320x240__.raw", "w+b");

	//in_file=fopen("\\SDMMC\\bigpic.yuv", "rb");
	//out_file=fopen("\\SDMMC\\120x160.raw", "w+b");
	in_file  = fopen("new.yuv", "rb");
	out_file = fopen("RGB32.raw", "w+b");
#else
/**/
	k = 1;
	if (argc > 1) {
		for ( ; k < argc; k += 2) {
			switch (argv[k][1]) {
				case 'i':
				case 'I':
					if (in_file != NULL)
						fclose(in_file);

					if ((in_file=fopen(argv[k+1], "rb")) == NULL) {
						goto LExit;
					}

					break;

				case 'o':
				case 'O':
					if (out_file != NULL)
						fclose(out_file);
		
					if ((out_file=fopen(argv[k+1], "w+b")) == NULL) {
						goto LExit;
					}

					break;

				default:
					goto LExit;
			} //!< switch(argv[k][1])
		} //!< for(k)
	}else{
//		show_usage();
#ifdef VOANDROID
		if ((in_file=fopen("/sdcard/Demo_VC1_VGA.yuv", "rb")) == NULL) {
			goto LExit;
		}

		if ((out_file=fopen("/sdcard/Demo_VC1_VGA.rgb32", "w+b")) == NULL) {
			goto LExit;
		}

		printf("VOANDROID  ");
#else
		if ((in_file=fopen("E:/MyResource/Video/clips/VC1/YUV_420_YUV_360x240.yuv", "rb")) == NULL) {
			goto LExit;
		}
		if ((out_file=fopen("E:/MyResource/Video/clips/VC1/YUV_420_YUV_360x240.rgb32", "w+b")) == NULL) {
			goto LExit;
		}
#endif

		printf("zou\n");

		//return;
	}

#endif	
	///////////////////
	in_width = 360;
	in_height = 240;
	in_stride = 360;
	//out_width = in_width*3/4;
	//out_height = in_height*3/4;
	out_width = 360;
	out_height = 240;
	rt = VO_RT_DISABLE;//VO_RT_180;//VO_RT_90R;//VO_RT_90L;//VO_RT_DISABLE;

	//in_width = 240;
	//in_height = 320;
	//in_stride = in_width;
	////out_width = in_width;
	////out_height = in_height;
	//out_width = 240;
	//out_height = 320;
	//rt = VO_RT_DISABLE;//VO_RT_180;//VO_RT_90R;//VO_RT_90L;//VO_RT_DISABLE;

	voGetVideoCCRRRAPI(&voCCRRR, NULL);

	memset(mb_skip, 0, 1024*4);
	mb_skip[0] = mb_skip[1] = 0;

	in_buf = (unsigned char*)malloc(in_stride*in_height*3/2);

	if((rt == VO_RT_DISABLE)||(rt == VO_RT_180)){
		//out_stride = out_width*2;
		//out_size = out_height*out_stride;

		out_stride = out_width*4;
		out_size = out_height*out_stride;

		//out_stride = out_width*3;
		//out_size = out_height*out_stride;

		//out_stride = out_width*2;
		//out_size = out_height*out_stride;
	}else{
		//out_stride = out_height*2;
		//out_size = out_width*out_stride;

		out_stride = out_height*4;
		out_size = out_width*out_stride;

		//out_stride = out_height*3;
		//out_size = out_width*out_stride;

	}
	out_buf = (unsigned char*)malloc(out_size);

	fread(in_buf, 1, in_stride*in_height*3/2, in_file);

	src_y = in_buf;
	src_u = in_buf + in_stride * in_height;
	src_v = src_u + ((in_stride * in_height) >> 2);


	pInput.Buffer[0] = src_y;
	pInput.Buffer[1] = src_u;
	pInput.Buffer[2] = src_v;
	pInput.Stride[0] = in_stride;
	pInput.Stride[1] = in_stride/2;
	pInput.Stride[2] = 0;
	pInput.ColorType = VO_COLOR_YUV_PLANAR420;//VO_COLOR_YUV_PLANAR420;//VO_COLOR_YUV_PLANAR420;

	pOutput.Stride[0] = out_stride;
	pOutput.Buffer[0] = out_buf;
	//pOutput.ColorType = VO_COLOR_RGB888_PACKED;//VO_COLOR_RGB565_PACKED;
	pOutput.ColorType = VO_COLOR_ARGB32_PACKED;//VO_COLOR_RGB565_PACKED;//VO_COLOR_YUYV422_PACKED;//VO_COLOR_RGB565_PACKED;


	voCCRRR.Init(&hnd,NULL,NULL,0);

	voCCRRR.SetCCRRSize(hnd, &in_width, &in_height, &out_width, &out_height, rt);//*pRotateType = 	CCRR_ROTATION_DISABLE |	CCRR_ROTATION_90L | CCRR_ROTATION_90R | CCRR_ROTATION_180;
	//voCCRRR.SetColorType(hnd, VO_COLOR_YUV_PLANAR420, VO_COLOR_YUYV422_PACKED);
	voCCRRR.SetColorType(hnd, VO_COLOR_YUV_PLANAR420, VO_COLOR_ARGB32_PACKED);
	//voCCRRR.SetColorType(hnd, VO_COLOR_YUV_PLANAR420, VO_COLOR_RGB888_PACKED);
	//voCCRRR.SetColorType(hnd, VO_COLOR_YUV_PLANAR420, VO_COLOR_RGB565_PACKED);
	//voCCRRR.SetColorType(hnd, VO_COLOR_YUV_PLANAR420, VO_COLOR_RGB565_PACKED);
	

	for(k = 0; k < 1; k ++){
#ifdef _WIN32_WCE
	enctime_start = GetTickCount();
#elif VOANDROID
	enctime_start = timeGetTime();
#endif
		voCCRRR.Process(hnd, &pInput, &pOutput, NULL, 0);
#ifdef _WIN32_WCE
	totalenctime += GetTickCount() - enctime_start;
#elif VOANDROID
	totalenctime += (timeGetTime() - enctime_start);
#endif
		fwrite(out_buf, 1, out_size, out_file);
		fread(in_buf, 1, in_stride*in_height*3/2, in_file);
	}


	voCCRRR.Uninit(hnd);

	//printf(stats_file, "\n cc %d frame, time: %d ms  %2.3f", k, (int)enctime_start,(k*1.0)/totalenctime);


	//fwrite(out_buf, 1, out_size, out_file);

LExit:
	if (in_file)
		fclose(in_file);
	if (out_file)
		fclose(out_file);
	if(in_buf)
		free(in_buf);
	if(out_buf)
		free(out_buf);
	
}

#endif 
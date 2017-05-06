/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

/*!
************************************************************************
*
* \file		voMMCCRRSample.c
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


static	show_usage(void)
{
	printf("\nUsage: voCCSample -i input_file_name -o output_file_name \n");
	printf("\n");
} //!< show_usage()


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

#endif

#ifdef _WIN32_WCE
	in_file  = fopen("bigpic.yuv", "rb");
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
		return;
	}

#endif	
	///////////////////
	in_width = 240;
	in_height = 320;
	in_stride = 240;
	out_width = 640;
	out_height = 480;
	rt = VO_RT_DISABLE;//VO_RT_180;//VO_RT_90R;//VO_RT_90L;//VO_RT_DISABLE;

	voGetVideoCCRRRAPI(&voCCRRR, NULL);

	memset(mb_skip, 0, 1024*4);
	mb_skip[0] = mb_skip[1] = 0;

	in_buf = (unsigned char*)malloc(in_stride*in_height*3/2);

	if((rt == VO_RT_DISABLE)||(rt == VO_RT_180)){
		out_stride = out_width*2;
		out_size = out_height*out_stride;
	}else{
		out_stride = out_height*2;
		out_size = out_width*out_stride;
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
	pInput.Stride[2] = in_stride/2;
	pInput.ColorType = VO_COLOR_YUV_PLANAR420;//VO_COLOR_YUV_PLANAR420;

	pOutput.Stride[0] = out_stride;
	pOutput.Buffer[0] = out_buf;
	pOutput.ColorType = VO_COLOR_RGB565_PACKED;//VO_COLOR_RGB565_PACKED;//VO_COLOR_YUYV422_PACKED;//VO_COLOR_RGB565_PACKED;


	voCCRRR.Init(&hnd,NULL,NULL,0);

	voCCRRR.SetCCRRSize(hnd, &in_width, &in_height, &out_width, &out_height, rt);//*pRotateType = 	//VO_RT_180;//VO_RT_90R;//VO_RT_90L;//VO_RT_DISABLE;
	voCCRRR.SetColorType(hnd, VO_COLOR_YUV_PLANAR420, VO_COLOR_RGB565_PACKED);

#ifdef _WIN32_WCE
	enctime_start = GetTickCount();
#endif

	for(k = 0; k < 15; k ++){
		voCCRRR.Process(hnd, &pInput, &pOutput, NULL, 0);
		fwrite(out_buf, 1, out_size, out_file);
		fread(in_buf, 1, in_stride*in_height*3/2, in_file);
	}
#ifdef _WIN32_WCE
	enctime_start = GetTickCount() - enctime_start;
#endif

	voCCRRR.Uninit(hnd);

#ifdef _WIN32_WCE
	fprintf(stats_file, "\n cc %d frame, time: %d ms", k, (int)enctime_start);
#endif

	fwrite(out_buf, 1, out_size, out_file);

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
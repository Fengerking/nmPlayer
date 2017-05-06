
#include "stdafx.h"

#include <stdio.h>

#include "ClrConv_HTC_API.H"
#include "gx.h"

static void TestFromYUV420();
static void TestResize();

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	/*! Test conversion */
	TestFromYUV420();

    /*! Test resize */
//	TestResize();
	return 0;
}

/* Open this flag to write the data into video memory */
//#define DISPLAY_BUF 

static void TestFromYUV420()
{
	double enctime_start =0, enctime =0, totalenctime = 0;
	int temp;

	FILE *in_file, *out_file, *stat;
	ClrConvData conv_data;
	unsigned char  *outBufA, *outBufB;

	int ySize, uvSize, outSize, i, j, k, tmp;
	double XScale, YScale;
	
	/* Open input file */
	in_file = fopen("/My Documents/foreman_qvga.yuv", "rb");

	/* Open output file */
	out_file = fopen("/testcc.rgb565", "wb");

	/* Open statistic file */
	stat = fopen("/conver_time.txt", "wb");

	/* Configure the conversion structure */
	conv_data.inType	= TP_PLANAR_YUV_420;
	conv_data.outType	= TP_RGB565;
	conv_data.inWidth	= 320;
	conv_data.inHeight	= 240;
	conv_data.inStride 	= 320; /* Byte unit */
	conv_data.outWidth	= 320;
	conv_data.outHeight	= 240;
	conv_data.outStride	= 640; /* Byte unit */
	conv_data.parameter	= ITU_R_BT_601;
	conv_data.isResize  = 0;
	conv_data.isRotationOut = FLIP;

	/*Set the clipped height when do scaling, 
	  it means some rows don't need to do conversion */
	conv_data.clipHeight = 0; 

	/* Calculate scaling factor */
	XScale = (double)conv_data.inWidth / (double)conv_data.outWidth;
	YScale = (double)(conv_data. inHeight - conv_data.clipHeight) / (double)conv_data.outHeight;

	/* Do initialization if it needs to do scaling */
	ccIniClrConv(XScale, YScale, &conv_data);


	/* Calculate the input and output size */
	if(conv_data.inType == TP_YUYV422 || conv_data.inType == TP_YVYU422 ||
		conv_data.inType == TP_UYVY422 || conv_data.inType == TP_VYUY422){
		ySize   = conv_data.inStride * conv_data.inHeight;
		conv_data.inBuf[0]  = (unsigned char *) malloc(ySize   * sizeof(unsigned char));
	}else if(conv_data.inType == TP_PLANAR_YUV_422 || conv_data.inType == TP_PLANAR_YVU_422){
		ySize   = conv_data.inStride * conv_data.inHeight;
		uvSize  = conv_data.inStride * conv_data.inHeight / 2;
		conv_data.inBuf[0]  = (unsigned char *) malloc(ySize   * sizeof(unsigned char));
		conv_data.inBuf[1]  = (unsigned char *) malloc(uvSize  * sizeof(unsigned char));
		conv_data.inBuf[2]  = (unsigned char *) malloc(uvSize  * sizeof(unsigned char));
	}else{
		ySize   = conv_data.inStride * conv_data.inHeight;
		uvSize  = conv_data.inStride * conv_data.inHeight / 4;
		conv_data.inBuf[0]  = (unsigned char *) malloc(ySize   * sizeof(unsigned char));
		conv_data.inBuf[1]  = (unsigned char *) malloc(uvSize  * sizeof(unsigned char));
		conv_data.inBuf[2]  = (unsigned char *) malloc(uvSize  * sizeof(unsigned char));
	}

	if(conv_data.isRotationOut == ROTATION_DISABLE || conv_data.isRotationOut == FLIP){
		outSize = (conv_data.outStride) * (conv_data.outHeight);
	}else{
		outSize = (conv_data.outStride) * (conv_data.outWidth);
	}


#ifndef DISPLAY_BUF /* Output to system buffer */
	outBufA = (unsigned char *) malloc((outSize +128)* sizeof(unsigned char));
	conv_data.outBuf[0] = outBufA;
#else /* Output to video buffer */
	GXOpenDisplay(NULL, GX_FULLSCREEN);
	conv_data.outBuf[0] = (LPBYTE) GXBeginDraw();
#endif


	for (i = 0; i < 10; i++){
		/* Load raw data from the input file */
//		if(i == 0)
		{

			fread(conv_data.inBuf[0], 1, ySize, in_file);
			if(conv_data.inType != TP_YUYV422 && conv_data.inType != TP_YVYU422 &&
				conv_data.inType != TP_UYVY422 && conv_data.inType != TP_VYUY422){
				fread(conv_data.inBuf[1], 1, uvSize, in_file);
				fread(conv_data.inBuf[2], 1, uvSize, in_file);
			}
		}

		enctime_start = GetTickCount();

		/* Do conversion of a frame */
		ccConvAFrame(&conv_data); 

		enctime = GetTickCount() - enctime_start;

		totalenctime += enctime;

		/* Output the data */
		fwrite(conv_data.outBuf[0], 1, outSize, out_file);
		fflush(out_file);

	}

	/* To close the scaling function */
	ccEndClrConv(&conv_data);

	fprintf(stat,"Finish encoding in %d ms", (int)totalenctime);

	/* To free the buffers and close the files */
	if(conv_data.inBuf[0]){
		free(conv_data.inBuf[0]);
		conv_data.inBuf[0] = NULL;
	}
	if(conv_data.inBuf[1]){
		free(conv_data.inBuf[1]);
		conv_data.inBuf[1] = NULL;
	}
	if(conv_data.inBuf[2]){
		free(conv_data.inBuf[2]);
		conv_data.inBuf[2] = NULL;
	}
#ifndef DISPLAY_BUF
	free(outBufA);
#else
	GXEndDraw ();
	GXCloseDisplay();
#endif
	fclose(in_file);
	fclose(out_file);
	fclose(stat);
}

static void TestResize()
{
	double enctime_start =0, enctime =0, totalenctime = 0;
	int temp;

	FILE *in_file, *out_file, *stat;
	ClrConvData conv_data;
	unsigned char  *outBufA, *outBufB;

	int ySize, uvSize, outSize, i, j, k, tmp;
	double XScale, YScale;

	/* Open input file */
	in_file = fopen("/My Documents/foreman_qvga_320x240.rgb565", "rb");

	/* Open output file */
	out_file = fopen("/testcc.rgb565", "wb");

	/* Open statistic file */
	stat = fopen("/conver_time.txt", "wb");

	/* Configure the conversion structure */
	memset(&conv_data, 0, sizeof(ClrConvData));
	conv_data.inType	= TP_RGB565;/* Only support RGB16 now */
	conv_data.outType	= TP_RGB565;
	conv_data.inWidth	= 320;
	conv_data.inHeight	= 240;
	conv_data.inStride 	= 640;
	conv_data.outWidth	= 320;
	conv_data.outHeight	= 240;
	conv_data.outStride	= 640;
	conv_data.parameter	= ITU_R_BT_601;
	conv_data.isResize  = 1;
	conv_data.isRotationOut = ROTATION_DISABLE;
	conv_data.clipHeight = 0;  /*Set the clipped height when do scaling */

	/* Calculate scaling factor */
	XScale = (double)conv_data.inWidth / (double)conv_data.outWidth;
	YScale = (double)conv_data. inHeight	/ (double)conv_data.outHeight;

	/* Calculate the input and output size */
	if(conv_data.inType == TP_YUYV422 || conv_data.inType == TP_YVYU422 ||
		conv_data.inType == TP_UYVY422 || conv_data.inType == TP_VYUY422){
		ySize   = conv_data.inStride * conv_data.inHeight;
		conv_data.inBuf[0]  = (unsigned char *) malloc(ySize   * sizeof(unsigned char));
	}else if(conv_data.inType == TP_PLANAR_YUV_422 || conv_data.inType == TP_PLANAR_YVU_422){
		ySize   = conv_data.inStride * conv_data.inHeight;
		uvSize  = conv_data.inStride * conv_data.inHeight / 2;
		conv_data.inBuf[0]  = (unsigned char *) malloc(ySize   * sizeof(unsigned char));
		conv_data.inBuf[1]  = (unsigned char *) malloc(uvSize  * sizeof(unsigned char));
		conv_data.inBuf[2]  = (unsigned char *) malloc(uvSize  * sizeof(unsigned char));
	}else if(conv_data.inType == TP_RGB565){
		ySize   = conv_data.inStride * conv_data.inHeight;
		conv_data.inBuf[0]  = (unsigned char *) malloc(ySize   * sizeof(unsigned char));
	}else{
		ySize   = conv_data.inStride * conv_data.inHeight;
		uvSize  = conv_data.inStride * conv_data.inHeight / 4;
		conv_data.inBuf[0]  = (unsigned char *) malloc(ySize   * sizeof(unsigned char));
		conv_data.inBuf[1]  = (unsigned char *) malloc(uvSize  * sizeof(unsigned char));
		conv_data.inBuf[2]  = (unsigned char *) malloc(uvSize  * sizeof(unsigned char));
	}


	if(conv_data.isRotationOut == ROTATION_DISABLE){
		outSize = (conv_data.outStride) * (conv_data.outHeight);
	}else{
		outSize = (conv_data.outStride) * (conv_data.outWidth);
	}


#ifndef DISPLAY_BUF  /* Output to system buffer */
	outBufA = (unsigned char *) malloc((outSize +128)* sizeof(unsigned char));
	conv_data.outBuf[0] = outBufA;
#else /* Output to video buffer */
	tmp = GXOpenDisplay(NULL, GX_FULLSCREEN);
	conv_data.outBuf[0] = (LPBYTE) GXBeginDraw();
#endif

	/* Do initialization if it needs to do scaling */
	ccIniClrConv(XScale, YScale, &conv_data);


	for (i = 0; i < 1500; i++){
		/* Load raw data from the input file */
//		if(i == 0)
		{

			fread(conv_data.inBuf[0], 1, ySize, in_file);
			if(conv_data.inType != TP_YUYV422 && conv_data.inType != TP_YVYU422 &&
				conv_data.inType != TP_UYVY422 && conv_data.inType != TP_VYUY422 && 
				conv_data.inType != TP_RGB565){
				fread(conv_data.inBuf[1], 1, uvSize, in_file);
				fread(conv_data.inBuf[2], 1, uvSize, in_file);
			}
		}

		enctime_start = GetTickCount();

		/* Do scaling of a frame */
		ccResizeAFrame(&conv_data); 

		enctime = GetTickCount() - enctime_start;

		totalenctime += enctime;

		/* Output the data */
//		fwrite(conv_data.outBuf[0], 1, outSize, out_file);
		fflush(out_file);

	}

	/* To close the scaling function */
	ccEndClrConv(&conv_data);
	fprintf(stat,"Finish encoding %d frames in %d ms", i, (int)totalenctime);

	/* To free the buffers and close the files */
	if(conv_data.inBuf[0]){
		free(conv_data.inBuf[0]);
		conv_data.inBuf[0] = NULL;
	}
	if(conv_data.inBuf[1]){
		free(conv_data.inBuf[1]);
		conv_data.inBuf[1] = NULL;
	}
	if(conv_data.inBuf[2]){
		free(conv_data.inBuf[2]);
		conv_data.inBuf[2] = NULL;
	}
#ifndef DISPLAY_BUF
	free(outBufA);
#else
	GXEndDraw ();
	GXCloseDisplay();
#endif

	fclose(in_file);
	fclose(out_file);
	fclose(stat);

}



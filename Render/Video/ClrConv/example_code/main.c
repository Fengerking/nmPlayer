
// Windows Header Files:
//#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include "ClrConv_HTC_API.H"
//#include "gx.h"


static void TestFromYUV420();
static void TestRGB16Rotation();
static void TestYUVRotation();
static void TestResize();
#ifdef WIN32
void main()
#else
int WinMain()
#endif
{
//	TestFromYUV420();
//	TestRGB16Rotation();
//	TestYUVRotation();
	TestResize();
	return 0;
}


void yuv2nv12(int width, int height, unsigned char *u, unsigned char *v, unsigned char *nv)
{
	int i, j, size;

	size = width*height;

	for(i = 0; i < size; i ++){
		*(nv++) = *(u++) ;
		*(nv++) = *(v++) ;
	}
}


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
#ifdef WIN32
//	in_file = fopen("E:\\test\\testcc1.rgb565", "rb");
	in_file = fopen("E:\\test\\foreman_qvga.yuv", "rb");
	out_file = fopen("e:\\test\\testcc.rgb565", "wb");
#else
	in_file = fopen("/My Documents/foreman_qvga_nv12.yuv", "rb");
	out_file = fopen("/testcc.rgb565", "wb");
#endif
	stat = fopen("/conver_time.txt", "wb"); 

	memset(&conv_data, 0, sizeof(ClrConvData));
	conv_data.inType	= YUV_PLANAR420;///UYVY422_PACKED;//YUV_PLANAR420;
	conv_data.outType	= TP_RGB565;
	conv_data.inWidth	= 320;
	conv_data.inHeight	= 240;
	conv_data.inStride 	= 320;
	conv_data.outWidth	= 320;
	conv_data.outHeight	= 240;
	conv_data.outStride	= 320*2;
	conv_data.parameter	= ITU_R_BT_601;
	conv_data.isResize  = 1;
	conv_data.isRotationOut = ROTATION_DISABLE;//FLIP;//ROTATION_90L;//ROTATION_DISABLE;
	conv_data.clipHeight = 16;//setted when do scaling
	XScale = (double)conv_data.inWidth / (double)conv_data.outWidth;
//	YScale = (double)conv_data. inHeight / (double)conv_data.outHeight;
	YScale = (double)(conv_data. inHeight - conv_data.clipHeight) / (double)conv_data.outHeight;

	if(conv_data.inType == TP_YUYV422 || conv_data.inType == YVYU422_PACKED ||
		conv_data.inType == UYVY422_PACKED || conv_data.inType == TP_VYUY422){
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
	}else if(conv_data.inType == YUVNV12){
		ySize   = conv_data.inStride * conv_data.inHeight;
		uvSize  = conv_data.inStride * conv_data.inHeight / 2;
		conv_data.inBuf[0]  = (unsigned char *) malloc(ySize   * sizeof(unsigned char));
		conv_data.inBuf[1]  = (unsigned char *) malloc(uvSize  * sizeof(unsigned char));
	}else{
		ySize   = conv_data.inStride * conv_data.inHeight;
		uvSize  = conv_data.inStride * conv_data.inHeight / 4;
		conv_data.inBuf[0]  = (unsigned char *) malloc(ySize   * sizeof(unsigned char));
		conv_data.inBuf[1]  = (unsigned char *) malloc(uvSize  * sizeof(unsigned char));
		conv_data.inBuf[2]  = (unsigned char *) malloc(uvSize  * sizeof(unsigned char));
	}


	if(conv_data.isRotationOut == ROTATION_DISABLE || conv_data.isRotationOut == FLIP_Y){
		outSize = (conv_data.outStride) * (conv_data.outHeight);
	}else{
		outSize = (conv_data.outStride) * (conv_data.outWidth);
	}



#ifndef DISPLAY_BUF 
	conv_data.outBuf[0] = (unsigned char *) malloc((outSize +128)* sizeof(unsigned char));
#else
	tmp = GXOpenDisplay(NULL, GX_FULLSCREEN);
	conv_data.outBuf[0] = (LPBYTE) GXBeginDraw();
#endif
#if 1

//	if(conv_data.isResize)
	ccIniClrConv(XScale, YScale, &conv_data);


	for (i = 0; i < 10; i++){
//		if(i == 0)
		{

			fread(conv_data.inBuf[0], 1, ySize, in_file);
			if(conv_data.inType != TP_YUYV422 && conv_data.inType != YVYU422_PACKED &&
				conv_data.inType != UYVY422_PACKED && conv_data.inType != TP_VYUY422 && 
				conv_data.inType != TP_RGB565){
				fread(conv_data.inBuf[1], 1, uvSize, in_file);
				if(conv_data.inType != YUVNV12)
					fread(conv_data.inBuf[2], 1, uvSize, in_file);
			}
		}
#ifndef WIN32
		enctime_start = GetTickCount();
#endif
		ccConvAFrame(&conv_data); 

//		ccResizeAFrame(&conv_data); 
#ifndef WIN32
		enctime = GetTickCount() - enctime_start;
#endif
		totalenctime += enctime;

		fwrite(conv_data.outBuf[0], 1, outSize, out_file);
//		fwrite(conv_resize_rt.inBuf[0], 1, 320*240, out_file);
//		fwrite(conv_resize_rt.inBuf[2], 1, 320*240/4, out_file);
//		fwrite(conv_resize_rt.inBuf[1], 1, 320*240/4, out_file);
//		fwrite(conv_data.inBuf[0], 1, 320*240, out_file);
//		fwrite(conv_data.outBuf[0], 1, 320*240/2, out_file);
		fflush(out_file);
//		ccEndResize();

//		conv_data.outWidth	= 176;
//		conv_data.outHeight	= 144;
//		conv_data.outStride	= 352;
//		XScale = (double)conv_data.inWidth / (double)conv_data.outWidth;
//		YScale = (double)conv_data.inHeight	/ (double)conv_data.outHeight;
//	if(conv_data.isResize)
//		ccIniClrConv(XScale, YScale, conv_data.outWidth, conv_data.outHeight);

	}

	ccEndClrConv(&conv_data);
	fprintf(stat,"Finish encoding in %d ms", (int)totalenctime);
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
	if(conv_data.outBuf[0]){
		free(conv_data.outBuf[0]);
		conv_data.outBuf[0] = NULL;
	}
#else
	GXEndDraw ();
	GXCloseDisplay();
#endif
//	free(conv_data.inBuf);
//	free(conv_data.outBuf);
	fclose(in_file);
	fclose(out_file);
	fclose(stat);

#endif
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
#ifdef WIN32
	in_file = fopen("E:\\test\\HTC_CC\\testRGB.rgb565", "rb");
//	in_file = fopen("E:\\test\\Clips\\qvga\\foreman_qvga.yuv", "rb");
	out_file = fopen("E:\\test\\HTC_CC\\testRGBN.rgb565", "wb");
#else
	in_file = fopen("/My Documents/foreman_qvga.yuv", "rb");
	out_file = fopen("/testcc.rgb565", "wb");
#endif
	stat = fopen("/conver_time.txt", "wb");

	memset(&conv_data, 0, sizeof(ClrConvData));
	conv_data.inType	= RGB565_PACKED;///UYVY422_PACKED;//YUV_PLANAR420;
	conv_data.outType	= RGB565_PACKED;
	conv_data.inWidth	= 240;
	conv_data.inHeight	= 320;
	conv_data.inStride 	= 240*2;
	conv_data.outWidth	= 240;
	conv_data.outHeight	= 320;
	conv_data.outStride	= 320*2;
	conv_data.parameter	= ITU_R_BT_601;
	conv_data.isResize  = 0;
	conv_data.clipHeight= 0;
	conv_data.isRotationOut = ROTATION_90L;//ROTATION_DISABLE;//ROTATION_90L;//ROTATION_DISABLE;
	XScale = (double)conv_data.inWidth / (double)conv_data.outWidth;
	YScale = (double)(conv_data. inHeight - conv_data.clipHeight) / (double)conv_data.outHeight;


	if(conv_data.inType == TP_YUYV422 || conv_data.inType == YVYU422_PACKED ||
		conv_data.inType == UYVY422_PACKED || conv_data.inType == TP_VYUY422){
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


	if(conv_data.isRotationOut == ROTATION_DISABLE || conv_data.isRotationOut == FLIP_Y){
		outSize = (conv_data.outStride) * (conv_data.outHeight);
	}else{
		outSize = (conv_data.outStride) * (conv_data.outWidth);
	}


#ifndef DISPLAY_BUF 
	conv_data.outBuf[0] = (unsigned char *) malloc((outSize)* sizeof(unsigned char));
	if(conv_data.outType == YUV_PLANAR420){
		conv_data.outBuf[1] = (unsigned char *) malloc((outSize/4)* sizeof(unsigned char));
		conv_data.outBuf[2] = (unsigned char *) malloc((outSize/4)* sizeof(unsigned char));
	}
#else
	tmp = GXOpenDisplay(NULL, GX_FULLSCREEN);
	conv_data.outBuf[0] = (LPBYTE) GXBeginDraw();
#endif
#if 1

//	if(conv_data.isResize)
		ccIniClrConv(XScale, YScale, &conv_data);


	for (i = 0; i < 10; i++){
//		if(i == 0)
		{

			fread(conv_data.inBuf[0], 1, ySize, in_file);
			if(conv_data.inType != TP_YUYV422 && conv_data.inType != YVYU422_PACKED &&
				conv_data.inType != UYVY422_PACKED && conv_data.inType != TP_VYUY422 && 
				conv_data.inType != TP_RGB565){
				fread(conv_data.inBuf[1], 1, uvSize, in_file);
				fread(conv_data.inBuf[2], 1, uvSize, in_file);
			}
		}
#ifndef WIN32
		enctime_start = GetTickCount();
#endif
//		ccConvAFrame(&conv_data); 
		ccResizeAFrame(&conv_data); 
#ifndef WIN32
		enctime = GetTickCount() - enctime_start;
#endif
		totalenctime += enctime;

		if(conv_data.outBuf[0])
			fwrite(conv_data.outBuf[0], 1, outSize, out_file);
		if(conv_data.outBuf[1])
			fwrite(conv_data.outBuf[1], 1, outSize/4, out_file);
		if(conv_data.outBuf[2])
			fwrite(conv_data.outBuf[2], 1, outSize/4, out_file);
//		fwrite(conv_data.outBuf[0], 1, 304*240*3/2, out_file);
		fflush(out_file);
//		ccEndClrConv(&conv_data);

//		conv_data.outWidth	= 176;
//		conv_data.outHeight	= 144;
//		conv_data.outStride	= 352;
//		XScale = (double)conv_data.inWidth / (double)conv_data.outWidth;
//		YScale = (double)conv_data.inHeight	/ (double)conv_data.outHeight;
//	if(conv_data.isResize)
//		ccIniClrConv(XScale, YScale, conv_data.outWidth, conv_data.outHeight);

	}

	ccEndClrConv(&conv_data);
	fprintf(stat,"Finish encoding in %d ms", (int)totalenctime);
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
	if(conv_data.outBuf[0])
		free(conv_data.outBuf[0]);
	if(conv_data.outBuf[1])
		free(conv_data.outBuf[1]);
	if(conv_data.outBuf[2])
		free(conv_data.outBuf[2]);
#else
	GXEndDraw ();
	GXCloseDisplay();
#endif
//	free(conv_data.inBuf);
//	free(conv_data.outBuf);
	fclose(in_file);
	fclose(out_file);
	fclose(stat);

#endif
}


static void RGB16Rt90L(unsigned short *src, unsigned short *dst, const int width, const int height)
{

	unsigned int *p_src0 = (unsigned int *)src;
	unsigned int *p_src1 = (unsigned int *)(src+width);

	unsigned short *start_out = dst + (width - 1)*height;
	unsigned int a, b, c, d, e, f;

	unsigned int vx, vy;

	for(vy = 0; vy < height; vy+=2) {
		unsigned short *out_dst = start_out + vy;

		for(vx = 0; vx < width; vx+=16) {
			a = *(p_src0++);			//2p
			b = *(p_src1++);
			c = *(p_src0++);			//4p
			d = *(p_src1++);

			e = a&0xFFFF;
			f = (b&0xFFFF)<<16;
			e = e|f;//
			(*(unsigned int *)out_dst) = e;	//0
			out_dst = out_dst - height;

			e = (a>>16);
			f = b&0xFFFF0000;

			a = *(p_src0++);			//6p			
			b = *(p_src1++);

			e = e|f;
			(*(unsigned int *)out_dst) = e;	//1
			out_dst = out_dst - height;
			


			e = c&0xFFFF;
			f = (d&0xFFFF)<<16;
			e = e|f;
			(*(unsigned int *)out_dst) = e;	//2
			out_dst = out_dst - height;

			
			e = (c>>16);
			f = d&0xFFFF0000;

			c = *(p_src0++);			//8p
			d = *(p_src1++);

			e = e|f;
			(*(unsigned int *)out_dst) = e;	//3
			out_dst = out_dst - height;



			e = a&0xFFFF;
			f = (b&0xFFFF)<<16;
			e = e|f;//
			(*(unsigned int *)out_dst) = e;	//4
			out_dst = out_dst - height;
			
			e = (a>>16);
			f = b&0xFFFF0000;

			a = *(p_src0++);			//10p
			b = *(p_src1++);

			e = e|f;
			(*(unsigned int *)out_dst) = e;	//5
			out_dst = out_dst - height;



			e = c&0xFFFF;
			f = (d&0xFFFF)<<16;
			e = e|f;
			(*(unsigned int *)out_dst) = e;	//6
			out_dst = out_dst - height;

			
			e = (c>>16);
			f = d&0xFFFF0000;

			c = *(p_src0++);			//12p
			d = *(p_src1++);

			e = e|f;
			(*(unsigned int *)out_dst) = e;	//7
			out_dst = out_dst - height;


			e = a&0xFFFF;
			f = (b&0xFFFF)<<16;
			e = e|f;//
			(*(unsigned int *)out_dst) = e;	//8
			out_dst = out_dst - height;
			
			e = (a>>16);
			f = b&0xFFFF0000;

			a = *(p_src0++);			//14p
			b = *(p_src1++);

			e = e|f;
			(*(unsigned int *)out_dst) = e;	//9
			out_dst = out_dst - height;



			e = c&0xFFFF;
			f = (d&0xFFFF)<<16;
			e = e|f;
			(*(unsigned int *)out_dst) = e;	//10
			out_dst = out_dst - height;

			
			e = (c>>16);
			f = d&0xFFFF0000;

			c = *(p_src0++);			//16p
			d = *(p_src1++);

			e = e|f;
			(*(unsigned int *)out_dst) = e;	//11
			out_dst = out_dst - height;


			e = a&0xFFFF;
			f = (b&0xFFFF)<<16;
			e = e|f;//
			(*(unsigned int *)out_dst) = e;	//12
			out_dst = out_dst - height;
			
			e = (a>>16);
			f = b&0xFFFF0000;
			e = e|f;
			(*(unsigned int *)out_dst) = e;	//13
			out_dst = out_dst - height;

			e = c&0xFFFF;
			f = (d&0xFFFF)<<16;
			e = e|f;
			(*(unsigned int *)out_dst) = e;	//14
			out_dst = out_dst - height;

			
			e = (c>>16);
			f = d&0xFFFF0000;
			e = e|f;
			(*(unsigned int *)out_dst) = e;	//15
			out_dst = out_dst - height;

		}
		p_src0 += (width>>1);
		p_src1 += (width>>1);

	}
}

static void RGB16DownSizeRt90L(unsigned short *src, unsigned short *dst, const int width, const int height, int out_stride)
{

	unsigned int *p_src0 = (unsigned int *)src;
	unsigned int *p_src1 = (unsigned int *)(src+(width<<1));

	unsigned short *start_out = dst + ((width>>1) - 1)*(out_stride>>1);
	unsigned int a, b, c, d, e, f;

	unsigned int vx, vy;
	out_stride = out_stride>>1;

	for(vy = 0; vy < height; vy+=4) {
		unsigned short *out_dst = start_out;

		for(vx = 0; vx < width; vx+=16) {
			a = *(p_src0++);			//2p
			b = *(p_src1++);
			c = *(p_src0++);			//4p
			d = *(p_src1++);

			e = a&0xFFFF;
			f = (b&0xFFFF)<<16;
			e = e|f;//
			(*(unsigned int *)out_dst) = e;	//0
			out_dst = out_dst - out_stride;

			a = *(p_src0++);			//6p			
			b = *(p_src1++);			

			e = c&0xFFFF;
			f = (d&0xFFFF)<<16;
			e = e|f;
			(*(unsigned int *)out_dst) = e;	//2
			out_dst = out_dst - out_stride;



			c = *(p_src0++);			//8p
			d = *(p_src1++);

			e = a&0xFFFF;
			f = (b&0xFFFF)<<16;
			e = e|f;//
			(*(unsigned int *)out_dst) = e;	//4
			out_dst = out_dst - out_stride;
			

			a = *(p_src0++);			//10p
			b = *(p_src1++);

			e = c&0xFFFF;
			f = (d&0xFFFF)<<16;
			e = e|f;
			(*(unsigned int *)out_dst) = e;	//6
			out_dst = out_dst - out_stride;


			c = *(p_src0++);			//12p
			d = *(p_src1++);

			e = a&0xFFFF;
			f = (b&0xFFFF)<<16;
			e = e|f;//
			(*(unsigned int *)out_dst) = e;	//8
			out_dst = out_dst - out_stride;
			

			a = *(p_src0++);			//14p
			b = *(p_src1++);

			e = c&0xFFFF;
			f = (d&0xFFFF)<<16;
			e = e|f;
			(*(unsigned int *)out_dst) = e;	//10
			out_dst = out_dst - out_stride;


			c = *(p_src0++);			//16p
			d = *(p_src1++);

			e = a&0xFFFF;
			f = (b&0xFFFF)<<16;
			e = e|f;//
			(*(unsigned int *)out_dst) = e;	//12
			out_dst = out_dst - out_stride;
			

			e = c&0xFFFF;
			f = (d&0xFFFF)<<16;
			e = e|f;
			(*(unsigned int *)out_dst) = e;	//14
			out_dst = out_dst - out_stride;

		}
		p_src0 += width + (width>>1);
		p_src1 += width + (width>>1);
		start_out += 2;

	}
}

static void TestRGB16Rotation()
{
	double enctime_start =0, enctime =0, totalenctime = 0;
	int temp;

	FILE *in_file, *out_file, *stat;

	unsigned short *inBufA, *outBufB;

	int ySize, uvSize, outSize, i, j, k, tmp, inSize;
	int width, height;
//#ifdef WIN32
	in_file = fopen("E:\\test\\clips\\qvga\\foreman_qvga.yuv", "rb");
	out_file = fopen("e:\\test\\testcc22.yuv", "wb");
//#else
//	in_file = fopen("/My Documents/foreman_qvga.raw", "rb");
//	out_file = fopen("/testcc.rgb565", "wb");
//#endif
	stat = fopen("/conver_time.txt", "wb");


	width = 320;
	height = 240;

	outSize = width * (height+4)>>2;
	inSize = width * height;

	inBufA = (unsigned short *) malloc(inSize * sizeof(unsigned short));
	outBufB = (unsigned short *) malloc(outSize * sizeof(unsigned short));

	for (i = 0; i < 10; i++){
//		if(i == 0)
		{
			fread(inBufA, 1, inSize*2, in_file);
		}
#ifndef WIN32
		enctime_start = GetTickCount();
#endif
//		RGB16Rotation90L(inBufA, outBufB, width, height);
		RGB16DownSizeRt90L(inBufA, outBufB, width, height, height+4);
#ifndef WIN32
		enctime = GetTickCount() - enctime_start;
#endif
		totalenctime += enctime;

		fwrite(outBufB, 1, outSize*2, out_file);
		fflush(out_file);

	}

	fprintf(stat,"Finish encoding in %d ms", (int)totalenctime);


	free(inBufA);
	free(outBufB);


	fclose(in_file);
	fclose(out_file);
	fclose(stat);

}

static void TestYUVRotation()
{
	double enctime_start =0, enctime =0, totalenctime = 0;
	int temp;

	FILE *in_file, *out_file, *stat;

	unsigned short *inBufA, *outBufB;

	int ySize, uvSize, outSize, i, j, k, tmp, inSize;
	int width, height;

	in_file = fopen("E:\\test\\clips\\qvga\\foreman_qvga.yuv", "rb");
	out_file = fopen("e:\\test\\testcc22.yuv", "wb");
	stat = fopen("/conver_time.txt", "wb");


	width = 320;
	height = 240;

	outSize = width * height;
	inSize = width * height;

	inBufA = (unsigned char *) malloc(inSize * sizeof(unsigned char));
	outBufB = (unsigned char *) malloc(outSize * sizeof(unsigned char));

	for (i = 0; i < 10; i++){
//		if(i == 0)
		{
			fread(inBufA, 1, inSize, in_file);
		}

		YUVPlanarRt90L(inBufA, outBufB, width, height, width);
		fwrite(outBufB, 1, outSize, out_file);
		fflush(out_file);

//		if(i == 0)
		{
			fread(inBufA, 1, inSize>>2, in_file);
		}

		YUVPlanarRt90L(inBufA, outBufB, width>>1, height>>1, width>>1);
		fwrite(outBufB, 1, outSize>>2, out_file);
		fflush(out_file);


//		if(i == 0)
		{
			fread(inBufA, 1, inSize>>2, in_file);
		}

		YUVPlanarRt90L(inBufA, outBufB, width>>1, height>>1, width>>1);
		fwrite(outBufB, 1, outSize>>2, out_file);
		fflush(out_file);


	}

	fprintf(stat,"Finish encoding in %d ms", (int)totalenctime);


	free(inBufA);
	free(outBufB);


	fclose(in_file);
	fclose(out_file);
	fclose(stat);

}
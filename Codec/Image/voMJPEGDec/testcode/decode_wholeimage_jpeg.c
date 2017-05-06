#ifndef LINUX
#include <tchar.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "voJPEG.h"


#ifdef LINUX
#pragma   pack(1)
#else
#pragma pack(push, 1)
#endif
typedef unsigned char       BYTE;
typedef unsigned long       DWORD;
typedef unsigned short      WORD;
typedef long LONG;
typedef char CHAR;
typedef CHAR *LPSTR, *PSTR;
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
typedef struct tagRGBQUAD  {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;
typedef struct tagBITMAPFILEHEADER {
        unsigned short    bfType;
        unsigned int   bfSize;
        unsigned short    bfReserved1;
        unsigned short    bfReserved2;
        unsigned int   bfOffBits;
} BITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO;
#ifdef LINUX
#pragma   pack() 
#else
#pragma pack(pop)
#endif


#define ConstV1 (0x198937>>8) //1673527
#define ConstV2 (0xD020C>>8)  //852492
#define ConstU1 (0x2045A1>>8) //2114977
#define ConstU2 (0x645A1>>8)  //411041
#define ConstY	(0x129FBE>>8) //1220542

#define SAT(Value) (Value) < 0 ? 0: ((Value) > 255 ? 255: (Value));

void writebmpfile(char * filename,unsigned char* image,long width,long height,long stride,long componet)
{
	int i=0;
	BITMAPINFO info;
	BITMAPFILEHEADER bmphead;			
	FILE * fp = fopen(filename,"wb");

	bmphead.bfType = 0x4d42;
	bmphead.bfSize = width*height*componet+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO);//+1078;
	bmphead.bfReserved1 = 0;
	bmphead.bfReserved2 = 0;
	bmphead.bfOffBits = 54;//1078;

	//info=(BITMAPINFO*)malloc(sizeof(BITMAPINFO));//sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD));//1064
	info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); //40
	info.bmiHeader.biBitCount = componet*8;
	info.bmiHeader.biClrImportant =0;
	info.bmiHeader.biClrUsed = 0;
	info.bmiHeader.biWidth = width;
	info.bmiHeader.biHeight = -height;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biSizeImage = width*height*componet;//imgData.pBmpInfoHead->biSizeImage;
	info.bmiHeader.biXPelsPerMeter =0;
	info.bmiHeader.biYPelsPerMeter =0;	


    info.bmiColors->rgbBlue = 0x001F ;
    info.bmiColors->rgbGreen = 0x07E0 ;
    info.bmiColors->rgbRed = 0xF800;
    info.bmiColors->rgbReserved = 0;


	fwrite((LPSTR)&bmphead, sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(&info,sizeof(BITMAPINFOHEADER),1, fp);

	for(i=0;i<height;i++)
	{
		unsigned char* prt= image+i*stride;
		fwrite(prt,1,stride,fp);
	}
	fclose(fp);
	//free(info);
	return;
}

//void writebmpfile(char * filename,unsigned char* image,long width,long height)
//{
//	BITMAPINFO *info;
//	BITMAPFILEHEADER bmphead;			
//	FILE * fp = fopen(filename,"wb");
//
//	bmphead.bfType = 0x4d42;
//	bmphead.bfSize = width*height*3+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO);//+1078;
//	bmphead.bfReserved1 = 0;
//	bmphead.bfReserved2 = 0;
//	bmphead.bfOffBits = 54;//1078;
//
//	info=(BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER));//sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD));//1064
//	info->bmiHeader.biSize=sizeof(BITMAPINFOHEADER); //40
//	info->bmiHeader.biBitCount = 24;
//	info->bmiHeader.biClrImportant =0;
//	info->bmiHeader.biClrUsed = 0;
//	info->bmiHeader.biWidth = width;
//	info->bmiHeader.biHeight = -height;
//	info->bmiHeader.biCompression = BI_RGB;
//	info->bmiHeader.biPlanes = 1;
//	info->bmiHeader.biSizeImage = width*height*3;//imgData.pBmpInfoHead->biSizeImage;
//	info->bmiHeader.biXPelsPerMeter =0;
//	info->bmiHeader.biYPelsPerMeter =0;	
//
//	fwrite((LPSTR)&bmphead, sizeof(BITMAPFILEHEADER),1,fp);
//	fwrite(info,sizeof(BITMAPINFOHEADER),1, fp);
//
//	fwrite(image,width*height*3,1,fp);
//	fclose(fp);
//	free(info);
//	return;
//}
void cc_rgb888p_torgb24_c_16bit(unsigned char *psrc_r, unsigned char *psrc_g, unsigned char *psrc_b, const signed long in_stridey, 
						unsigned char *out_buf, const signed long out_stride, signed long width, signed long height,
						const signed long in_strideu, const signed long in_stridev)
{
	int i,j;
	for(i = 0; i < height; i ++)
	{
		unsigned char* r = psrc_r+in_stridey*i;
		unsigned char* g = psrc_g+in_stridey*i;
		unsigned char* b = psrc_b+in_stridey*i;
		unsigned char* outptr = out_buf + out_stride*i;
		for(j = 0; j < width; j ++)
		{
			*(outptr) = *b++;
			*(outptr+1) = *g++;;
			*(outptr+2) =*r++;

			outptr +=3;
		}
	}

}

void cc_yuv420torgb24_c_16bit(unsigned char *psrc_y, unsigned char *psrc_u, unsigned char *psrc_v, const signed long in_stridey, 
						unsigned char *out_buf, const signed long out_stride, signed long width, signed long height,
						const signed long in_strideu, const signed long in_stridev)
{
	signed long  a0, a1, a2, a3, a4, a5, a7, a33, a77;
	unsigned long  i, j;
	for(i = 0; i < height; i += 2){

		for(j = 0; j < width; j += 2){
			a0 = psrc_u[0] - 128;
			a1 = psrc_v[0] - 128;
			a2 = (a0 * ConstU1);
			a0 = (a1 * ConstV2 + a0 *ConstU2);
			a1 = (a1 * ConstV1);

			a3 = (psrc_y[0] - 16)*ConstY;
			a33 = (psrc_y[1] - 16)*ConstY;

			a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
			a77 = (psrc_y[in_stridey+1] - 16)*ConstY;

			a4  = SAT((a3 + a1)>>12);
			a5  = SAT((a3 - a0)>>12);
			a3  = SAT((a3 + a2)>>12);
			*(out_buf) = a3;
			*(out_buf+1) = a5;
			*(out_buf+2) = a4;

			a4  = SAT((a33 + a1)>>12);
			a5  = SAT((a33 - a0)>>12);
			a33  = SAT((a33 + a2)>>12);
			*(out_buf+3) = a33;
			*(out_buf+4) = a5;
			*(out_buf+5) = a4;

			a4  = SAT((a7 + a1)>>12);
			a5  = SAT((a7 - a0)>>12);
			a7  = SAT((a7 + a2)>>12);
			*(out_buf+0+out_stride) = a7;
			*(out_buf+1+out_stride) = a5;
			*(out_buf+2+out_stride) = a4;

			a4  = SAT((a77 + a1)>>12);
			a5  = SAT((a77 - a0)>>12);
			a77  = SAT((a77 + a2)>>12);
			*(out_buf+3+out_stride) = a77;
			*(out_buf+4+out_stride) = a5;
			*(out_buf+5+out_stride) = a4;

			out_buf += 6;
			psrc_y += 2;
			psrc_u++;
			psrc_v++;

		}
		psrc_y -= width;
		psrc_y += (in_stridey<<1);

		psrc_u -= (width>>1);
		psrc_u += in_strideu;

		psrc_v -= (width>>1);
		psrc_v += in_stridev;

		out_buf -= (width*3);
		out_buf += (out_stride<<1);
	}
}

void cc_rgb24torgb16_c_16bit(unsigned char *psrc_rgb, 
						unsigned char *out_buf, const signed long width,signed long height)

{
    unsigned long  i, j;
    unsigned short *pdist = (unsigned short*)out_buf;
    for(i=0;i<height;i++)
    {
         for(j=0;j<width;j++)
         {
             unsigned char r = *(psrc_rgb++);
             unsigned char g = *(psrc_rgb++);
             unsigned char b = *(psrc_rgb++);
             unsigned short rgb565=0;
             r = r>>3;
             g = g>>2;
             b = b>>3;
             rgb565 = r + (g<<5) + (b<<(5+6));

             *(pdist++) =  rgb565;

         }
    }
}


/*/rgb5652rgb888(unsigned char *image,unsigned char *image888)
{
unsigned char R,G,B;
B=(*image) & 0x1F;//000BBBBB
G=( *(image+1) << 3 ) & 0x38 + ( *image >> 5 ) & 0x07 ;//得到00GGGGGG00
R=( *(image+1) >> 3 ) & 0x1F; //得到000RRRRR
*(image888+0)=B * 255 / 63; // 把5bits映射到8bits去，自己可以优化一下算法，下同
*(image888+1)=G * 255 / 127;
*(image888+2)=R * 255 / 63;
}

*/
#define RGB565_MASK_RED        0xF800
#define RGB565_MASK_GREEN    0x07E0
#define RGB565_MASK_BLUE      0x001F

void cc_rgb16torgb24_c_16bit(unsigned char *psrc_rgb16, 
						unsigned char *out_buf24, const signed long width,signed long height)

{
    unsigned long  i, j;
    unsigned short *psrc = (unsigned short*)psrc_rgb16;
    for(i=0;i<height;i++)
    {
         for(j=0;j<width;j++)
         {
             unsigned char B = (*psrc & RGB565_MASK_RED) >> 11;  
             unsigned char G = (*psrc & RGB565_MASK_GREEN) >> 5;
             unsigned char R = (*psrc & RGB565_MASK_BLUE);

             //amplify the image
             *out_buf24++ = (R<<3);
             *out_buf24++ = (G<<2);
             *out_buf24++ = (B<<3);
             psrc++;
         }
    }
}


int  UpdataBuffer(VO_VIDEO_BUFFER *pOutData, int width, int height)
{
	int i;
	int yuv_width[3];  
	int	yuv_height[3];
	int	yuv_size[3];
	switch(pOutData->ColorType)
	{
	case VO_COLOR_RGB888_PLANAR:
	case VO_COLOR_YUV_PLANAR444:		/*!< YUV planar mode:444  vertical sample is 1, horizontal is 1  */
		yuv_width[0] = yuv_width[1] = yuv_width[2] = (width + 15) & ~15;
		yuv_height[0] = yuv_height[1] = yuv_height[2] = (height + 15) & ~15;
		break;
	case VO_COLOR_YUV_PLANAR422_12:	/*!< YUV planar mode:422, vertical sample is 1, horizontal is 2  */
		yuv_width[0] = (width + 15) & ~15;
		yuv_width[1] = yuv_width[2] = yuv_width[0]/2;
		yuv_height[0] = yuv_height[1] = yuv_height[2] = (height + 15) & ~15;
		break;
	case VO_COLOR_YUV_PLANAR422_21:	/*!< YUV planar mode:422  vertical sample is 2, horizontal is 1  */
		yuv_width[0] = yuv_width[1] = yuv_width[2] = (width + 15) & ~15;
		yuv_height[0] = (height + 15) & ~15;
		yuv_height[1] = yuv_height[2] = yuv_height[0]/2;
		break;
	case VO_COLOR_YUV_PLANAR420:		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  */
		yuv_width[0] = (width + 31) & ~31;
		yuv_width[1] = yuv_width[2] = yuv_width[0]/2;
		yuv_height[0] = (height + 31) & ~31;
		yuv_height[1] = yuv_height[2] = yuv_height[0]/2;
		break;
	case VO_COLOR_YUV_PLANAR411V:
		yuv_width[0] = (width + 7) & ~7;
		yuv_width[1] = yuv_width[2] = yuv_width[0];
		yuv_height[0] = (height + 31) & ~31;
		yuv_height[1] = yuv_height[2] = yuv_height[0]/4;
		break;
	case VO_COLOR_YUV_PLANAR411:
		yuv_width[0] = (width + 31) & ~31;
		yuv_width[1] = yuv_width[2] = yuv_width[0]/4;
		yuv_height[0] = (height + 7) & ~7;
		yuv_height[1] = yuv_height[2] = yuv_height[0];
		break;
	}
	
	for(i = 0; i < 3; i++)
	{
		pOutData->Stride[i] = yuv_width[i];
		yuv_size[i] = yuv_width[i] * yuv_height[i];
		
		pOutData->Buffer[i] = (char *)malloc(yuv_size[i]);
		memset(pOutData->Buffer[i], 0, yuv_size[i]);
		if(pOutData->Buffer[i] == NULL)
			return -1;
	}
	return 0;
}
int writeYUVdata(
				 VO_VIDEO_BUFFER*	poutdata,
				 VO_VIDEO_FORMAT*	poutformat,
				 FILE*					out_file)
{
	int i, j;
	unsigned char *out_src, *out_bk, out_src128;
	unsigned char *out_src1, *out_src2;

	out_src128 = 128;	
	if(poutdata->ColorType == VO_COLOR_GRAY_PLANARGRAY)
	{
		out_src = poutdata->Buffer[0];
		for(i = 0; i < poutformat->Height; i++)
		{
			fwrite(out_src, poutdata->Stride[0], 1, out_file);
			out_src += poutdata->Stride[0];
		}

		for(i = 0; i < poutformat->Height/2; i++)
		{
			for(j= 0; j < poutdata->Stride[0]/2; j++)
				fwrite(&out_src128, 1, 1, out_file);
		}
		
		for(i = 0; i < poutformat->Height/2; i++)
		{
			for(j= 0; j < poutdata->Stride[0]/2; j++)
				fwrite(&out_src128, 1, 1, out_file);
	}
		
	}
	else if(poutdata->ColorType == VO_COLOR_YUV_PLANAR444)
	{
		out_src = poutdata->Buffer[0];
		for(i = 0; i < poutformat->Height; i++)
		{
			fwrite(out_src, poutdata->Stride[0], 1, out_file);
			out_src += poutdata->Stride[0];
		}
		out_src = poutdata->Buffer[1];
		for(i = 0; i < poutformat->Height/2; i++)
		{
			out_bk = out_src;
			for(j= 0; j < poutdata->Stride[1]/2; j++)
			{
				fwrite(out_bk, 1, 1, out_file);
				out_bk++;
			}
			out_src += poutdata->Stride[1];
		}		
		out_src = poutdata->Buffer[2];
		for(i = 0; i < poutformat->Height/2; i++)
		{
			out_bk = out_src;
			for(j= 0; j < poutdata->Stride[2]/2; j++)
			{
				fwrite(out_bk, 1, 1, out_file);
				out_bk++;
			}
			out_src += poutdata->Stride[2];
		}
	}
	else if(poutdata->ColorType <= VO_COLOR_YUV_PLANAR422_12)
	{
		out_src = poutdata->Buffer[0];
		for(i = 0; i < poutformat->Height; i++)
		{
			fwrite(out_src, poutdata->Stride[0], 1, out_file);
			out_src += poutdata->Stride[0];
		}
		out_src = poutdata->Buffer[1];
		for(i = 0; i < poutformat->Height/2; i++)
		{
			fwrite(out_src, poutdata->Stride[1], 1, out_file);
			out_src += poutdata->Stride[1];
		}
		
		out_src = poutdata->Buffer[2];
		for(i = 0; i < poutformat->Height/2; i++)
		{
			fwrite(out_src, poutdata->Stride[2], 1, out_file);
			out_src += poutdata->Stride[2];
		}		
	}
	else if(poutdata->ColorType == VO_COLOR_RGB888_PLANAR)
	{
		out_src = poutdata->Buffer[0]+(poutformat->Height - 1)*poutdata->Stride[0];
		out_src1 = poutdata->Buffer[1]+(poutformat->Height - 1)*poutdata->Stride[0];
		out_src2 = poutdata->Buffer[2]+(poutformat->Height - 1)*poutdata->Stride[0];
		
		for(i = 0; i < poutformat->Height; i++)
		{
			for(j = 0; j < poutdata->Stride[0]; j++)
			{
				fwrite(&out_src2[j], 1, 1, out_file);
				fwrite(&out_src1[j], 1, 1, out_file);
				fwrite(&out_src[j], 1, 1, out_file);				
			}

			out_src -= poutdata->Stride[0];
			out_src1 -= poutdata->Stride[1];
			out_src2 -= poutdata->Stride[2];
		}
	}
	else if(poutdata->ColorType == VO_COLOR_YUV_PLANAR420)
	{
		out_src = poutdata->Buffer[0];
		for(i = 0; i < poutformat->Height; i++)
		{
			fwrite(out_src, poutdata->Stride[0], 1, out_file);
			out_src += poutdata->Stride[0];
		}
		
		out_src = poutdata->Buffer[1];
		for(i = 0; i < poutformat->Height/2; i++)
		{
//			for(j= 0; j < poutdata->Stride[1]; j++)
//				fwrite(&out_src128, 1, 1, out_file);
			fwrite(out_src, poutdata->Stride[1], 1, out_file);
			out_src += poutdata->Stride[1];
		}
		
		out_src = poutdata->Buffer[2];
		for(i = 0; i < poutformat->Height/2; i++)
		{
//			for(j= 0; j < poutdata->Stride[2]; j++)
//				fwrite(&out_src128, 1, 1, out_file);
			fwrite(out_src, poutdata->Stride[2], 1, out_file);
			out_src += poutdata->Stride[2];
		}
	}
	else if(poutdata->ColorType == VO_COLOR_YUV_PLANAR411V)
	{
		out_src = poutdata->Buffer[0];
		for(i = 0; i < poutformat->Height; i++)
		{
			fwrite(out_src, poutdata->Stride[0], 1, out_file);
			out_src += poutdata->Stride[0];
		}
		out_src = poutdata->Buffer[1];
		for(i = 0; i < poutformat->Height/4; i++)
		{
			fwrite(out_src, poutdata->Stride[1], 1, out_file);
			out_src += poutdata->Stride[1];
		}
		out_src = poutdata->Buffer[2];
		for(i = 0; i < poutformat->Height/4; i++)
		{
			fwrite(out_src, poutdata->Stride[2], 1, out_file);
			out_src += poutdata->Stride[2];
		}
	}
	else if (poutdata->ColorType == VO_COLOR_YUV_PLANAR411)
	{
		out_src = poutdata->Buffer[0];
		for(i = 0; i < poutformat->Height; i++)
		{
			fwrite(out_src, poutdata->Stride[0], 1, out_file);
			out_src += poutdata->Stride[0];
		}
		out_src = poutdata->Buffer[1];
		for(i = 0; i < poutformat->Height; i++)
		{
			fwrite(out_src, poutdata->Stride[1], 1, out_file);
			out_src += poutdata->Stride[1];
		}
		out_src = poutdata->Buffer[2];
		for(i = 0; i < poutformat->Height; i++)
		{
			fwrite(out_src, poutdata->Stride[2], 1, out_file);
			out_src += poutdata->Stride[2];
		}
	}
	else if (poutdata->ColorType == VO_COLOR_YUV_PLANAR422_21)
	{
		out_src = poutdata->Buffer[0];
		for(i = 0; i < poutformat->Height; i++)
		{
			fwrite(out_src, poutdata->Stride[0], 1, out_file);
			out_src += poutdata->Stride[0];
		}

		out_src = poutdata->Buffer[1];
		for(i = 0; i < poutformat->Height/2; i++)
		{
			fwrite(out_src, poutdata->Stride[1]/2, 1, out_file);
			out_src += poutdata->Stride[1];
		}

		out_src = poutdata->Buffer[2];
		for(i = 0; i < poutformat->Height/2; i++)
		{
			fwrite(out_src, poutdata->Stride[2]/2, 1, out_file);
			out_src += poutdata->Stride[2];
		}
	}

	return 0;
}


#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
//int WinMain()
#else // _WIN32_WCE
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	FILE *in_file = NULL, *out_file = NULL, *out_file1 = NULL;
	FILE *res_file = NULL;
	unsigned char *in_buffer0 = NULL, *in_buffer1 = NULL;
	int file_size, read_size, frameNum=0;
	int zoomout, i, width, height;
	int buffer_limite = 30000000;
	VO_HANDLE phCodec;
	JPEG_IMAGE_FIELD	outField;
	VO_CODECBUFFER	indata;
	VO_VIDEO_BUFFER	outdata;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_OUTPUTINFO outInfo1;
	VO_VIDEO_BUFFER	outdata1;
	VO_VIDEO_FORMAT	outformat;
    int modenormal = 1;

	VO_IMAGE_DECAPI voJPEGDec;

	unsigned char* rgb565 = NULL;

	//image_yuv444[i*image_width*3+j]= 263*r + 516*g + 100*b + 16;
			//image_yuv444[i*image_width*3+j+1] = -152*r - 299*g + 449*b + 128;
			//image_yuv444[i*image_width*3+j+2] = 449*r - 377*g - 73*b + 128;

#ifdef _WIN32_WCE
	TCHAR* path = _T("/ResidentFlash/HammerB2716/(jpeg1)1024x768_24.jpg"); 
#else
	#ifdef LINUX
	char* path = "/data/local/tmp/YUV_444_1600x1200.jpeg";
	#else
	TCHAR *path = _T("E:/MyResource/Video/clips/mjpeg/QuickTest/00000100_YUV_444_YUV_1600x1200.jpeg");
	#endif
#endif
	long total = 0;
	long total1 = 0;
	int ret;
	long begin;
	long end;

#ifdef _WIN32_WCE	 
	if(!(out_file = _wfopen(_T("\\Program Files\\vojpegDecTst\\testjpeg3.0.yuv"), _T("w+b")))){
		printf("Open output file error!");
	}

#else
#ifdef LINUX
	if(!(out_file = fopen("/data/local/tmp/YUV_444_1600x1200.yuv", "wb"))){
		printf("Open output file error!");
	}
#else                  
	if(!(out_file = fopen("E:/MyResource/Video/clips/mjpeg/QuickTest/00000100_YUV_444_YUV_1600x1200.yuv", "wb"))){
		printf("Open output file error!");
	}
#endif
#endif
	memset(&outdata, 0 ,sizeof(VO_VIDEO_BUFFER));
	memset(&outdata1, 0 ,sizeof(VO_VIDEO_BUFFER));
	outField.start_X = 0; 
	outField.start_Y = 0;	

	voGetJPEGDecAPI(&voJPEGDec,NULL);
	ret = voJPEGDec.Init(&phCodec,VO_IMAGE_CodingJPEG,NULL);
	if(ret == VO_ERR_LICENSE_ERROR)
	{
		goto free_memory1;
	}

#ifdef _WIN32_WCE
	begin = GetTickCount();
#else
	begin = clock();
#endif

	printf(" aaa \n");
	voJPEGDec.SetParam(phCodec, VO_JPEG_FILE_PATH, (VO_PTR)path);	
	//voJPEGDecSetParameter(phCodec, JPEG_DECSTEP_ENABLE, 1);
    modenormal = 0;  // 0 : out put 420 ;  1: out = in
    voJPEGDec.SetParam(phCodec,VO_PID_DEC_JPEG_SET_NORMAL_CHROMA, &modenormal);

	zoomout = *argv[1] -0x30;
	printf("zoomout = %d \n",zoomout);
	voJPEGDec.SetParam(phCodec, VO_JPEG_OUTPUT_ZOOMOUT, &zoomout);
	voJPEGDec.SetParam(phCodec, VO_JPEG_BUFFER_LIMITED, &buffer_limite);
	
	voJPEGDec.GetParam(phCodec, VO_PID_JPEG_WIDTH, &width);
	voJPEGDec.GetParam(phCodec, VO_PID_JPEG_HEIGHT, &height);
	voJPEGDec.GetParam(phCodec, VO_JPEG_VIDEO_TYPE, &outdata.ColorType);
	outInfo.Format.Width = width;
	outInfo.Format.Height = height;
	outInfo.Format.Type   = VO_VIDEO_FRAME_I;
	

    // deocde a part of the image
	//outField.end_X = 400 + outField.start_X;
	//outField.end_Y = 300 + outField.start_Y;
	// deocde whole of the image	
	outField.end_X = width + outField.start_X;
	outField.end_Y = height + outField.start_Y;

	outInfo.Format.Width = outField.end_X - outField.start_X;
	outInfo.Format.Height = outField.end_Y - outField.start_Y;
	voJPEGDec.SetParam(phCodec, VO_JPEG_OUTPUT_FIELD, (VO_PTR)(&outField));
	
	outInfo.Format.Width = ((outInfo.Format.Width + zoomout - 1) / zoomout + 1) & ~1;
	outInfo.Format.Height = ((outInfo.Format.Height + zoomout - 1) / zoomout + 1) & ~1;
	
	if(UpdataBuffer(&outdata, outInfo.Format.Width, outInfo.Format.Height) < 0)
		goto free_memory;

	printf("bbb \n");

	do {
		ret = voJPEGDec.GetParam(phCodec, VO_JPEG_OUTPUT_BUFFER, &outdata); 
	} while(ret == VO_ERR_JPEGDEC_DECODE_UNFINISHED);
	printf("ccc \n");
#ifdef _WIN32_WCE
	end = GetTickCount();
	total += end - begin;
	{
		FILE *fp =fopen("/ResidentFlash/HammerB2716/vojpeg/performance.txt","a");
		fprintf(fp,"%d ms \n",total);
		fclose(fp);
	}
#else
#ifdef LINUX
	end = clock();
	total += end - begin;
	{
		FILE *fp =fopen("/data/local/tmp/performance.txt","a");
		fprintf(fp,"%d ms \n",total);
		fclose(fp);
	}
#else
	end = clock();
	total += end - begin;
	printf("%d ms",total);
#endif
#endif	

	width = outInfo.Format.Width;
	height = outInfo.Format.Height;

	printf("%d %d\n",width,height);
#if 0
	if(ret == VO_ERR_NONE)
		{
			int i,j;
			FILE *fp =fopen("E:\\zzz.yuv","ab");
			fwrite(outdata.Buffer[0],width*height,1,fp);
			fwrite(outdata.Buffer[1],width*height/4,1,fp);
			fwrite(outdata.Buffer[2],width*height/4,1,fp);
			fclose(fp);
			//cc_yuv420torgb24_c_16bit cc_rgb888p_torgb24_c_16bit
				cc_yuv420torgb24_c_16bit(outdata.Buffer[0], outdata.Buffer[1],outdata.Buffer[2], outdata.Stride[0], 
							image,width*3, width,height,outdata.Stride[1], outdata.Stride[2]);
#ifdef _WIN32_WCE	
			writebmpfile("/ResidentFlash/HammerB2716/vojpeg/result.bmp",image,width,height);
#else
	#ifdef LINUX
			writebmpfile("/data/local/result.bmp",image,width,height);
   #else
			writebmpfile("E:/MyResource/Video/clips/mjpeg/QuickTest/00000100_YUV_444_YUV_1600x1200.bmp",image,width,height,width*3,3);
   #endif
#endif


            {
                unsigned char* image_565 = (unsigned char*)malloc(sizeof(unsigned char)*width*height*2);
                cc_rgb24torgb16_c_16bit(image, image_565,width,height);
                writebmpfile("E:/MyResource/Image/clips/JPEG/LENA_422_12_rgb16.bmp",image_565,width,height,width*2,2);
                

                {
                    FILE *fp =fopen("E:/MyResource/Image/clips/JPEG/LENA_422_12.rgb16","wb");
                    fwrite(image_565,1,width*height*2,fp);
                    fclose(fp);
                }

                {
                    cc_rgb16torgb24_c_16bit(image_565,image, width,height);
                    writebmpfile("E:/MyResource/Image/clips/JPEG/LENA_422_12_rgb24_2.bmp",image,width,height,width*3,3);
                }

                free(image_565);
            }
		}
#endif
	
	if(ret == VO_ERR_NONE)
	{
		writeYUVdata(&outdata, &(outInfo.Format), out_file);
		{
			VO_U8* image = (unsigned char*)malloc(sizeof(unsigned char)*width*height*3);
			cc_yuv420torgb24_c_16bit(outdata.Buffer[0], outdata.Buffer[1],outdata.Buffer[2], outdata.Stride[0], 
							image,width*3, width,height,outdata.Stride[1], outdata.Stride[2]);
			writebmpfile("/data/local/tmp/YUV_444_1600x1200.bmp",image,width,height,width*3,3);
			free(image);
		}
	}
	
free_memory:
	voJPEGDec.Uninit(phCodec);

free_memory1:
	if (out_file)
		fclose(out_file);
	
	for(i = 0; i < 3; i++)
	{
		if(outdata.Buffer[i])
			free(outdata.Buffer[i]);
		if(outdata1.Buffer[i])
			free(outdata1.Buffer[i]);
	}
	
	if(in_buffer1)
		free(in_buffer1);
    return 0;	
}
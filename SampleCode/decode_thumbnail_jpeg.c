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

void writebmpfile(char * filename,unsigned char* image,long width,long height)
{
	BITMAPINFO *info;
	BITMAPFILEHEADER bmphead;			
	FILE * fp = fopen(filename,"wb");

	bmphead.bfType = 0x4d42;
	bmphead.bfSize = width*height*3+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO);//+1078;
	bmphead.bfReserved1 = 0;
	bmphead.bfReserved2 = 0;
	bmphead.bfOffBits = 54;//1078;

	info=(BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER));//sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD));//1064
	info->bmiHeader.biSize=sizeof(BITMAPINFOHEADER); //40
	info->bmiHeader.biBitCount = 24;
	info->bmiHeader.biClrImportant =0;
	info->bmiHeader.biClrUsed = 0;
	info->bmiHeader.biWidth = width;
	info->bmiHeader.biHeight = -height;
	info->bmiHeader.biCompression = BI_RGB;
	info->bmiHeader.biPlanes = 1;
	info->bmiHeader.biSizeImage = width*height*3;//imgData.pBmpInfoHead->biSizeImage;
	info->bmiHeader.biXPelsPerMeter =0;
	info->bmiHeader.biYPelsPerMeter =0;	

	fwrite((LPSTR)&bmphead, sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(info,sizeof(BITMAPINFOHEADER),1, fp);

	fwrite(image,width*height*3,1,fp);
	fclose(fp);
	free(info);
	return;
}
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


int  UpdataBuffer(VO_VIDEO_BUFFER *pOutData, int width, int height)
{
	int i;
	int yuv_width[3];  
	int	yuv_height[3];
	int	yuv_size[3];

	//printf("yuv---%d \n",pOutData->ColorType);
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

	//printf("%d %d\n",yuv_width[0],yuv_height[0]);

	return 0;
}

/*int writeYUVdata(
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
		//if(enableNormalChroma==0)
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
		
//		out_src = poutdata->Buffer[1];
//		for(i = 0; i < poutformat->Height/2; i++)
//		{
//			out_bk = out_src;
//			for(j= 0; j < poutdata->Stride[1]/2; j++)
//			{
//				fwrite(out_bk, 1, 1, out_file);
//				out_bk += 2;
//			}
////			fwrite(out_src, poutdata->stride[1], 1, out_file);
//			out_src += poutdata->Stride[1]*2;
//		}
//		
//		out_src = poutdata->Buffer[2];
//		for(i = 0; i < poutformat->Height/2; i++)
//		{
//			out_bk = out_src;
//			for(j= 0; j < poutdata->Stride[2]/2; j++)
//			{
//				fwrite(out_bk, 1, 1, out_file);
//				out_bk += 2;
//			}
////			fwrite(out_src, poutdata->stride[2], 1, out_file);
//			out_src += poutdata->Stride[2]*2;
//		}
	}
	else if(poutdata->ColorType <= VO_COLOR_YUV_PLANAR422_12)
	{
		out_src = poutdata->Buffer[0];
		for(i = 0; i < poutformat->Height; i++)
		{
			fwrite(out_src, poutdata->Stride[0], 1, out_file);
			out_src += poutdata->Stride[0];
		}
		//if(enableNormalChroma==0)
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
		
		//else
//		out_src = poutdata->Buffer[1];
//		for(i = 0; i < poutformat->Height/2; i++)
//		{
////			for(j= 0; j < poutdata->stride[1]; j+=2)
//// 				fwrite(&out_src[j], 1, 1, out_file);
//			fwrite(out_src, poutdata->Stride[1], 1, out_file);
//			out_src += poutdata->Stride[1]*2;
//		}
//		
//		out_src = poutdata->Buffer[2];
//		for(i = 0; i < poutformat->Height/2; i++)
//		{
////			for(j= 0; j < poutdata->stride[2]; j+=2)
////				fwrite(&out_src[j], 1, 1, out_file);
//			fwrite(out_src, poutdata->Stride[2], 1, out_file);
//			out_src += poutdata->Stride[2]*2;
//		}
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
	else 
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
//			for(j= 0; j < poutdata->stride[1]; j++)
//				fwrite(&out_src128, 1, 1, out_file);
			fwrite(out_src, poutdata->Stride[1], 1, out_file);
			out_src += poutdata->Stride[1];
		}
		
		out_src = poutdata->Buffer[2];
		for(i = 0; i < poutformat->Height/2; i++)
		{
//			for(j= 0; j < poutdata->stride[2]; j++)
//				fwrite(&out_src128, 1, 1, out_file);
			fwrite(out_src, poutdata->Stride[2], 1, out_file);
			out_src += poutdata->Stride[2];
		}
	}

	return 0;
}*/

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
		/*out_src = poutdata->Buffer[1];
		for(i = 0; i < poutformat->Height/2; i++)
		{
			out_bk = out_src;
			for(j= 0; j < poutdata->Stride[1]/2; j++)
			{
				fwrite(out_bk, 1, 1, out_file);
				out_bk+=2;
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
				out_bk+=2;
			}
			out_src += poutdata->Stride[2];
		}*/
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
		/*out_src = poutdata->Buffer[1];
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
		}*/		
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

	VO_IMAGE_DECAPI voJPEGDec;

	unsigned char* rgb565 = NULL;

#ifdef _WIN32_WCE
	TCHAR* path = _T("\\Temp\\jpeg\\test-yuv422.jpg"); 
#else
	char *path = "D:\\En_smallpic_2n.jpg";
#endif
	long total = 0;
	long total1 = 0;
	int ret;
	int times=1;
	long begin;
	long end;

#ifdef _WIN32_WCE	
	if(!(out_file = _wfopen(_T("\\Program Files\\vojpegDecTst\\testjpeg3.0.yuv"), _T("w+b")))){
		printf("Open output file error!");
	}
#else
#ifdef LINUX
#else
	if(!(out_file = fopen("D:\\Software\\yuvtools_v2\\testjpeg2.0.yuv", "w+b"))){
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
		goto free_memory1;

#ifdef _WIN32_WCE
	begin = GetTickCount();
#else
	begin = clock();
#endif
	{ // thumbnail
		int enable_thumb =1;
		int mode =1;
#ifdef _WIN32_WCE
		indata.Buffer = _T("\\Temp\\10.jpg");
#else
	#ifdef LINUX
		indata.Buffer = "/data/local/test-yuv422.jpg";
	#else
		indata.Buffer = _T("E:\\JPEGTESTFILE\\WVGA\\10\\smallpic_3.jpg");
   #endif
#endif

		indata.Length = 0;
		voJPEGDec.SetParam(phCodec, VO_PID_EXIFINFO_SUPPT, (VO_PTR)(&enable_thumb));
	    voJPEGDec.SetParam(phCodec, VO_JPEG_INPUT_MODE, &mode); // 1 file mode; 0 buffer mode

		voJPEGDec.GetParam(phCodec, VO_PID_JPEG_WIDTH, &width);
		do 
		{
			ret = voJPEGDec.Process(phCodec, &indata, &outdata, &outformat);
		} while(ret == VO_ERR_JPEGDEC_DECODE_UNFINISHED);

		{
			
		}

//		if(ret == VO_ERR_NONE)
//		{
//			unsigned char* image = (unsigned char*)malloc(sizeof(unsigned char)*outformat.Height*outformat.Width*3);
//
//			//cc_yuv420torgb24_c_16bit cc_rgb888p_torgb24_c_16bit
//				cc_rgb888p_torgb24_c_16bit(outdata.Buffer[0], outdata.Buffer[1],outdata.Buffer[2], outdata.Stride[0], 
//							image,outformat.Width*3, outformat.Width,outformat.Height,outdata.Stride[1], outdata.Stride[2]);
//#ifdef _WIN32_WCE	
//			writebmpfile("\Program Files\\vojpegDecTst\\result.bmp",image,outformat.Width,outformat.Height);
//#else
//	#ifdef LINUX
//			writebmpfile("/data/local/result.bmp",image,outformat.Width,outformat.Height);
//   #else
//			writebmpfile("E:\\JPEGTESTFILE\\result.bmp",image,outformat.Width,outformat.Height);
//#endif
//#endif
//			free(image);
//		}

		{ // here decode extra information
			long mode=0;
			char OutString[40];
			int	Degree[6];
			long total = 0;
			voJPEGDec.GetParam(phCodec, VO_PID_PICXRESOLUTION, &mode);

			ret = voJPEGDec.GetParam(phCodec, VO_PID_MACHINE_MAKE, (long *)OutString);
			if(ret == VO_ERR_NONE) printf("Machine Make: %s\n", OutString);

			ret = voJPEGDec.GetParam(phCodec, VO_PID_MACHINE_MODEL, (long *)OutString);
			if(ret == VO_ERR_NONE) printf("Machine Model: %s\n", OutString);

			ret = voJPEGDec.GetParam(phCodec, VO_PID_PICTURE_DATE, (long *)OutString);
			if(ret == VO_ERR_NONE) printf("Picture Date&Time: %s\n", OutString);

			ret = voJPEGDec.GetParam(phCodec, VO_PID_PICXRESOLUTION, Degree);
			if(ret == VO_ERR_NONE) printf("Picture X RESOLUTION: %d\n", Degree[0]/Degree[1]);

			ret = voJPEGDec.GetParam(phCodec, VO_PID_PICYRESOLUTION, Degree);
			if(ret == VO_ERR_NONE) printf("Picture Y RESOLUTION: %d\n", Degree[0]/Degree[1]);

			ret = voJPEGDec.GetParam(phCodec, VO_PID_GPS_MAPDATUM, (long *)OutString);
			if(ret == VO_ERR_NONE) printf("Picture Mapdatum: %s\n", OutString);

			ret = voJPEGDec.GetParam(phCodec, VO_PID_GPS_LATIREF, &total);
			if(ret == VO_ERR_NONE) printf("Picture Latitude: %c:", total);

			ret = voJPEGDec.GetParam(phCodec, VO_PID_GPS_LATITUDE, Degree);
			if(ret == VO_ERR_NONE) printf("%d %d'%0.2f\"\n", Degree[0]/Degree[1],
				Degree[2]/Degree[3], Degree[4]*1.0/Degree[5]);

			ret = voJPEGDec.GetParam(phCodec, VO_PID_GPS_LONGIREF, &total);
			if(ret == VO_ERR_NONE) printf("Picture Longitude: %c:", total);

			ret = voJPEGDec.GetParam(phCodec, VO_PID_GPS_LONGITUDE, Degree);
			if(ret == VO_ERR_NONE) printf("%d %d'%d\"\n", Degree[0]/Degree[1],
				Degree[2]/Degree[3], Degree[4]/Degree[5]);
			
			ret = voJPEGDec.GetParam(phCodec, VO_PID_GPS_ALITUDE, &total);
			if(ret == VO_ERR_NONE) printf("Picture Alitude: %d m\n", total);
		}

	}	
#ifdef _WIN32_WCE
	end = GetTickCount();
	total += end - begin;
	{
		FILE *fp =fopen("\\Program Files\\vojpegDecTst\\performance.txt","a");
		fprintf(fp,"total = %d aver=%f ms \n",total,(total*1.0)/1);
		fclose(fp);
	}
#else
	end = clock();
	total += end - begin;
	printf("%d ms\n",total);
#endif	
	
free_memory:
	voJPEGDec.Uninit(phCodec);

free_memory1:
	if (out_file)
		fclose(out_file);	
	if(in_buffer1)
		free(in_buffer1);
    return 0;	
}
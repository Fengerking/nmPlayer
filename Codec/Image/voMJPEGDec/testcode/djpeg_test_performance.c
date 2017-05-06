#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE

//#include <stdafx.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "voJPEG.h"


#define ConstV1 (0x198937>>8) //1673527
#define ConstV2 (0xD020C>>8)  //852492
#define ConstU1 (0x2045A1>>8) //2114977
#define ConstU2 (0x645A1>>8)  //411041
#define ConstY	(0x129FBE>>8) //1220542

#define SAT(Value) (Value) < 0 ? 0: ((Value) > 255 ? 255: (Value));

void cc_yuv420torgb565_c_16bit(unsigned char *psrc_y, unsigned char *psrc_u, unsigned char *psrc_v, const signed long in_stridey, const signed long in_strideu, const signed long in_stridev, 
				        unsigned char *out_buf, const signed long out_stride, signed long width, signed long height)
{
	signed long a0, a1, a2, a3, a4, a5, a7, a33, a77;
	signed long i, j;
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
			*(out_buf+1) =( (a4 & 0xF8)  | ( a5 >> 5) );
			*(out_buf) =( ((a5 & 0x1C) << 3) | ( a3 >> 3) );
			//*(out_buf) = a3;
			//*(out_buf+1) = a5;
			//*(out_buf+2) = a4;

			a4  = SAT((a33 + a1)>>12);
			a5  = SAT((a33 - a0)>>12);
			a33  = SAT((a33 + a2)>>12);
			*(out_buf+3) =( (a4 & 0xF8)  | ( a5 >> 5) );
			*(out_buf+2) =( ((a5 & 0x1C) << 3) | ( a33 >> 3) );
			//*(out_buf+3) = a33;
			//*(out_buf+4) = a5;
			//*(out_buf+5) = a4;

			a4  = SAT((a7 + a1)>>12);
			a5  = SAT((a7 - a0)>>12);
			a7  = SAT((a7 + a2)>>12);
			*(out_buf+1+out_stride) =( (a4 & 0xF8)  | ( a5 >> 5) );
			*(out_buf+0+out_stride) =( ((a5 & 0x1C) << 3) | ( a7 >> 3) );
			//*(out_buf+0+out_stride) = a7;
			//*(out_buf+1+out_stride) = a5;
			//*(out_buf+2+out_stride) = a4;

			a4  = SAT((a77 + a1)>>12);
			a5  = SAT((a77 - a0)>>12);
			a77  = SAT((a77 + a2)>>12);
			*(out_buf+3+out_stride) =( (a4 & 0xF8)  | ( a5 >> 5) );
			*(out_buf+2+out_stride) =( ((a5 & 0x1C) << 3) | ( a77 >> 3) );
			//*(out_buf+3+out_stride) = a77;
			//*(out_buf+4+out_stride) = a5;
			//*(out_buf+5+out_stride) = a4;

			out_buf += 4;
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

		out_buf -= (width*2);
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

	VO_IMAGE_DECAPI voJPEGDec;

	unsigned char* rgb565 = NULL;

#ifdef _WIN32_WCE
	TCHAR* path = _T("\\Temp\\jpeg\\test-yuv422.jpg"); 
#else
	TCHAR *path = _T("E:\\test_th.jpg");
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
	if(!(out_file = fopen("D:\\Software\\yuvtools_v2\\testjpeg2.0.yuv", "w+b"))){
		printf("Open output file error!");
	}
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
	//{ // thumbnail
	//	int enable_thumb =1;
	//	int mode =1;
	//	voJPEGDec.SetParam(phCodec, VO_PID_EXIFINFO_SUPPT, (VO_PTR)(&enable_thumb));
	//    voJPEGDec.SetParam(phCodec, VO_JPEG_INPUT_MODE, &mode); // 1 file mode; 0 buffer mode

	//	voJPEGDec.GetParam(phCodec, VO_PID_JPEG_WIDTH, &width);
	//	do 
	//	{
	//		indata.Buffer = _T("Temp//jpeg//e.jpg");
	//		indata.Length = 0;
	//		ret = voJPEGDec.Process(phCodec, &indata, &outdata, &outformat);
	//	} while(ret == VO_ERR_JPEGDEC_DECODE_UNFINISHED);
	//}
while(times--)
{
	voJPEGDec.SetParam(phCodec, VO_JPEG_FILE_PATH, (VO_PTR)path);
	zoomout = 1;
	voJPEGDec.SetParam(phCodec, VO_JPEG_OUTPUT_ZOOMOUT, &zoomout);
	voJPEGDec.SetParam(phCodec, VO_JPEG_BUFFER_LIMITED, &buffer_limite);
	voJPEGDec.GetParam(phCodec, VO_PID_JPEG_WIDTH, &width);
	voJPEGDec.GetParam(phCodec, VO_PID_JPEG_HEIGHT, &height);
	voJPEGDec.GetParam(phCodec, VO_JPEG_VIDEO_TYPE, &outdata.ColorType);
	outInfo.Format.Width = width;
	outInfo.Format.Height = height;
	outInfo.Format.Type   = VO_VIDEO_FRAME_I;	
	outField.end_X = width + outField.start_X;
	outField.end_Y = height + outField.start_Y;	
	outInfo.Format.Width = outField.end_X - outField.start_X;
	outInfo.Format.Height = outField.end_Y - outField.start_Y;
	voJPEGDec.SetParam(phCodec, VO_JPEG_OUTPUT_FIELD, (VO_PTR)(&outField));	
	outInfo.Format.Width = ((outInfo.Format.Width + zoomout - 1) / zoomout + 1) & ~1;
	outInfo.Format.Height = ((outInfo.Format.Height + zoomout - 1) / zoomout + 1) & ~1;	
	if(UpdataBuffer(&outdata, outInfo.Format.Width, outInfo.Format.Height) < 0)
		goto free_memory;

	do {
		ret = voJPEGDec.GetParam(phCodec, VO_JPEG_OUTPUT_BUFFER, &outdata); 
	} while(ret == VO_ERR_JPEGDEC_DECODE_UNFINISHED);

	{
		//FILE *fp =fopen("\\Program Files\\vojpegDecTst\\performance.txt","a");
		//fprintf(fp,"%d \n",times);
		//fclose(fp);
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
	printf("%d ms",total);
#endif	
	
	if(ret == VO_ERR_NONE)
		writeYUVdata(&outdata, &(outInfo.Format), out_file);
	
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
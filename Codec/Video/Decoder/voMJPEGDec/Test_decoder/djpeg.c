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
		yuv_width[0] = (width + 15) & ~15;
		yuv_width[1] = yuv_width[2] = yuv_width[0]/2;
		yuv_height[0] = (height + 15) & ~15;
		yuv_height[1] = yuv_height[2] = yuv_height[0]/2;
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
				out_bk += 2;
			}
//			fwrite(out_src, poutdata->stride[1], 1, out_file);
			out_src += poutdata->Stride[1]*2;
		}
		
		out_src = poutdata->Buffer[2];
		for(i = 0; i < poutformat->Height/2; i++)
		{
			out_bk = out_src;
			for(j= 0; j < poutdata->Stride[2]/2; j++)
			{
				fwrite(out_bk, 1, 1, out_file);
				out_bk += 2;
			}
//			fwrite(out_src, poutdata->stride[2], 1, out_file);
			out_src += poutdata->Stride[2]*2;
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
//			for(j= 0; j < poutdata->stride[1]; j+=2)
// 				fwrite(&out_src[j], 1, 1, out_file);
			fwrite(out_src, poutdata->Stride[1], 1, out_file);
			out_src += poutdata->Stride[1]*2;
		}
		
		out_src = poutdata->Buffer[2];
		for(i = 0; i < poutformat->Height/2; i++)
		{
//			for(j= 0; j < poutdata->stride[2]; j+=2)
//				fwrite(&out_src[j], 1, 1, out_file);
			fwrite(out_src, poutdata->Stride[2], 1, out_file);
			out_src += poutdata->Stride[2]*2;
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

	VO_IMAGE_DECAPI voJPEGDec;

#ifdef _WIN32_WCE
	TCHAR* path = _T("test1.jpg"); 
#else
	TCHAR *path = _T("E:/test.JPG");
#endif
	long total = 0;
	long total1 = 0;
	int ret;
	long begin;
	long end;

#ifdef _WIN32_WCE	
	if(!(out_file = _wfopen(_T("testjpeg.yuv"), _T("w+b")))){
		printf("Open output file error!");
	}

	if(!(out_file1 = _wfopen(_T("testjpeg1.yuv"), _T("w+b")))){
		printf("Open output file error!");
	}
#else
	if(!(out_file = fopen("E:/bigpic.yuv", "w+b"))){
		printf("Open output file error!");
	}
	if(!(out_file1 = fopen("E:/smallpic.yuv", "w+b"))){
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
	voJPEGDec.SetParam(phCodec, VO_JPEG_FILE_PATH, (VO_PTR)path);	
	//voJPEGDecSetParameter(phCodec, JPEG_DECSTEP_ENABLE, 1);
	zoomout = 1;
	voJPEGDec.SetParam(phCodec, VO_JPEG_OUTPUT_ZOOMOUT, &zoomout);
	voJPEGDec.SetParam(phCodec, VO_JPEG_BUFFER_LIMITED, &buffer_limite);

	voJPEGDec.GetParam(phCodec, VO_PID_JPEG_WIDTH, &width);
	voJPEGDec.GetParam(phCodec, VO_PID_JPEG_HEIGHT, &height);
	voJPEGDec.GetParam(phCodec, VO_JPEG_VIDEO_TYPE, &outdata.ColorType);
	outInfo.Format.Width = width;
	outInfo.Format.Height = height;
	outInfo.Format.Type   = VO_VIDEO_FRAME_I;


	//outField.end_X = 320*4 + outField.start_X;
	//outField.end_Y = 240*4 + outField.start_Y;	
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
	
#ifdef _WIN32_WCE
	end = GetTickCount();
	total += end - begin;
#else
	end = clock();
	total += end - begin;
#endif	
	
	if(ret == VO_ERR_NONE)
		writeYUVdata(&outdata, &(outInfo.Format), out_file);

	outField.end_X = 400*4 + outField.start_X;
	outField.end_Y = 240*4 + outField.start_Y;	
	outdata1.ColorType = outdata.ColorType;

#ifdef _WIN32_WCE
	begin = GetTickCount();
#else
	begin = clock();
#endif
	voJPEGDec.SetParam(phCodec, VO_JPEG_OUTPUT_ZOOMOUT, &zoomout);
	voJPEGDec.SetParam(phCodec, VO_JPEG_OUTPUT_FIELD, (VO_PTR)(&outField));	

	outInfo1.Format.Width = outField.end_X - outField.start_X;
	outInfo1.Format.Height = outField.end_Y - outField.start_Y;

	outInfo1.Format.Width = ((outInfo1.Format.Width + zoomout - 1) / zoomout + 1) & ~1;
	outInfo1.Format.Height = ((outInfo1.Format.Height + zoomout - 1) / zoomout + 1) & ~1;

	if(UpdataBuffer(&outdata1, outInfo1.Format.Width, outInfo1.Format.Height) < 0)
		goto free_memory;

	do {
		ret = voJPEGDec.GetParam(phCodec, VO_JPEG_OUTPUT_BUFFER, &outdata1);
	} while(ret == VO_ERR_JPEGDEC_DECODE_UNFINISHED);

#ifdef _WIN32_WCE
	end = GetTickCount();
	total1 += end - begin;
#else
	end = clock();
	total1 += end - begin;
#endif	

	if(ret == VO_ERR_NONE)
		//writeYUVdata(&outdata1, &(outInfo1.Format), out_file1);

#ifdef _WIN32_WCE	
	res_file = fopen("JPEGresult.txt", "a+");
	fprintf(res_file, "\n\t totTime = %d\n\n", total);
	fprintf(res_file, "\n\t totTime = %d\n\n", total1);
	fclose (res_file);
#else
	printf("totTime = %d\n", total);
#endif
	
free_memory:
	voJPEGDec.Uninit(phCodec);

free_memory1:
	if (out_file)
		fclose(out_file);
	if (out_file1)
		fclose(out_file1);
	
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


#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "voMJPEG.h"

#define INPUT_BUF_SIZE 1024*1024
#define READ_SIZE INPUT_BUF_SIZE/2
#ifdef _WIN32_WCE
//int WinMain()
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	FILE *in_file = NULL, *out_file = NULL;
	FILE *res_file = NULL;
	unsigned char *in_buffer0 = NULL;
	int file_size, read_size, frameNum=0;
	VO_HANDLE phCodec;
	VO_CODECBUFFER	indata;
	VO_VIDEO_BUFFER	outdata;
	VO_VIDEO_FORMAT	outformat;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI	voMJPEGDec;
	long total = 0;
	long begin = 0;
	long end = 0;

#ifdef _WIN32_WCE	
	if(!(in_file = fopen ("/Storage Card/test1.mjp", "rb"))){
		printf ("Could not open file!");
		return 0;
	}
	
	
	if(!(out_file = fopen("/Storage Card/test1.yuv", "w+b"))){
		printf("Open output file error!");
		return 0;
	}
#else
    if (argc > 1) {
		if(!(in_file = fopen (argv[1], "rb"))){
			printf ("Could not open file \"%s\".\n", argv[1]);
		}
    } else{
	
		printf ("Could not open file!");
	}

	if(!(out_file = fopen(argv[2], "w+b"))){
		printf("Open output file error!");
	}
#endif
	/* input buffer */
	in_buffer0 = (unsigned char *)malloc(INPUT_BUF_SIZE);
	if (!in_buffer0)
		goto free_memory;
	
    fseek(in_file, 0, SEEK_END);
    file_size = ftell(in_file);
    fseek(in_file, 0, SEEK_SET);

	/* fill input buffer*/
	if(!(read_size = fread(in_buffer0, 1, READ_SIZE, in_file)))
		goto free_memory;

	indata.Buffer = in_buffer0;
	indata.Length = read_size;

	voGetMJPEGDecAPI(&voMJPEGDec,NULL);

	voMJPEGDec.Init(&phCodec, VO_VIDEO_CodingMJPEG, NULL);
	//voMJPEGDec.SetParam(phCodec,VO_PID_DEC_MJPEG_SET_NORMAL_CHROMA,1);

	while(file_size>0)
	{
		int length;
		unsigned char *out_src;
#ifdef _WIN32_WCE
		begin = GetTickCount();
#endif	
		voMJPEGDec.SetInputData(phCodec, &indata);
		voMJPEGDec.GetOutputData(phCodec, &outdata, &outInfo);

		outformat = outInfo.Format;
#ifdef _WIN32_WCE
		end = GetTickCount();
#endif
		total += end - begin;
		length = outInfo.InputUsed;
		if(length > 0){
			int tmp_len, left_len = read_size - length;
			
			if(left_len > 0){
				/* fill input buffer */
				memmove(in_buffer0, in_buffer0 + length, left_len);
			}
			file_size -= length;	
			
			tmp_len = fread((in_buffer0 + left_len), 1, READ_SIZE - left_len, in_file);
			
			read_size = tmp_len + left_len;
			indata.Length = read_size;
			
			//			if(dec_frame_info.image_type != NULL_FRAME){
#if 1			
			//	if(dec_frame_info.image_type != NULL_FRAME)
			{
				int i;
				
				if(outdata.ColorType == VO_COLOR_GRAY_PLANARGRAY)
				{
					out_src = outdata.Buffer[0];
					for(i = 0; i < outformat.Height; i++)
					{
						fwrite(out_src, outdata.Stride[0], 1, out_file);
						out_src += outdata.Stride[0];
					}
				}
				else if(outdata.ColorType <= VO_COLOR_YUV_PLANAR422_12)
				{
					out_src = outdata.Buffer[0];
					for(i = 0; i < outformat.Height; i++)
					{
						fwrite(out_src, outdata.Stride[0], 1, out_file);
						out_src += outdata.Stride[0];
					}
					
					out_src = outdata.Buffer[1];
					for(i = 0; i < outformat.Height; i++)
					{
						fwrite(out_src, outdata.Stride[1], 1, out_file);
						out_src += outdata.Stride[1];
					}
					
					out_src = outdata.Buffer[2];
					for(i = 0; i < outformat.Height; i++)
					{
						fwrite(out_src, outdata.Stride[2], 1, out_file);
						out_src += outdata.Stride[2];
					}
				}
				else 
				{
					out_src = outdata.Buffer[0];
					for(i = 0; i < outformat.Height; i++)
					{
						fwrite(out_src, outdata.Stride[0], 1, out_file);
						out_src += outdata.Stride[0];
					}
					
					out_src = outdata.Buffer[1];
					for(i = 0; i < outformat.Height >> 1; i++)
					{
						fwrite(out_src, outdata.Stride[1], 1, out_file);
						out_src += outdata.Stride[1];
					}
					
					out_src = outdata.Buffer[2];
					for(i = 0; i < outformat.Height >> 1; i++)
					{
						fwrite(out_src, outdata.Stride[2], 1, out_file);
						out_src += outdata.Stride[2];
					}
				}				
			}
#endif
		}else{/* no enough source data in input buffer*/
			int tmp_len;
			if(!(tmp_len = fread(in_buffer0 + read_size, 1, READ_SIZE, in_file)))
				goto end;
			read_size += tmp_len;

			indata.Length = read_size;

		}
		frameNum++;
//		printf("frameNum=%d\n",frameNum);
	}
end:
	voMJPEGDec.Uninit(phCodec);
#ifdef _WIN32_WCE	
	res_file = fopen("/Storage Card/DMresult.txt", "a+");
	fprintf(res_file, "\n\t totTime = %d\n\n", total);
	fclose (res_file);
#endif
	
free_memory :
	if (in_file)
		fclose(in_file);
	if (out_file)
		fclose(out_file);	
	if(in_buffer0)
		free(in_buffer0);
	
    return 0;
}

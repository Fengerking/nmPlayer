#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "voMJPEG.h"

#ifdef VOANDROID
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
static unsigned long timeGetTime(){
  struct timeval tval;
  gettimeofday(&tval, NULL);
  return tval.tv_sec*1000 + tval.tv_usec/1000;
}
#endif


#define INPUT_BUF_SIZE 1024*1024
#define READ_SIZE INPUT_BUF_SIZE/2

float decode_mjpeg(char* inmfilename,char* outmfilename,int times,int enable_ouput);

#ifdef _WIN32_WCE
//int WinMain()
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	int k=0;
    int enable_ouput=0;
    int decode_time = 0;
    float fps =0;
	char inmfilename[255];
    char outmfilename[255];
	inmfilename[0] ='\0';
    outmfilename[0]='\0';

	if(argc>1)
    {
		strcpy(inmfilename, argv[1]);
        strcpy(outmfilename, argv[2]);
        decode_time = (int)(*argv[3]-48);
        enable_ouput =(int)(*argv[4]-48);
    }
	else
		sprintf(inmfilename,"E:/MyResource/Video/clips/mjpeg/QuickTest/09511113_AVI_MJPEG_2000K39s173ms720x48030f2c48KHz16b.mjp");

#ifndef _WIN32_WCE
	srand((int)time(0));
#endif

	for(k=0;k<decode_time;k++)
	{
		printf("\n-----------------------------------------%d \n",k);
		fps += decode_mjpeg(inmfilename,outmfilename,k,enable_ouput);		
	}
    printf("------%f \n",fps/k);
}
float decode_mjpeg(char* inmfilename,char* outmfilename,int times,int enable_ouput)
{
	FILE *in_file = NULL, *out_file = NULL,*tmp_file=NULL;
	FILE *res_file = NULL;
	unsigned char *in_buffer0 = NULL;
	int file_size, read_size, frameNum=0;
	VO_HANDLE phCodec;
	VO_CODECBUFFER	indata;
	VO_VIDEO_BUFFER	outdata;
	VO_VIDEO_FORMAT	outformat;
	VO_VIDEO_OUTPUTINFO outInfo;
	VO_VIDEO_DECAPI	voMJPEGDec;
	unsigned long total = 0;
	unsigned long begin = 0;
	unsigned long end = 0;

	char errordatafilename[255];

#ifdef _WIN32_WCE	
	if(!(in_file = fopen ("\\Temp\\320x240x20.mjp", "rb"))){
		printf ("Could not open file!");
		return 0;
	}	
	if(!(out_file = fopen("\\Temp\\test.dec.yuv", "w+b"))){
		printf("Open output file error!");
		return 0;
	}
#else
#ifdef VOANDROID
	if(!(in_file = fopen (inmfilename, "rb"))){
			printf ("LINUX Could not open file \n");
		}
	if(!(out_file = fopen(outmfilename, "w+b"))){
		printf("Open output file error!");
	}
#else

	sprintf(errordatafilename,"errordata_%d.mjp",times%10);
	if(0){
		//in_file = fopen ("D:/15072516_AVI_MJPEG_12M'19s987ms'720x576'30f.mjp", "rb");
		in_file = fopen (inmfilename, "rb");
		tmp_file = fopen (errordatafilename, "wb");

		fseek(in_file, 0, SEEK_END);
		file_size = ftell(in_file);
		fseek(in_file, 0, SEEK_SET);

		while(file_size>0)
		{
			unsigned char buf[1024];
			unsigned long i,errorpos,errordata;		
			unsigned long readsize = fread(buf, 1, 1024, in_file);
			//srand((int)time(0));
			for(i=0;i<20*(readsize/1024);i++)
			{				
				errorpos  =  (unsigned long)rand()%readsize;
				errordata =(int)rand()%256;
				//printf("[%4d %4d] ",errorpos,errordata);
				buf[errorpos] = errordata;
			}
			//printf("\n");
			fwrite(buf,readsize,1,tmp_file);	
			file_size -=readsize;
		}
		fclose(in_file);
		fclose(tmp_file);
		printf("finish adding error data \n");

		inmfilename = errordatafilename;
	}

	
		
	if(!(in_file = fopen (inmfilename, "rb"))){
			printf ("------Could not open file \"%s\".\n");
		}
	if(!(out_file = fopen("E:/MyResource/Video/clips/mjpeg/QuickTest/YUV_out.yuv", "w+b"))){
		printf("Open output file error!");
	}
#endif
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


	indata.Time  = 2300;

	voGetMJPEGDecAPI(&voMJPEGDec,NULL);

	voMJPEGDec.Init(&phCodec, VO_VIDEO_CodingMJPEG, NULL);
    //normalmode = 0;
	//voMJPEGDec.SetParam(phCodec,VO_PID_DEC_MJPEG_SET_NORMAL_CHROMA,&normalmode);

	//printf(__FILE__); //
    //printf("\n%s %d",__FILE__,__LINE__ );

//#define ERROR_DATA 1
#ifdef ERROR_DATA
	srand((int)time(0));
#endif

	while(file_size>0)
	{
		int length;
		int ret;
		unsigned char *out_src;

#ifdef ERROR_DATA
			unsigned long i,errorpos,errordata;  //167 48 ; 23672 103		

			for(i=0;i<20;i++)
			{
				errorpos  =  (unsigned long)rand()%indata.Length;
				errordata =(int)rand()%256;
				printf("indata.Buffer[%d] = %d\n",errorpos,errordata);
				indata.Buffer[errorpos] = errordata;
			}

#endif

#ifdef _WIN32_WCE
		begin = GetTickCount();
#elif VOANDROID
        begin = timeGetTime();
#endif	

		voMJPEGDec.SetInputData(phCodec, &indata);
		ret=voMJPEGDec.GetOutputData(phCodec, &outdata, &outInfo);

#ifdef _WIN32_WCE
		end = GetTickCount();
#elif VOANDROID
        end = timeGetTime();
#endif

		//if(ret == VO_ERR_NONE)
		//	printf("correct !! \n");
		//else
		//	printf("error !! \n");

		outformat = outInfo.Format;

		total += end - begin;
		length = outInfo.InputUsed;
		if(length > 0)
		{
			int tmp_len, left_len = read_size - length;			
			if(left_len > 0){
				/* fill input buffer */
				memmove(in_buffer0, in_buffer0 + length, left_len);
			}
			file_size -= length;				
			tmp_len = fread((in_buffer0 + left_len), 1, READ_SIZE - left_len, in_file);			
			read_size = tmp_len + left_len;
			indata.Length = read_size;
			// if(dec_frame_info.image_type != NULL_FRAME){
#if 1		
			//	if(dec_frame_info.image_type != NULL_FRAME)
			//printf(" return error \n ");
			if(enable_ouput && ret == VO_ERR_NONE)
			{
				int i;
				//printf(" OK  \n ");				
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
		}
		else
		{/* no enough source data in input buffer*/
			int tmp_len;
			if(!(tmp_len = fread(in_buffer0 + read_size, 1, READ_SIZE, in_file)))
				goto end;
			read_size += tmp_len;
			indata.Length = read_size;
		}
		
        //printf("---%d  %d %d \n",frameNum, outInfo.Format.Width,outInfo.Format.Height);		
		if(frameNum==264)
			frameNum = frameNum;
		frameNum++;
	}
end:
	voMJPEGDec.Uninit(phCodec);
#ifdef _WIN32_WCE	
	res_file = fopen("/Storage Card/DMresult.txt", "a+");
	fprintf(res_file, "\n\t totTime = %d\n\n", total);
	fclose (res_file);
#endif
    //printf(" ----%d %d %f fps \n",frameNum, total,frameNum*1000.0/total);
	
free_memory :
	if (in_file)
		fclose(in_file);
	if (out_file)
		fclose(out_file);	
	if(in_buffer0)
		free(in_buffer0);
	
    return (frameNum*1000.0/total);
}

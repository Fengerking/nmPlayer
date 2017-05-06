#ifdef VOWINCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#elif VOWINXP
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#elif RVDS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#elif VOANDROID 
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

#include <stdio.h>
#include <stdlib.h>
//#include "mjpegenc.h"
#include "voMJPEG.h"

int soky(int a, int b) 
{
    int i,s = 1; 
    for ( i = 0; i <= b - 1; i++) 
        s = s * a; 
      return s; 
}

#define INPUT_BUF_SIZE 1024*1024
#define READ_SIZE INPUT_BUF_SIZE/2
#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else 
int main(int argc, char **argv)
#endif
{
	FILE *in_file = NULL, *out_file = NULL;
	FILE *res_file = NULL;
	unsigned char *out_buffer = NULL;
	unsigned char* Ydata = NULL;
	unsigned char* Udata = NULL;
	unsigned char* Vdata = NULL;
	int file_size, read_size, frameNum=0;
	VO_HANDLE hCodec;
	VO_VIDEO_ENCAPI vomjpegEnc;
	VO_VIDEO_BUFFER InData;
	VO_CODECBUFFER OutData;
	VOMJPEGENCRETURNCODE ret;
	//HVOCODEC hCodec;
	//VOCODECVIDEOBUFFER InData;
	//VOCODECDATABUFFER  OutData;
	unsigned long begin, end,total=0;
	int width = 0;//720;
	int height = 0;//480;
	int ysize = 0;//width * height*3;
	int usize = 0;//width * height ;
	int vsize = 0;//width * height ;
    int enable_output=0;

    char mfilename[255];
	char mfilenameout[255];
    char yuv_mode[255];
	mfilename[0] ='\0';
	mfilenameout[0] = '\0';
    yuv_mode[0] = '\0';

    printf("argc = %d \n",argc);
    if(argc==7)
	{
        int i=0,n=0;
		strcpy(mfilename, argv[1]);
		strcpy(mfilenameout, argv[2]);
        n = strlen(argv[3]);
        for(i=0;i<n;i++)
           width += (argv[3][i]-48)*soky(10,n-1-i);

        n = strlen(argv[4]);
        for(i=0;i<n;i++)
           height += (argv[4][i]-48)*soky(10,n-1-i);

        strcpy(yuv_mode, argv[5]); 
        enable_output = *argv[6]-48;
	}
	else
	{
        printf("*******************************************************************\n");    
        printf("Usage: \n");
        printf("\n./voMJPEGEncTst  input.yuv output.mjp width height yuv_mode enable_output \n");
        printf("\nThis tool support two yuv_mode: PACKED_RGB24,PLANAR_YUV420 !!! \n");
        printf("\nenable_output: 0 no output, 1 output \n ");
        printf("*******************************************************************\n");
        goto free_memory;
		//sprintf(mfilename,"E:/MyResource/Video/clips/YUV/0001#03__RGB'888__BGR'720x480.rgb");
		//sprintf(mfilenameout,"E:/MyResource/Video/clips/YUV/0001#03__RGB'888__BGR'720x480.mjp");
        //sprintf(yuv_mode,"PACKED_RGB24");
	}


	if(!(in_file = fopen (mfilename, "rb"))){
		printf ("Could not open file \"%s\".\n", argv[1]);
	}
	if(!(out_file = fopen(mfilenameout, "w+b"))){
		printf("Open output file error!");
	}

   if(strcmp(yuv_mode,"PACKED_RGB24") == 0)
   {
       ysize = width * height*3;
       //usize = width * height ;
       //vsize = width * height ;
       out_buffer = malloc(ysize + 31);
       Ydata = malloc(ysize + 31);
       Udata = 0;
       Vdata = 0;
       InData.Buffer[0] = (unsigned char*)(((unsigned int)Ydata +  31) & ~31);
       InData.Buffer[1] = 0;
       InData.Buffer[2] = 0;
       InData.Stride[0] = width*3;
       InData.Stride[1] = InData.Stride[2] = 0;	
       InData.ColorType = VO_COLOR_RGB888_PACKED;
   }
   else if(strcmp(yuv_mode,"PLANAR_YUV420") ==0)
   {
       ysize = width * height;
       usize = width * height>>2 ;
       vsize = width * height>>2 ;
       out_buffer = malloc(ysize + 31);
       Ydata = malloc(ysize + 31);
       Udata = malloc(usize + 31);
       Vdata = malloc(vsize + 31);
       InData.Buffer[0] = (unsigned char*)(((unsigned int)Ydata +  31) & ~31);
       InData.Buffer[1] = (unsigned char*)(((unsigned int)Udata +  31) & ~31);
       InData.Buffer[2] = (unsigned char*)(((unsigned int)Vdata +  31) & ~31);
       InData.Stride[0] = width;
       InData.Stride[1] = InData.Stride[2] = width>>1;	
       InData.ColorType = VO_COLOR_YUV_PLANAR420;
   }
   else
   {
       printf(" yuv mode is not supported \n");
       goto free_memory;
   }
   

	OutData.Buffer = (unsigned char*)(((unsigned int)out_buffer +  31) & ~31);
	OutData.Length = ysize;

    fseek(in_file, 0, SEEK_END);
    file_size = ftell(in_file);
    fseek(in_file, 0, SEEK_SET);

	ret = voGetMJPEGEncAPI(&vomjpegEnc);

	vomjpegEnc.Init(&hCodec,VO_VIDEO_CodingMJPEG,NULL);
	vomjpegEnc.SetParam(hCodec, VO_PID_ENC_MJPEG_WIDTH, &width);
	vomjpegEnc.SetParam(hCodec, VO_PID_ENC_MJPEG_HEIGHT, &height);	
	while(file_size>0)
	{
		int length;
		/* fill input buffer*/
        if(InData.ColorType == VO_COLOR_RGB888_PACKED)
        {
            if(!(read_size = fread(InData.Buffer[0], 1, ysize, in_file)))
                goto free_memory;
        }
        else if(InData.ColorType == VO_COLOR_YUV_PLANAR420)
        {
            if(!(read_size = fread(InData.Buffer[0], 1, ysize, in_file)))
                goto free_memory;
            if(!(read_size = fread(InData.Buffer[1] , 1, usize, in_file)))
                goto free_memory;
            if(!(read_size = fread(InData.Buffer[2] , 1, vsize, in_file)))
                goto free_memory;
        }

		length = ysize + usize + vsize;
		
		OutData.Buffer = (unsigned char*)(((unsigned int)out_buffer +  31) & ~31);
		OutData.Length = ysize;

		file_size -= length;
		//printf("%d \n",file_size);

#ifdef _WIN32_WCE
		begin = GetTickCount();
#elif VOANDROID
        begin = timeGetTime();
#else
		begin = clock();
#endif
		vomjpegEnc.Process(hCodec, &InData, &OutData,NULL);
#ifdef _WIN32_WCE
		end = GetTickCount();
		total += end-begin;
#elif VOANDROID
	    end =  timeGetTime();
	    total += end-begin;
#else
	    end = clock();
	    total += end-begin;
#endif

//if(dec_frame_info.image_type != NULL_FRAME){
#if 1			
//if(dec_frame_info.image_type != NULL_FRAME)
        if(enable_output)
        {
            if(!(read_size = fwrite(OutData.Buffer, OutData.Length, 1, out_file)))
                printf("error write!!!");
        }
			
#endif
		frameNum++;
		if(frameNum>1000) 
			break;

	}

#ifdef _WIN32_WCE
		{
			FILE *fp=fopen("/Storage Card/TIFF/performance.txt","a");
			fprintf(fp,"%d ms frameNum= %d\n",total,frameNum);
		}

#endif
	vomjpegEnc.Uninit(hCodec);

    printf("tatal %d frames,  %f fps \n",frameNum,frameNum*1000/(total*1.0));
	
free_memory :
	if (in_file)
		fclose(in_file);
	if (out_file)
		fclose(out_file);

	if(out_buffer)
		free(out_buffer);

	if(Ydata)
		free(Ydata);

	if(Udata)
		free(Udata);

	if(Vdata)
		free(Vdata);
	
    return 0;
}

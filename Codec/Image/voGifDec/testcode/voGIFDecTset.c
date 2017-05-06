#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "voGif.h"

#ifdef _WIN32_WCE
//int WinMain()
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	int ret;
	unsigned long t1,t2,total=0;
	VO_HANDLE phCodec;
	VO_IMAGE_DECAPI voGIFDec;
	VO_VIDEO_BUFFER	outdata;

	unsigned int width,height,frames=0;

#ifdef _WIN32_WCE
	char* path = "/Temp/Gif/0001#010_GIF_GIF'89a_3264x2448.gif"; 
	char* filename[250];
	FILE *fout = NULL;
#else
	#ifdef LINUX
	char* path = "/data/local/1280x960.jpg";
	#else
	char *path = "E:/GiffImage/cycle_237x216_4bpp.gif";
	char* filename[250];
	FILE *fout = NULL;
	#endif
#endif

#ifdef _WIN32_WCE
		t1 = GetTickCount();
#endif
	voGetGIFDecAPI(&voGIFDec,NULL);
	ret = voGIFDec.Init(&phCodec,VO_IMAGE_CodingGIF,NULL);
	if(ret!=VO_ERR_NONE)
		return-1;

	ret = voGIFDec.SetParam(phCodec, VO_PID_GIF_FILEPATH, (VO_PTR)path);
	if(ret!=VO_ERR_NONE)
	{
		voGIFDec.Uninit(phCodec);
		return -1;
	}
	ret = voGIFDec.GetParam(phCodec, VO_PID_GIF_WIDTH, &width);
	ret = voGIFDec.GetParam(phCodec, VO_PID_GIF_HEIGHT, &height);

	memset(&outdata,0,sizeof(VO_VIDEO_BUFFER));
	outdata.Buffer[0] = (unsigned char*)malloc(sizeof(unsigned char)*width*height*3);
	if(outdata.Buffer[0]==NULL)
	{
		voGIFDec.Uninit(phCodec);
		return -1;
	}
#define WRITE_BMP
	do
	{
		ret = voGIFDec.GetParam(phCodec, VO_PID_GIF_OUTPUTDATA, &outdata);

#ifdef WRITE_BMP
		if(ret==VO_ERR_GIFDEC_DECODE_UNFINISHED)
		{
#ifdef _WIN32_WCE
		sprintf(filename, "/Storage Card/gif/result%d_%d.bmp",frames++,outdata.Time);
#else
		sprintf(filename, "E:/GiffImage/bmp/result%d.bmp",frames++);
#endif
			fout = writebmphead(filename,width,height,3,NULL,0);
			writebmpdata(fout,outdata.Buffer[0],width,height,width*3,3);
			writebmptail(fout);
		}
#else
		frames++;
#endif
	}while(ret==VO_ERR_GIFDEC_DECODE_UNFINISHED);

	ret = voGIFDec.Uninit(phCodec);

		frames-=1;
#ifdef _WIN32_WCE
		t2 = GetTickCount();
		total +=t2-t1;
		{
			FILE* fp=fopen("/Temp/Gif/gif_speed.txt","a");
			fprintf(fp,"zou:%dms,%dframes,%2.1ffps\n",total,frames,frames*1000.0/total);
			fclose(fp);
		}
#endif
	free(outdata.Buffer[0]);
	outdata.Buffer[0] =NULL;

	return 0;

}

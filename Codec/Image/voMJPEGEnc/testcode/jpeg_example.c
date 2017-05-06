#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#include "jpegenc.h"
#include "voJPEG.h"

char* make = "VISLON";
char* Model = "VO_001";
char* MapData = "SHANGHAI.CHINA";
char* Name = "Jacky Lin SH";

#ifdef _WIN32_WCE
int GetTimeString(char* dataTime)
{
	SYSTEMTIME stm;
	GetLocalTime(&stm);
	sprintf(dataTime, "%4d:%2d:%2d %2d:%2d:%2d", stm.wYear, stm.wMonth, stm.wDay, 
			stm.wHour, stm.wMinute, stm.wSecond);
	
	if(dataTime[ 5] == ' ') dataTime[ 5] = '0';
	if(dataTime[ 8] == ' ') dataTime[ 8] = '0';
	if(dataTime[11] == ' ') dataTime[11] = '0';
	if(dataTime[14] == ' ') dataTime[14] = '0';
	if(dataTime[17] == ' ') dataTime[17] = '0';

	return 0;
}
#endif

void PLANAR411H_to_PLANAR420(unsigned char *src, int SrcStride, unsigned char *dst, int DestStride)
{
	int i,j;
	for(i = 0; i < 968/2 ; i++)
	{
		for(j=0;j<DestStride;j+=2)
			dst[j] = dst[j+1] = (src[j/2] + src[j/2 + SrcStride] + 1) >> 1;

		src += (SrcStride << 1);
		dst += DestStride;
	}	
}

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else
int main(int argc, char **argv) 
#endif
{
	FILE *in_file = NULL, *out_file = NULL, *out_file1 = NULL;
	FILE *res_file = NULL;
	char  DateTime[20] = {0x80, 0x81, 0x02, 0x83};	
	unsigned char* Ydata;
	unsigned char* Udata;
	unsigned char* Vdata;
	unsigned char* out_buffer;
	int read_size;
	int Degree[6];
	VO_HANDLE hCodec;
	VO_IMAGE_ENCAPI vojpegEnc;	  
	VO_VIDEO_BUFFER InData;
	VO_CODECBUFFER OutData;
	VO_VIDEO_FORMAT	outformat;	
	VOJPEGENCRETURNCODE ret;

	long total = 0;
	int width = 720;
	int height = 480;
	int quality = 100;
	int exifinfo_suppt = 1;
	int thumpic_suprt  = 0;
	int reversal       = 1;
	long thumbnail_width = 150;
	long thumbnail_height = 120;
	int ysize = width * height;//width * height;
	int usize = width * height >> 2;//2;
	int vsize = width * height >> 2;//2;
	int totalsize;
	int temp1 = *((int*)DateTime);
	int temp2 = temp1;

#ifdef _WIN32_WCE	
	if(!(in_file = fopen ("E:/MyResource/Video/clips/YUV/720x480_out.yuv", "rb"))){
		printf ("Could not open file!");
	}	
	
	if(!(out_file = fopen("E:/MyResource/Video/clips/YUV/En_smallpic.jpg", "w+b"))){
		printf("Open output file error!");
	}
	GetTimeString(DateTime);
#else 
	if(!(in_file = fopen ("E:/MyResource/Video/clips/YUV/720x480_out.yuv", "rb"))){ //E:/MyResource/Video/clips/YUV
		printf ("Could not open file!>\n");
	}

	if(!(out_file = fopen("E:/MyResource/Video/clips/YUV/smallpic_3.jpg" , "w+b"))){
		printf("Open output file error!");
	}
#endif
	 

	totalsize = ysize + usize + vsize;
	out_buffer = (unsigned char*)malloc(65536 + 31);
	
	//Ydata = (unsigned char*)malloc(totalsize + 31);
	//Udata = (unsigned char*)malloc(usize + 31);
	//Vdata = (unsigned char*)malloc(vsize + 31);

	Ydata = (unsigned char*)malloc(ysize + 31);
	Udata = (unsigned char*)malloc(usize + 31);
	Vdata = (unsigned char*)malloc(vsize + 31);

	InData.Buffer[0] = (unsigned char* )(((unsigned int)Ydata + 31) & ~31);
	InData.Buffer[1] = (unsigned char* )(((unsigned int)Udata + 31) & ~31);
	InData.Buffer[2] = (unsigned char* )(((unsigned int)Vdata + 31) & ~31);
	
	InData.Stride[0] = width;
	InData.Stride[1] = InData.Stride[2] = width>>1; 
	
	InData.ColorType = VO_COLOR_YUV_PLANAR420;//VO_COLOR_YUV_PLANAR422_12;//VO_COLOR_YUV_PLANAR420;
	
	OutData.Buffer = (unsigned char* )(((unsigned int)out_buffer + 31) & ~31);
	OutData.Length = 65536;

	ret = voGetJPEGEncAPI(&vojpegEnc);
	ret = vojpegEnc.Init(&hCodec,VO_IMAGE_CodingJPEG,NULL);
	
	//voJPEGEncInit(&hCodec);	
	/* fill input buffer*/
	if(!(read_size = fread(InData.Buffer[0], 1, ysize, in_file)))
		goto free_memory;
	if(!(read_size = fread(InData.Buffer[1], 1, usize, in_file)))
		goto free_memory;
	if(!(read_size = fread(InData.Buffer[2], 1, vsize, in_file)))
		goto free_memory;
	vojpegEnc.SetParam(hCodec, VO_PID_JPEG_WIDTH, &width);
	vojpegEnc.SetParam(hCodec, VO_PID_JPEG_HEIGHT, &height);
	vojpegEnc.SetParam(hCodec, VO_PID_JPEG_QUALITY, &quality);
	vojpegEnc.SetParam(hCodec, VO_PID_EXIFINFO_SUPPT, &exifinfo_suppt);
	vojpegEnc.SetParam(hCodec, VO_PID_THUMPIC_SUPPT, &thumpic_suprt);

	vojpegEnc.SetParam(hCodec, VO_PID_THUMPIC_WIDTH, &thumbnail_width);
	vojpegEnc.SetParam(hCodec, VO_PID_THUMPIC_HEIGHT, &thumbnail_height);

	//vojpegEnc.SetParam(hCodec, VO_PID_JPEG_REVERSAL, &reversal);  //zou 2010.1.5
	
	Degree[0] = 100; Degree[1] = 1;
	vojpegEnc.SetParam(hCodec, VO_PID_PICXRESOLUTION, Degree);
	vojpegEnc.SetParam(hCodec, VO_PID_PICYRESOLUTION, Degree);

	//vojpegEnc.SetParam(hCodec, VO_PID_MACHINE_MAKE, make);
	//vojpegEnc.SetParam(hCodec, VO_PID_MACHINE_MODEL, Model);
	//vojpegEnc.SetParam(hCodec, VO_PID_JPEG_WINAUTHOR,Name);	
	//vojpegEnc.SetParam(hCodec, VO_PID_PICTURE_DATE, DateTime);
	   //vojpegEnc.SetParam(hCodec, VOID_GPS_LATIREF, NORTH_LATITITUDE);
	Degree[0] = 31; Degree[1] = 1;
	Degree[2] = 17; Degree[3] = 1;
	Degree[4] = 4412;	Degree[5] = 100;
	vojpegEnc.SetParam(hCodec, VO_PID_GPS_LATITUDE, Degree);
	//vojpegEnc.SetParam(hCodec, VOID_GPS_LATIREF, EAST_LONGITUDE);

	Degree[0] = 121; Degree[1] = 1;
	Degree[2] = 29; Degree[3] = 1;
	Degree[4] = 20;	Degree[5] = 6;
	vojpegEnc.SetParam(hCodec, VO_PID_GPS_LONGITUDE, Degree);	
	Degree[0] = 10; Degree[1] = 1;
	vojpegEnc.SetParam(hCodec, VO_PID_GPS_ALITUDE, Degree);
	vojpegEnc.SetParam(hCodec, VO_PID_GPS_MAPDATUM, MapData);
	vojpegEnc.SetParam(hCodec, VO_PID_GPS_DATESTAMP, DateTime);
	

	do {
		ret = vojpegEnc.Process(hCodec, &InData, &OutData,&outformat);
		if(!(read_size = fwrite(OutData.Buffer, OutData.Length, 1, out_file)))
			printf("error write!!!");
		OutData.Length = 65536;	
	} while(ret == VO_ERR_JPEGENC_ENCUNFINISHED);

	vojpegEnc.Uninit(hCodec);
	
free_memory :
	if (in_file)
		fclose(in_file);
	if (out_file)
		fclose(out_file);
	//if (out_file1)
		//fclose(out_file1);

	if(out_buffer)
		free(out_buffer);
	if(Ydata)
		free(Ydata);
    return 0;
}





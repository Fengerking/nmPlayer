#ifdef __cplusplus
extern "C" {
#endif

#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<time.h>
#include		<limits.h>
#include 		<stdarg.h>
#include		"annexb.h"

#ifdef WIN32
#include		<windows.h>
#include		<winbase.h>
#endif

/*
mobim test:
-O2 -Otime -W -zc -apcs /ropi -cpu ARM926EJ-S -fy -Wabdfrvy -Ecf -DNDEBUG -DDISABLE_LICENSE -DARM -DLINUX -DARM_ASM -DDISABLE_MCORE -DRVDS
*/
#if 1//!RENAME_BP
int gLogLevel = LL_ALL & ~(LL_INFO | LL_FLOW | LL_INOUT | LL_WARN);
//int gLogLevel = LL_ALL;

void PrintVersion()
{
	AvdLog2(LL_INFO,"\nvoH264Dec:build time:%s  %s\n", __TIME__,  __DATE__);
	//printf("\nvoH264Dec:build time:%s  %s\n", __TIME__,  __DATE__);
}
void voH264Memset(void * s, int d, size_t size)
{
	memset(s,d,size);
}
unsigned long voH264GetCurTime()
{
#ifndef _WIN32_WCE
		return clock();
#else
		return 0;
#endif
}
void voH264Printf(const char * format,...)
{
	va_list arg;
	va_start(arg, format);
	vprintf(format, arg);
	va_end(arg);
}
void voPrintf(const char * format,...)
{
	va_list arg;
	va_start(arg, format);
	vprintf(format, arg);
	va_end(arg);
}
void voH264Sprintf(char* tmpBuf,char* format,...)
{
	va_list arg;
	va_start(arg, format);
	vsprintf(tmpBuf,format,arg);
	va_end(arg);
}
void voH264Memcpy(void * dst, const void * src, size_t size)
{
	memcpy(dst,src,size);
}
void voH264Memmove(void * dst, const void * src, size_t size)
{
	memmove(dst,src,size);
}
void* voH264Calloc(size_t s, size_t s2)
{
	void* ret=NULL;
	//AvdLog2(LL_INFO,"before voH264Calloc,size=(%d,%d)\n",s,s2);
	ret = calloc(s, s2);
	//AvdLog2(LL_INFO,"after calloc1,%X\n",ret);
	return ret;
}

void voH264Free(void* d)
{
	free(d);
}
#ifndef _WIN32
void voH264Qsort(void * a, size_t b, size_t c,
           int (*d)(const void *, const void *))
{
	qsort(a,b,c,d);
}
#endif// _WIN32
int voH264IntDiv(int a,int b) 
{
	return a/b;
}
int voH264IntMod(int a,int b) 
{
	return a%b;
}
#if !defined(X86_TEST)
void AvdLog(int logId, char *format, ...)
{
	va_list arg;
	va_start(arg, format);
#ifdef G1
	//voLog_android_264_debug
#else
	vprintf(format, arg);
#endif
	va_end(arg);
}
#endif//defined(X86_TEST)
#if ENABLE_TRACE
#ifndef DISABLE_MCORE
#include "voThread.h"
#include "voOSFunc.h"
#endif//
#define ONLY_KEEP_LOGCAT 0 //for some devices, the log will cause crash
#define TRACE_OTHERS
#if defined(X86_TEST) 
#define TRACE_INPUT_RAW
#define TRACE_INPUT 
#define TRACE_OUTPUT 
#endif//(X86_TEST) 
static FILE* logfile2=NULL;
static FILE* datafile=NULL;
static FILE* streamFile=NULL;
static FILE* rawbitFile=NULL;
FILE* memFile=NULL;//will be used in ldecod.c
FILE* getMemFile(){return memFile;}
#ifdef X86_TEST
char* DMPH264_IN="e:/h264decIn.vo264";
char* DMPH264_OUT= "e:/h264decYUV.yuv";
char* DMPH264_INFO= "e:/h264decInfo.txt";
char* DUMPH264_IN_RAW=  "e:/in264.264";
char* DUMPH264_MEM=  "e:/voH264memIssues.txt";
char* DMPH264_IN2="c:/h264decIn.vo264";
char* DMPH264_OUT2= "c:/h264decYUV.yuv";
char* DMPH264_INFO2= "c:/h264decInfo.txt";
char* DUMPH264_IN_RAW2=  "c:/in264.264";
char* DUMPH264_MEM2=  "c:/voH264memIssues.txt";
#endif//X86 for checkin
#if defined( _MAC_OS) || defined(_IOS)
char* DMPH264_IN="/h264decIn.vo264";
char* DMPH264_OUT= "/h264decYUV.yuv";
char* DMPH264_INFO= "/h264decInfo.txt";
char* DUMPH264_IN_RAW=  "/in264.264";
char* DUMPH264_MEM=  "/voH264memIssues.txt";
char* DMPH264_IN2="/h264decIn.vo264";
char* DMPH264_OUT2= "/h264decYUV.yuv";
char* DMPH264_INFO2= "/h264decInfo.txt";
char* DUMPH264_IN_RAW2=  "/in264.264";
char* DUMPH264_MEM2=  "/voH264memIssues.txt";
#endif//X86
#ifdef _LINUX_ANDROID
char* DMPH264_IN="/sdcard/h264decIn.vo264";
char* DMPH264_OUT= "/sdcard/h264decYUV.yuv";
char* DMPH264_INFO= "/sdcard/h264decInfo.txt";
char* DUMPH264_IN_RAW=  "/sdcard/in264.264";
char* DUMPH264_MEM=  "/sdcard/voH264memIssues.txt";
char* DMPH264_IN2="/data/local/h264decIn.vo264";
char* DMPH264_OUT2= "/data/local/h264decYUV.yuv";
char* DMPH264_INFO2= "/data/local/h264decInfo.txt";
char* DUMPH264_IN_RAW2= "/data/local/in264.264";
char* DUMPH264_MEM2=  "/data/local/voH264memIssues.txt";
#elif defined(LINUX)
char* DMPH264_IN="./h264decIn.vo264";;
char* DMPH264_OUT= "./h264decYUV.yuv";
char* DMPH264_INFO= "./h264decInfo.txt";
char* DUMPH264_IN_RAW=  "./in264.264";
char* DUMPH264_MEM=  "./voH264memIssues.txt";
char* DMPH264_IN2="./h264decIn.vo264";;
char* DMPH264_OUT2= "./h264decYUV.yuv";
char* DMPH264_INFO2= "./h264decInfo.txt";
char* DUMPH264_IN_RAW2=  "./in264.264";
char* DUMPH264_MEM2=  "./voH264memIssues.txt";
#elif defined(_WIN32_WCE)
char* DMPH264_IN="/sdmmc/h264decIn.vo264";
char* DMPH264_OUT= "/sdmmc/h264decYUV.yuv";
char* DMPH264_INFO= "/sdmmc/h264decInfo.txt";
char* DUMPH264_IN_RAW=  "/sdmmc/in264.264";
char* DUMPH264_MEM=  "/sdmmc/voH264memIssues.txt";
char* DMPH264_IN2="/storagecard/h264decIn.vo264";
char* DMPH264_OUT2= "/storagecard/h264decYUV.yuv";
char* DMPH264_INFO2= "/storagecard/h264decInfo.txt";
char* DUMPH264_IN_RAW2=  "/storagecard/in264.264";
char* DUMPH264_MEM2=  "/storagecard/voH264memIssues.txt";
#endif//OUTPUT_ANDROID_INFO

#ifdef WIN32
HANDLE hMutex;
#endif

void InitLog2()
{
#ifdef _IOS
	char preDir[512];
	char tmpPath[512];
	voOS_GetAppFolder(tmpPath,512);
#ifdef TRACE_OTHERS
	if(logfile2==NULL)
	{
		strcpy(preDir,tmpPath);strcat(preDir,DMPH264_INFO);
		logfile2=fopen(preDir,"wb");
		printf("infoLog=%s\n",preDir);
	}
#endif//TRACE_OTHERS
#ifdef TRACE_OUTPUT
	if(datafile==NULL)
	{
		strcpy(preDir,tmpPath);strcat(preDir,DMPH264_OUT);
		datafile=fopen(preDir,"wb");
		printf("outLog=%s\n",preDir);
	}
#endif//TRACE_OUTPUT
#ifdef TRACE_INPUT
	if(streamFile==NULL)
	{
		strcpy(preDir,tmpPath);strcat(preDir,DMPH264_IN);
		streamFile=fopen(preDir,"wb");
		printf("inStreamLog=%s\n",preDir);
	}
#endif//TRACE_OUTPUT
#ifdef TRACE_INPUT_RAW
	if(rawbitFile==NULL)
	{
		strcpy(preDir,tmpPath);strcat(preDir,DUMPH264_IN_RAW ) ;
		rawbitFile = fopen(preDir,"wb");
		printf("inRawStreamLog=%s\n",preDir);
	}
#endif//TRACE_INPUT_RAW
#ifdef TRACE_MEM
	if(memFile==NULL)
	{
		strcpy(preDir,tmpPath);strcat(preDir,DUMPH264_MEM ) ;
		memFile = fopen(preDir,"wb");
		printf("inMemLog=%s\n",preDir);
	}
#endif//TRACE_INPUT_RAW
#else//_IOS

#ifdef TRACE_OTHERS
	if(logfile2==NULL&&!ONLY_KEEP_LOGCAT)
	{
		logfile2=fopen(DMPH264_INFO,"wb");
		if(logfile2==NULL)
		{
			logfile2=fopen(DMPH264_INFO2,"wb");

		}
	}
#endif//TRACE_OTHERS
#ifdef TRACE_OUTPUT
	if(datafile==NULL&&!ONLY_KEEP_LOGCAT)
	{
		datafile=fopen(DMPH264_OUT,"wb");
		if(datafile==NULL)
		{
			datafile=fopen(DMPH264_OUT2,"wb");
		}
	}
#endif//TRACE_OUTPUT
#ifdef TRACE_INPUT
	if(streamFile==NULL&&!ONLY_KEEP_LOGCAT)
	{
		streamFile=fopen(DMPH264_IN,"wb");
		if(streamFile==NULL)
		{
			streamFile=fopen(DMPH264_IN2,"wb");
		}
	}
#endif//TRACE_OUTPUT
#ifdef TRACE_INPUT_RAW
	if(rawbitFile==NULL&&!ONLY_KEEP_LOGCAT)
	{
		rawbitFile = fopen(DUMPH264_IN_RAW  ,"wb");
		if(rawbitFile==NULL)
		{
			rawbitFile = fopen(DUMPH264_IN_RAW2  ,"wb");
		}
	}
#endif//TRACE_INPUT_RAW
#ifdef TRACE_MEM
	if(memFile==NULL)
	{
		memFile = fopen(DUMPH264_MEM  ,"wb");
		if(memFile==NULL)
		{
			memFile = fopen(DUMPH264_MEM2  ,"wb");
		}
	}
#endif//TRACE_INPUT_RAW
#endif//_IOS
#ifdef WIN32
hMutex=CreateMutex(NULL,FALSE,NULL);
#endif
}
void UninitLog2()
{
	if(logfile2)
	{
		fclose(logfile2);
		logfile2=NULL;
	}
	if(datafile)
	{
		fclose(datafile);
		datafile=NULL;
	}
	if(streamFile)
	{
		fclose(streamFile);
		streamFile=NULL;
	}
	if(rawbitFile)
	{
		fclose(rawbitFile);
		rawbitFile=NULL;
	}
	if (memFile)
	{
		fclose(memFile);
		memFile=NULL;
	}
}
void AvdOutData(char *data,int width,int height)
{
	if(datafile)
	{
		fwrite(data,1,width*height*3/2,datafile);
		fflush(datafile);
	}
}
void AvdInData(char *buf,int size)
{
	FILE* file=streamFile;
	int actualWrite=0;
	if(file==NULL)
		return;
	//add prefix
	fwrite(&size,1,4,file);
	actualWrite=fwrite(buf,1,size,file);
	fflush(file);
}


void DUMPDataIn(char* data,int size,int prefixAdd)
{
	int prefix;
	if(rawbitFile==NULL)
			return;
#if 0
	switch (prefixAdd)
	{
	case 4:
		prefix=0x01000000;
		fwrite(&prefix,4,1,rawbitFile);
		break;
	case 3:
		prefix=0x010000;
		
		fwrite(&prefix,3,1,rawbitFile);
	default:
		break;
	}
#else
	if (prefixAdd)
	{
		prefix=0x01000000;
		fwrite(&prefix,4,1,rawbitFile);
	}
	
#endif

	fwrite(data,size,1,rawbitFile);
	fflush(rawbitFile);
};
static char tmpBuf[1024];
void DUMPDataToLOG(char* data,int size)
{
	char tmpChar[10]={0};
	tmpBuf[0]='\0';
	do 
	{
		sprintf(tmpChar,"%02X ",*data++);
		strcat(tmpBuf,tmpChar);
	} while (--size>0);
	printf(tmpBuf);
#ifdef  _LINUX_ANDROID
	voLogAndroidInfo(tmpBuf);
#endif
}
static int isLogging = 0;
#if 0
void TAvdLog(int level,char *format, ...)
{
	FILE *fp=logfile2;
	va_list arg;
	if (fp)
	{
		va_start(arg, format);
		vfprintf(fp, format, arg);
		vprintf(format, arg);
		va_end(arg);
		fflush(fp);
	}
	
}
#endif
void AvdLog2(int level,char *format, ...)
{
#ifdef DEBUG_INFO
	FILE *fp=logfile2;
	va_list arg;
	if ((gLogLevel & level)==0)
		return;
#ifndef DISABLE_MCORE
#ifdef WIN32
       WaitForSingleObject(hMutex,INFINITE);
#else
	while(isLogging);
#endif	
#endif//DISABLE_MCORE
	isLogging = 1;
	va_start(arg, format);
	vsprintf(tmpBuf, format, arg);
	va_end(arg);
	if (fp)
	{
		fprintf(fp,"%s",tmpBuf);
		fflush(fp);
	}
	printf(tmpBuf);
	
#ifdef  _LINUX_ANDROID
	voLogAndroidInfo(tmpBuf);
#endif
	isLogging = 0;
#endif//DEBUG_INFO
#ifdef WIN32
       ReleaseMutex(hMutex);
#endif	
}

#endif//ENABLE_TRACE
#if defined (WIN32) || defined (_WIN32_WCE)
#include <windows.h>
#include <stdio.h>
#endif//
int voH264GetSysTime()
{
#ifdef WIN32
	LARGE_INTEGER t1, t2, tf;
	//QueryPerformanceFrequency(&tf);
	//printf("Frequency: %u\n", tf.QuadPart);
	QueryPerformanceCounter(&t1);
	return t1.QuadPart;
#elif defined(_WIN32_WCE)
	return GetClickCount();
#else
	return (int)clock();
#endif
}
#endif//!RENAME_BP
#ifdef __cplusplus
}
#endif


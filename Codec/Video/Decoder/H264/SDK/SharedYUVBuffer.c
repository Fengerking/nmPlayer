#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************
THIS is a sample code to show the usage of sharing YUV buffer in framework side.
The caller(usually,the render) is responsible for maintaining the buffer queue.
************************************************************************/
#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		"sdkbase.h"
#include		"sharedYUVMem.h"
#include		<windows.h>
static VOMEM_VIDEO_INFO memInfo={0};
typedef struct  
{
	char* buf;
	int locked;
}TSharedBuffer;
static TSharedBuffer* sharedBufs=NULL;
static HANDLE		renderThread;
static	int stop=0;
#define DEBUG_SLEEP_TIME  50
extern int frameNumber;
static DWORD ThreadProc(LPVOID lParam)
{
	int i=0;
	FILE* outFile=fopen("c:/testSharedMem2.yuv","w");
	int framesize=memInfo.Stride*memInfo.Height*3/2;
	int frameNum=0;
	printf("\nrender thread start!!!\n");
	if(outFile==NULL)
	{
		printf("\noutFile==NULL exit!!!\n");
		return -1;
	}
	do 
	{
		for (i=0;i<memInfo.FrameCount;i++)
		{
			if(sharedBufs[i].locked)
			{
				fwrite(sharedBufs[i].buf,1,framesize,outFile);
				fflush(outFile);
				
				Sleep(DEBUG_SLEEP_TIME);
				sharedBufs[i].locked=0;//unlock it
				printf("Unlock frame_%d,size=%d,buf[%d]=%X\n",frameNumber++,framesize,i,sharedBufs[i].buf);
			}
		}
		Sleep(0);
	} while(stop==0);
	fclose(outFile);
	printf("\nrender thread stop!!!\n");
}
static unsigned long SharedYUVMemInit (long uID, VOMEM_VIDEO_INFO * pVideoMem)
{
	int i=0;
	
	//allocate the memory
	printf("\nSharedYUVMemInit\n");
	memcpy(&memInfo,pVideoMem,sizeof(VOMEM_VIDEO_INFO));
	if(memInfo.FrameCount>0)
	{
		int framesize;
		
		framesize=memInfo.Stride*memInfo.Height*3/2;
		memInfo.VBuffer=malloc(memInfo.FrameCount);
		sharedBufs=malloc(sizeof(TSharedBuffer)*memInfo.FrameCount);
		if(memInfo.VBuffer&&sharedBufs)
		{
			//char** bufs=(char**)memInfo.VBuffer;
			//char*  begin=(char*)memInfo.VBuffer;
			for (i=0;i<memInfo.FrameCount;i++)//,bufs++,begin+=framesize)
			{
				sharedBufs[i].buf=memInfo.VBuffer[i]=malloc(framesize);
				printf("sharedBuf%d=0x%X\n",i,sharedBufs[i].buf);
				sharedBufs[i].locked=0;
			}
		}
		pVideoMem->VBuffer=memInfo.VBuffer;
	}
	//test code
	stop=0;
	renderThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, 0, 0, NULL);
	
	return 0;
}
static unsigned long SharedYUVMemGetBufByIndex (long uID, long nIndex)
{
	if(sharedBufs[nIndex].locked)
	{
		printf("Fail to get %d buf\n",nIndex);
		return -1;
	}
	else
	{
		printf("Get the unLocked buf [%d]  0x%X \n",nIndex,sharedBufs[nIndex].buf);
		return 0;
	}
}
static unsigned long SharedYUVMemUninit (long uID)
{
	printf("\nSharedYUVMemUninit\n");
	//free the memory
	if(memInfo.VBuffer)
	{
		int i;
		for (i=0;i<memInfo.FrameCount;i++)
		{
			free(memInfo.VBuffer[i]);
			memInfo.VBuffer[i]=0;
		}
		
		free(sharedBufs);
		sharedBufs=NULL;
	}
	//test code
	stop=1;
	Sleep(10);
	CloseHandle(renderThread);
	return 0;
}
int 	SharedMemLock(char* buf)
{
	int bufNum=memInfo.FrameCount;
	int i=0;
	
	for (i=0;i<bufNum;i++)
	{
		if(sharedBufs[i].buf==buf)
		{
			printf("locking[%d] %X\n",i,buf);
			sharedBufs[i].locked=1;
		}
	}
	return 0;
}


int  GetSharedMemOP(VOMEM_VIDEO_OPERATOR* memOP)
{
	memOP->Init					 =SharedYUVMemInit;
	memOP->GetBufByIndex=SharedYUVMemGetBufByIndex;
	memOP->Uninit				 =SharedYUVMemUninit;
	return 1;
}
//MultiThread test

int	 WaitForUnlockedBuf(int timeByMS)
{
	int bufNum=memInfo.FrameCount;
	int i=0;
	do 
	{
		
		for (i=0;i<bufNum;i++)
		{
			if(sharedBufs[i].locked==0)
			{
				printf("WaitForUnlockedBuf...%d is unlocked\n",i);
				return 1;
			}
		}
		Sleep(timeByMS);
	} while(1);
	
	return 0;
}

#ifdef __cplusplus
}
#endif


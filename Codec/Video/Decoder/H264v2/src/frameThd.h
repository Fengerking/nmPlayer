#ifndef _FRAME_THD_H
#define _FRAME_THD_H

#include "voRVThread.h"
#include "voVideo.h"

#define COMBINE_FRAME	1
#define MAX_FRAME_INPUT 100
#define FLUSH_ERROR		1
#define CHECK_REF_WRONG		0
#define USE_WIN32_BROADCAST 1

#define MAX_THREAD_NUM2 8
#define MAX_INPUT_SIZE	(2*1024*1024)
#define INPUT_SIZE_INC_STEP (128*1024)

typedef enum
{
	FRAME_STATE_READY,
	FRAME_STATE_SETTING,
	FRAME_STATE_DECODING,
	FRAME_STATE_OUTPUT
}FrameState;

struct h264dec_g;
struct slice;
struct storable_picture;
typedef struct _frameThdInfo 
{
	pthread_t thread_id;
	pthread_mutex_t * stateMutex;
	pthread_mutex_t * progressMutex;
	pthread_mutex_t * outputMutex;
	pthread_cond_t * setupCond;
	pthread_cond_t * readyCond;
	pthread_cond_t * progressCond;
// 	pthread_cond_t * outputCond;
	volatile VO_S32 runCMD;		//run or stop, controlled by main thread 
	volatile VO_S32 bRunning;	//running state
	volatile VO_S32 errFlag;
	volatile VO_U32 usedBytes;
	volatile FrameState state;

	VO_U8 *data;				
	VO_U32 dataSize;		//data length is dataSize*INPUT_SIZE_INC_STEP
	struct h264dec_g *pDecGlobal;
	struct h264dec_g *pSelfGlobal;
	struct _frameThdInfo* pThd;
}FrameThdInfo;

typedef struct _h264ThreadParam2
{
	FrameThdInfo *selfInfo;
	struct h264dec_g *pDecGlobal;
}voH264ThreadParam2;

typedef struct _frameThdPool 
{
	pthread_mutex_t stateMutex[MAX_THREAD_NUM2];
	pthread_mutex_t progressMutex;
	pthread_mutex_t outputMutex;
	pthread_cond_t setupCond[MAX_THREAD_NUM2];
	pthread_cond_t readyCond[MAX_THREAD_NUM2];
	pthread_cond_t progressCond;
// 	pthread_cond_t outputCond;
	FrameThdInfo thdInfo[MAX_THREAD_NUM2];
// 	VO_U8 *pThdPriv[MAX_THREAD_NUM2];			//for allocate all threads' input data memory
	void *pThdContext;		//for allocate all threads' "SelfGlobal" memory
}FrameThdPool;

#define THD_COND_WAIT(cond, mutex, statement)\
do{\
	if (statement)\
	{\
		pthread_mutex_lock(mutex);\
		while (statement)\
		{\
			pthread_cond_wait(cond, mutex);\
		}\
		pthread_mutex_unlock(mutex);\
	}\
}while(0)

extern inline VO_S32 WaitForProgress(FrameThdInfo *const pThdInfo, const struct storable_picture * const p, const VO_S32 y, const VO_U32 field);
VO_S32 DecodeFrameMain(struct h264dec_g *pDecGlobal, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo, VO_S32 newInput);
VO_U32 InitH264Threads2(struct h264dec_g *pDecGlobal);
VO_U32 ReleaseH264Threads2(struct h264dec_g *pDecGlobal);

#endif

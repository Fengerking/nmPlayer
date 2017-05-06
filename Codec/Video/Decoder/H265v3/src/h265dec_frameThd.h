#ifndef _FRAME_THD_H
#define _FRAME_THD_H

#include "h265dec_RVThread.h"
#include "voVideo.h"

#if defined(WIN32)
#define inline _inline
#endif

#define USE_WIN32_BROADCAST 1

// #define MAX_INPUT_SIZE	(2*1024*1024)
#define INPUT_SIZE_INC_STEP (128*1024)

typedef enum
{
	FRAME_STATE_READY,
// 	FRAME_STATE_SETTING,
	FRAME_STATE_DECODING,
	FRAME_STATE_OUTPUT
}FrameState;

struct H265_DEC;
struct H265_DEC_SLICE;
struct TComPic;
// struct VP8Common;
typedef struct h265_frameThdInfo 
{
	pthread_t thread_id;
	pthread_mutex_t * stateMutex;
// 	pthread_mutex_t * progressMutex;
	pthread_mutex_t * outputMutex;
	pthread_cond_t * setupCond;
	pthread_cond_t * readyCond;
// 	pthread_cond_t * progressCond;
// 	pthread_cond_t * outputCond;
	volatile VO_S32 runCMD;		//run or stop, controlled by main thread 
	volatile VO_S32 bRunning;	//running state
	volatile VO_S32 errFlag;
	volatile VO_U32 usedBytes;
	volatile FrameState state;

	struct H265_DEC *pDecGlobal;
	struct H265_DEC_SLICE *pSelfSlice;
// 	struct h265_frameThdInfo* pNextThd;
}h265FrameThdInfo;

typedef struct _h265ThreadParam
{
	h265FrameThdInfo *selfInfo;
	struct H265_DEC *pDecGlobal;
}voH265ThreadParam;

typedef struct h265_frameThdPool 
{
	pthread_mutex_t stateMutex[MAX_THREAD_NUM2];
// 	pthread_mutex_t progressMutex;
	pthread_mutex_t outputMutex;
	pthread_cond_t setupCond[MAX_THREAD_NUM2];
	pthread_cond_t readyCond[MAX_THREAD_NUM2];
// 	pthread_cond_t progressCond;
// 	pthread_cond_t outputCond;
	h265FrameThdInfo thdInfo[MAX_THREAD_NUM2];
// 	VO_U8 *pThdPriv[MAX_THREAD_NUM2];			//for allocate all threads' input data memory
	void *pThdContext;		//for allocate all threads' "SelfSlice" memory
}h265FrameThdPool;

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

#if USE_WIN32_BROADCAST && (defined(VOWINCE) || defined(WIN32))

#define WIN32_BROADCAST_OTHERS(cond, nThd)\
do{\
	VO_U32 i;\
	for (i = nThd-1; i > 0; --i)\
	{\
		pthread_cond_signal(cond);\
	}\
}while(0)

#else
#define WIN32_BROADCAST_OTHERS(cond, nThd)

#endif

extern inline VO_S32 WaitForProgress(struct TComPic * const p, const VO_S32 y, const VO_S32 nReady_y, const VO_S32 x);
extern inline VO_VOID ReportReady(const struct H265_DEC_SLICE* const p_slice, struct TComPic * const p, const VO_S32 nReady_y);
extern inline VO_S32 WaitForProgress_mv(struct TComPic * const p, const VO_S32 addr);
extern inline VO_VOID ReportProgress(const struct H265_DEC_SLICE* const p_slice, struct TComPic * const p, const VO_S32 y);
extern inline VO_VOID ReportProgress_mv(const struct H265_DEC_SLICE* const p_slice, struct TComPic * const p, const VO_S32 addr);

VO_S32 DecodeFrameMain(struct H265_DEC *pDecGlobal, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo, VO_S32 newInput);
VO_U32 InitH265Threads(struct H265_DEC *pDecGlobal);
VO_U32 ReleaseH265Threads(struct H265_DEC *pDecGlobal);

#endif

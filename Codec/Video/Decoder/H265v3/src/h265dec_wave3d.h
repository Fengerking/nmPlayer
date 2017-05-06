#ifndef H265_WAVE_3D_H
#define H265_WAVE_3D_H

#include "h265dec_RVThread.h"
#include "voVideo.h"
#include "h265dec_ID.h"

#if defined(WIN32)
#define inline _inline
#endif

#define USE_WIN32_BROADCAST 1

// #define MAX_CPU_NUM 16
#define MAX_DECODING_FRAME_NUM	5
#define INPUT_SIZE_INC_STEP (128*1024)


typedef struct _TaskQ 
{
	volatile VO_U32 size;
// 	pthread_mutex_t mutex;

	VO_U32 head;
	VO_U32 tail;

	//private
	void **TaskPool;
	VO_U32 szPool;
}TaskQ;

// typedef struct _Parser_T
// {
// 	struct H265_DEC_SLICE *pSelfSlice;
// }Parser_T;
typedef struct H265_DEC_SLICE Parser_T;

// typedef struct _TReconTask
// {
// 	struct H265_DEC_SLICE *pSelfSlice;
// 	VO_U32 ctb_addr_rs;
// 
// 
// 
// 	struct _Recon_T *blocked;
// 	struct _Recon_T *next_blocked;
// }TReconTask;
// #define Recon_T TReconTask

typedef struct h265_wave3DThdInfo 
{
	pthread_t thread_id;
	pthread_mutex_t * freeFrameMutex;
	pthread_mutex_t * quaryTaskMutex;
	pthread_mutex_t * outputMutex;
	pthread_cond_t * freeFrameCond;
	pthread_cond_t * quaryTaskCond;
	TaskQ *freeFrameList;
	TaskQ *parserTaskQ;
	TaskQ *reconTaskQ;
	TaskQ *unblockTaskQ;

	VO_U8*  edge_emu_buff0;
	VO_U8*  edge_emu_buff1;
	VO_S16* asm_mc0_buff;
	VO_S16* asm_mc1_buff;


	volatile VO_S32 runCMD;		//run or stop, controlled by main thread 
	volatile VO_S32 bRunning;	//running state
	volatile VO_S32 errFlag;
	volatile VO_U32 usedBytes;
// 	volatile FrameState state;

	struct H265_DEC *pDecGlobal;
	struct H265_DEC_SLICE *pSelfSlice;
	
#if ENABLE_THD_INFO
    VO_U32 mc_sync;
    VO_U32 block_count;
#endif
}h265_wave3DThdInfo;

typedef struct _h265_wave3DThreadParam
{
	h265_wave3DThdInfo *selfInfo;
	struct H265_DEC *pDecGlobal;
}voH265Wave3DThreadParam;

typedef struct h265_wave3DThdPool 
{
	pthread_mutex_t freeFrameMutex;
	pthread_mutex_t quaryTaskMutex;
	pthread_mutex_t outputMutex;
	pthread_cond_t freeFrameCond;
	pthread_cond_t quaryTaskCond;

	TaskQ freeFrameList;
	TaskQ parserTaskQ;
	TaskQ reconTaskQ;
	TaskQ unblockTaskQ;

	h265_wave3DThdInfo thdInfo[MAX_THREAD_NUM2];
 	void *pThdContext;		//for allocate all threads' "SelfSlice" memory
}h265_wave3DThdPool;


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


VO_S32 PushTaskQ(TaskQ *pQ, void *task);

struct TComPic;

extern inline VO_U32 CheckPicFinished(h265_wave3DThdInfo * const selfInfo, const struct TComPic * const p);
struct TReconTask * GetRefLCU_rcTask(const struct H265_DEC_SLICE *const p_slice, /*const struct TComPic * const refPic, */VO_S32 max_y, VO_S32 max_x);
VO_BOOL CheckLCUFinished(struct TComPic * const p, struct TReconTask * const block, struct TReconTask * const reconTask);

VO_S32 DecodeFrameMain_wave3D(struct H265_DEC *pDecGlobal, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo, VO_S32 newInput);

VO_U32 InitH265Threads_wave3D(struct H265_DEC *pDecGlobal);
VO_U32 ReleaseH265Threads_wave3D(struct H265_DEC *pDecGlobal);

#endif

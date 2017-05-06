#ifdef CONFIG_MULTITHREAD
#include "voVP8Thread.h"
#include "voVP8DecFrame.h"
#include "voVP8Memory.h"
#include "voVP8DecFindNearMV.h"

#include "stdio.h"

extern int vp8_decode_mb_row_parser(VP8D_COMP *pbi,
                       VP8_COMMON *pc,
                       int mb_row,
                       MACROBLOCKD *xd,int thread,BOOL_DECODER *bc);
extern int vp8_decode_mb_row_idctmc(VP8D_COMP *pbi,
                       VP8_COMMON *pc,
                       int mb_row,
                       MACROBLOCKD *xd,int thread);
extern int decoder_mode_mvs_mbrow(VP8D_COMP *pbi,int mb_row);
extern int loopfilter_mbrow(VP8D_COMP *pbi, VP8_COMMON *cm,MACROBLOCKD *mbd,int *baseline_filter_level,
					 unsigned char*y_ptr, unsigned char*u_ptr, unsigned char*v_ptr,int mb_row,int thread);

extern void vp8_init_loop_filter(VP8_COMMON *cm);
extern void vp8_frame_init_loop_filter(loop_filter_info *lfi, int frame_type);
extern int vp8_decode_mb_row(VP8D_COMP *pbi,VP8_COMMON *pc,int mb_row,MACROBLOCKD *xd);
#ifdef LOG_ENABLE
//#define ANDROID_LOG 1
#ifdef VOANDROID
#define LOG_FILE "/data/local/log.txt"
#define LOG_TAG "VOPlayer"
#include <utils/Log.h>
#  define __VOLOG(fmt, args...) ({LOGD("->%d: %s(): " fmt, __LINE__, __FUNCTION__, ## args);})
#  define __VOLOGFILE(fmt, args...) ({FILE *fp =fopen(LOG_FILE, "a"); fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, ## args); fclose(fp);})
#elif VOWINXP
#define LOG_FILE "D:/log.txt"
#define LOG_TAG "VOPlayer"
#include "stdio.h"
#define __VOLOG(fmt, ...)  printf("[ %s ]->%d: %s(): "fmt"\n",LOG_TAG,__LINE__,__FUNCTION__,__VA_ARGS__);
#define __VOLOGFILE(fmt, ...) {FILE *fp =fopen(LOG_FILE, "a");fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, __VA_ARGS__); fclose(fp);}
#endif
#else
#define __VOLOGFILE
#endif

THREAD_FUNCTION vp8_thread_decoding_proc(void *p_data)
{
	DECODETHREAD_DATA * threadParam = (DECODETHREAD_DATA*)p_data;
	VP8D_COMP *pbi = (VP8D_COMP*)threadParam->ptr1;

	while(1)
	{
		VP8_COMMON *const pc = & pbi->common;
		MACROBLOCKD *const xd  = & pbi->mb2;
		volatile int *tmp = &pbi->stop_decoder_thread;
		if(*tmp )
			break;	

		if (sem_wait(pbi->h_event_startframe) == 0)
		{
			
			if(!pbi->isloopfilter)
			{
				int mb_row=0;
				int num_part = 1 << pc->multi_token_partition;
				BOOL_DECODER *bc = xd->current_bc;	

				for (mb_row = 1; mb_row < pc->mb_rows; mb_row+=2)
				{		
					volatile int * mby = &pbi->thread_mbrow_parser_ready[0];
					if(*tmp )
						break;

					while( mb_row-1 != *mby )
					{
						if(*tmp )
						break;
					} 

					if(num_part>1)
						bc = &pbi->mbc[mb_row%num_part];

					if(xd->frame_type == INTER_FRAME)
						decoder_mode_mvs_mbrow(pbi,mb_row);
					
					if(vp8_decode_mb_row_parser(pbi, pc, mb_row, xd, 1, bc)!=VPX_CODEC_OK)
					{
						stop_token_decoder(pbi);
						break;
					}					

					if(vp8_decode_mb_row_idctmc(pbi, pc, mb_row, xd, 1)!=VPX_CODEC_OK)
					{
						break;
					}						
				}
			}
			else //loop filter
			{
				int mb_row = 0;
				unsigned char *y_ptr, *u_ptr, *v_ptr;
				int *baseline_filter_level = pbi->baseline_filter_level;
				YV12_BUFFER_CONFIG *post = pc->frame_to_show;
				MACROBLOCKD *const xd  = & pbi->mb;

				if(*tmp )
					break;
				
				for (mb_row = 1; mb_row < pc->mb_rows; mb_row+=2)
				{
					//printf("thread ----row %d \n",mb_row);
					y_ptr = post->y_buffer+mb_row*post->y_stride  * 16;
					u_ptr = post->u_buffer+mb_row*post->uv_stride  * 8;
					v_ptr = post->v_buffer+mb_row*post->uv_stride  * 8;

					loopfilter_mbrow(pbi,pc, xd, baseline_filter_level,
								 y_ptr, u_ptr, v_ptr, mb_row,1);
				}

				pbi->thread_mb_row[1] = pc->mb_rows+1;
			}

			if(pbi->max_threads == 2 )
				sem_post(pbi->h_event_main);
		}
	}

    return 0 ;
}

int voVP8CreateThreads(VP8D_COMP *pbi)
{
	if(pbi->max_threads!=2)
		return 0;

	pbi->stop_decoder_thread = 0;

#ifdef _IOS
	sem_unlink("h_event_main");
	sem_unlink("h_event_startframe");
	pbi->h_event_main = sem_open("h_event_main",O_CREAT|O_EXCL,0644,0);
	pbi->h_event_startframe = sem_open("h_event_startframe",O_CREAT|O_EXCL,0644,0);
#else
    pbi->h_event_main = (sem_t*)MallocMem(pbi->pUserData, pbi->nCodecIdx, sizeof(sem_t), CACHE_LINE);
    sem_init(pbi->h_event_main, 0, 0);
	pbi->h_event_startframe = (sem_t*)MallocMem(pbi->pUserData, pbi->nCodecIdx, sizeof(sem_t), CACHE_LINE);
	sem_init(pbi->h_event_startframe, 0, 0);	
#endif

	pbi->threadParam = (DECODETHREAD_DATA*)MallocMem(pbi->pUserData, pbi->nCodecIdx, sizeof(DECODETHREAD_DATA), CACHE_LINE);
	if(!pbi->threadParam)
		return VO_ERR_OUTOF_MEMORY;

	pbi->threadParam->ithread = 1;
	pbi->threadParam->ptr1 = (void *)pbi;

	pthread_create(&pbi->h_decoding_thread, 0, vp8_thread_decoding_proc, pbi->threadParam);

	return 0;
}

int voVP8DestoryThreads(VP8D_COMP *pbi)
{
	if(pbi->max_threads!=2)
		return 0;
	pbi->stop_decoder_thread = 1;

	sem_post(pbi->h_event_startframe);	

	pthread_join(pbi->h_decoding_thread, 0);

#ifdef _IOS
	sem_close(pbi->h_event_main);
	sem_close(pbi->h_event_startframe);
	sem_unlink("h_event_main");
	sem_unlink("h_event_startframe");
#else
	sem_destroy(pbi->h_event_main);
	sem_destroy(pbi->h_event_startframe);	
	FreeMem(pbi->pUserData, pbi->nCodecIdx, pbi->h_event_main);
	FreeMem(pbi->pUserData, pbi->nCodecIdx, pbi->h_event_startframe);
#endif
	FreeMem(pbi->pUserData, pbi->nCodecIdx, pbi->threadParam);

	return 0;

}

#endif



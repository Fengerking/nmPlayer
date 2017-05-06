/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003-2011			*
*																		*
************************************************************************/
/*******************************************************************************
	File:		voRVThread.h

	Contains:	Thread type define header file

	Written by:	Rock Hu

	Change History (most recent first):
	2011-01-26		HW			Create file

*******************************************************************************/
#undef FALSE
#undef TRUE

#include <string.h>

#include <voRealVideo.h>
#include "decoder.h"
#include "basic.h"
#include "bsx.h"
#include "stdlib.h"

THREAD_FUNCTION thread_decoding_proc(void *p_data)
{
	//I32 ithread = ((DECODETHREAD_DATA *)p_data)->ithread + 1;
	struct Decoder *pbi = (struct Decoder  *)(((DECODETHREAD_DATA *)p_data)->ptr1);
	I32  uMBA;
	U8   *pb;
	U32  pb_size;
	I32  returnCode = 0;
	U32  i;
	SLICE_DEC *slice_di;

	//printf("thread created!\n");

	while(1){
      //  printf("waitting sem\n");

#ifdef THREAD_TEST
        pthread_mutex_lock(&pbi->child_mutex);
		printf("waitting child_cond \n");
		if(pthread_cond_wait(&pbi->child_cond, &pbi->child_mutex)==0){
//			pthread_mutex_unlock(&pbi->child_mutex);
#else
		//printf("waiting slice event \n");
		if (vo_sem_wait(&pbi->m_event_slice_decoding[0]) == 0){
#endif
			//printf("inter child_cond \n");

			if (pbi->m_multithreaded_rd == 0){
				break;
			}
            //printf("after waitting sem\n");
			slice_di = &pbi->m_slice_di[1];
			pb       = pbi->m_pb;
			pb_size  = pbi->m_pb_size;

			slice_di->m_xmb_pos = 0;
			slice_di->m_ymb_pos = 0;

			for(i = pbi->m_slice_main; i < pbi->m_nSlices; i++){
				initbits(slice_di, pb + pbi->m_sliceInfo[i].offset, pb_size);
				slice_di->m_pBitstart = pb;
				slice_di->m_pBitend   = pb + pb_size + 4;
				returnCode =  pbi->GetSliceHeader(pbi, &uMBA, slice_di);

				if(returnCode){//huwei 20110315 stability
					break;
				}

				if(0 == uMBA){//huwei 20110311 bug fixed
					break;
				}

				slice_di->m_resync_mb_x      = uMBA % pbi->m_mbX;
				slice_di->m_resync_mb_y      = uMBA / pbi->m_mbX;

				if((pbi->m_mbX== (slice_di->m_xmb_pos + 1)) && (pbi->m_mbY == (slice_di->m_ymb_pos + 1))){
					break;
				}

				SetMem((U8*)slice_di->m_pAboveSubBlockIntraTypes, -1, pbi->m_mbX << 2, pbi->m_pMemOP);

				slice_di->m_pMBIntraTypes[3]  = -1;
				slice_di->m_pMBIntraTypes[7]  = -1;
				slice_di->m_pMBIntraTypes[11] = -1;
				slice_di->m_pMBIntraTypes[15] = -1;
				Decoder_Slice(pbi, i + 1,1);
			//	printf("decoding slices\n");
			}
		}
#ifdef THREAD_TEST
		printf("child unlock child mutex \n");
		pthread_mutex_unlock(&pbi->child_mutex);

//		printf("signal main_cond \n");
//		pthread_cond_signal(&pbi->main_cond);
#else
		//printf("post main event \n");
		vo_sem_post(&pbi->m_event_main);
#endif

	}

//#if defined(VOANDROID) || defined (_IOS) 
//	pthread_exit("bye");
//#endif
	
	return VO_ERR_NONE;
}

void decoder_remove_threads(struct Decoder *t)
{
	I32 i;

	if (t->m_slice_di){
		FreeMem(t->m_slice_di, t->m_pMemOP);
		t->m_slice_di = NULL ;
	}

	if (t->m_multithreaded_rd){

		t->m_multithreaded_rd = 0;	

		for (i = 0; i < (t->m_decoding_thread_count - 1); i++){
#ifdef THREAD_TEST
	        pthread_cond_signal(&t->child_cond);
#else
			vo_sem_post(&t->m_event_slice_decoding[i]);
#endif	
			pthread_join(t->m_decoding_thread[i], NULL);
		}

		for (i = 0; i < (t->m_decoding_thread_count - 1); i++){
#ifdef THREAD_TEST
	        pthread_cond_destroy(&t->child_cond);
			pthread_mutex_destroy(&t->child_mutex);
	        pthread_cond_destroy(&t->main_cond);
			pthread_mutex_destroy(&t->main_mutex);
#else
			vo_sem_destroy("realvideochild", &t->m_event_slice_decoding[i]);
#endif
		}
#ifndef THREAD_TEST
		vo_sem_destroy("realvideomain", &t->m_event_main);
#endif

		if (t->m_decoding_thread){
			FreeMem(t->m_decoding_thread, t->m_pMemOP);
			t->m_decoding_thread = NULL;
		}
#ifndef THREAD_TEST
		if (t->m_event_slice_decoding){
			FreeMem(t->m_event_slice_decoding, t->m_pMemOP);
			t->m_event_slice_decoding = NULL;
		}
#endif
		if (t->m_thread_data){
			FreeMem(t->m_thread_data, t->m_pMemOP);
			t->m_thread_data = NULL;
		}
	}
}

VO_U32 decoder_create_threads(void *decoder)
{
	struct Decoder *t = (struct Decoder *)decoder;
	I32 core_count = 0;
	I32 ithread;

	t->m_multithreaded_rd = 0;
	core_count = t->m_decoding_thread_count;

	if (core_count > 1){
		t->m_multithreaded_rd     = 1;

		t->m_decoding_thread      = MallocMem(sizeof(pthread_t) * (t->m_decoding_thread_count - 1), CACHE_LINE, t->m_pMemOP);
		if (!t->m_decoding_thread){
			return VO_ERR_OUTOF_MEMORY;
		}
#ifndef THREAD_TEST

		t->m_event_slice_decoding = MallocMem(sizeof(VO_SEM) * (t->m_decoding_thread_count - 1), CACHE_LINE, t->m_pMemOP);
		if (!t->m_event_slice_decoding){
			return VO_ERR_OUTOF_MEMORY;
		}
#endif

		t->m_thread_data          = MallocMem(sizeof(DECODETHREAD_DATA) * (t->m_decoding_thread_count - 1), CACHE_LINE, t->m_pMemOP);
		if (!t->m_thread_data){
			return VO_ERR_OUTOF_MEMORY;
		}

		for (ithread = 0; ithread < (t->m_decoding_thread_count - 1); ithread++){
#ifdef THREAD_TEST
			if(pthread_cond_init(&t->child_cond, NULL)){
				return VO_ERR_FAILED;
			}

			if(pthread_mutex_init(&t->child_mutex, NULL)){
				return VO_ERR_FAILED;
			}

			if(pthread_cond_init(&t->main_cond, NULL)){
				return VO_ERR_FAILED;
			}

			if(pthread_mutex_init(&t->main_mutex, NULL)){
				return VO_ERR_FAILED;
			}
#else
			vo_sem_init("realvideochild", &t->m_event_slice_decoding[ithread], 0, 0);//TBD, only one by now
#endif

			t->m_thread_data[ithread].ithread  = ithread;
			t->m_thread_data[ithread].ptr1     = (void *)t;
#ifdef _IOS
			{
				pthread_attr_t  attr;
			    pthread_attr_init(&attr);
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                pthread_create(&t->m_decoding_thread[ithread], &attr, (void*(*)(void*))thread_decoding_proc, (&t->m_thread_data[ithread]));
			}
#else
			if(pthread_create(&t->m_decoding_thread[ithread], NULL, thread_decoding_proc, (&t->m_thread_data[ithread]))){
                                voVLog("create thread error !\n");
				return VO_ERR_FAILED;
                        }
#endif
			thread_sleep(1);

		}
#ifndef THREAD_TEST
		vo_sem_init("realvideomain", &t->m_event_main, 0, 0);
#endif

	}

	voVLog("Thread created!\n");
	return VO_ERR_NONE;
}


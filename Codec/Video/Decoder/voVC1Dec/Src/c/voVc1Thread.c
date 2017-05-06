//#ifdef MULTI_CORE_DEC
#include "voWmvPort.h"
#include "xplatform_wmv.h"
#include "limits.h"
#include "typedef.h"
#include "wmvdec_member.h"

#ifdef macintosh
#include "cpudetect_mac.h"
#include "motioncomp_altivec.h"
#endif
#include "tables_wmv.h"

#include "strmdec_wmv.h"
#include "motioncomp_wmv.h"
#include "idctdec_wmv2.h"
#include "repeatpad_wmv.h" 

#include "spatialpredictor_wmv.h"
#include "localhuffman_wmv.h"
#include "tables_wmv.h"
#include "wmvdec_function.h"
#include "wmvdec_member.h"

#ifdef VOANDROID
#include <sys/mman.h>
#include <sys/prctl.h>
#endif
extern void UpdateThreadFIFONew(tWMVDecInternalMember *pSrcWMVDec,tWMVDecInternalMember *pDstWMVDec);

THREAD_FUNCTION ThreadDecFrame(void *p_data)
{
#ifdef USE_FRAME_THREAD
	tWMVDecThdCtx* pThCtx = (tWMVDecThdCtx*)p_data;
	tWMVDecInternalMember *pThdDec = pThCtx->pThWMVDec;
	volatile tWMVDecInternalMember *pMainDec = pThCtx->pMainWMVDec;

	while (1)
	{
		if(pThdDec->pThreadStop)
		{
			pThdDec->pThreadStatus = THREAD_FINISHED;
			break;
		}
		if(pThdDec->pThreadStatus == THREAD_ERROR)
			return 0;

		printf("[thread%d]    wait  status=%d\n",pThdDec->nthreads,pThdDec->pThreadStatus); 
		if (sem_wait(pThdDec->pThreadStart )== 0)
		{
			int i=0,j=0;
			tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
			pThdDec->pThreadStatus = THREAD_BUSY;

			printf("[thread%d]    waited   status=%d\n",pThdDec->nthreads,pThdDec->pThreadStatus); 
			
			if(pThdDec->pThreadStop)
			{
				voprintf("[thread%d]  stop \n",pThdDec->nthreads);
				pThdDec->pThreadStatus = THREAD_FINISHED;
				break;
			}

			if((tWMVStatus = WMVVideoDecVOPHeaderNew(pMainDec,pThdDec))!=WMV_Succeeded)
			{
				pThdDec->error_flag_main = 1;
				pThdDec->error_flag_thread = 1;
				pThdDec->frame_errorflag = 1;
				pThdDec->pThreadStatus = THREAD_ERROR;
				pThdDec->m_pfrmCurrQ->m_decodeprocess = MAX_MB_HEIGHT;
				pThdDec->m_pfrmRef0Process->m_decodeprocess = MAX_MB_HEIGHT;
				UpdateThreadFIFONew(pThdDec,(tWMVDecInternalMember*)pMainDec);
				sem_post(pThdDec->pHasHeaderDecoded);
				sem_post(pThdDec->pHasFinishedThread);
				printf("[thread%d]   WMVVideoDecVOPHeaderNew  ---%d error \n",pThdDec->nthreads,pThdDec->m_pfrmCurrQ->m_decodeprocess);
				break;
			}

			GetOutputParamThreadBefore(pThdDec, 
													pThdDec->pOutBuff, 
													pThdDec->stride,
													pThdDec->m_iFrmWidthSrc,
													pThdDec->m_iFrmHeightSrc);	

			if(pThdDec->m_pvopcRenderThis)
			{
				volatile U32_WMV*outstatus = &pMainDec->nOutPutting;
				while(*outstatus==1)
					thread_sleep(0);
				pMainDec->nOutPutting = 1;
				if(pMainDec->output_index > 15 || pMainDec->output_index < 0)
					pMainDec->output_index = 0;
				pMainDec->outputList[pMainDec->output_index++] = pThdDec->m_pvopcRenderThis;	
				pMainDec->nOutPutting = 0;
			}
			
			pMainDec->m_frameNum ++;

#ifdef USE_FRAME_THREAD
			//update  globle
			UpdateThreadFIFONew(pThdDec,(tWMVDecInternalMember*)pMainDec);

			if(pThdDec->bNewheader == 1)
			{
				CopySQHParametes((tWMVDecInternalMember*)pMainDec, pThdDec);
				CopyEntryPointParametes((tWMVDecInternalMember*)pMainDec,pThdDec);
				if(pMainDec->bNewSize)
					InitDataMembers_WMVA ((tWMVDecInternalMember*)pMainDec, pMainDec->m_iFrmWidthSrc, pMainDec->m_iFrmHeightSrc);
			}
#endif			

			//Notify main that cur thread has finish header decode if not interlace or 411
			if (!pThdDec->m_bInterlaceV2 && !pThdDec->m_bYUV411) 
				sem_post(pThdDec->pHasHeaderDecoded);

			if(WMVideoDecVOP (pThdDec)!=WMV_Succeeded)
			{				
				pThdDec->error_flag_main = 1;
				pThdDec->error_flag_thread = 1;
				pThdDec->frame_errorflag = 1;
				pThdDec->pThreadStatus = THREAD_ERROR;
				pThdDec->m_pfrmCurrQ->m_decodeprocess = MAX_MB_HEIGHT;
				pThdDec->m_pfrmRef0Process->m_decodeprocess = MAX_MB_HEIGHT;
				sem_post(pThdDec->pHasHeaderDecoded);
				sem_post(pThdDec->pHasFinishedThread);
				printf("[thread%d]   WMVideoDecVOP  ---%d error \n",pThdDec->nthreads,pThdDec->m_pfrmCurrQ->m_decodeprocess);
				break;
			}

			if(tWMVStatus = GetOutputParamThreadAfter( pThdDec, 
																   pThdDec->pOutBuff, 
																   pThdDec->stride,
																   pThdDec->m_iFrmWidthSrc,
																   pThdDec->m_iFrmHeightSrc))
			{
				printf("[thread%d] after   error \n",pThdDec->nthreads);
				pThdDec->pThreadStatus = THREAD_ERROR;
				break;
			}

			pThdDec->pThreadStatus = THREAD_FINISHED;

			if (pThdDec->m_bInterlaceV2 || pThdDec->m_bYUV411) 
				sem_post(pThdDec->pHasHeaderDecoded);

			sem_post(pThdDec->pHasFinishedThread);
			
		}
	}
#endif
	return 0;
}

I32_WMV voVC1CreateThreads(tWMVDecInternalMember *pWMVDec)
{
#ifdef USE_FRAME_THREAD
    U32_WMV i=0;
    I32_WMV ret=0;

#ifdef _IOS
	const char* sem_names[MAX_THREAD_NUM*3] = {"sem0_ThreadStart","sem0_FinishedThread","sem0_HeaderDecoded",
														"sem1_ThreadStart","sem1_FinishedThread","sem1_HeaderDecoded",
														"sem2_ThreadStart","sem2_FinishedThread","sem2_HeaderDecoded",
														"sem3_ThreadStart","sem3_FinishedThread","sem3_HeaderDecoded"};
#endif
	//pWMVDec->pHasFinishedThread = (sem_t*)wmvMalloc(pWMVDec,sizeof(sem_t),DHEAP_STRUCT);
	//sem_init(pWMVDec->pHasFinishedThread, 0, 0);

	for(i=0;i<pWMVDec->CpuNumber;i++)
	{
		tWMVDecThdCtx* pThdCtx = NULL;
		tWMVDecInternalMember* pThreadWMVDec = NULL;
		pWMVDec->pThreadCtx[i] = wmvMalloc(pWMVDec,sizeof(tWMVDecThdCtx),DHEAP_STRUCT);
		if(pWMVDec->pThreadCtx[i] == NULL)
			return WMV_BadMemory;
		pThdCtx = pWMVDec->pThreadCtx[i];

		pThdCtx->nthreadidx = i;
		pThdCtx->pThWMVDec = wmvMalloc(pWMVDec,sizeof(tWMVDecInternalMember),DHEAP_STRUCT);
		if(pThdCtx->pThWMVDec == NULL)
			return -1;
		pThdCtx->pMainWMVDec = pWMVDec;

		pThreadWMVDec = pThdCtx->pThWMVDec;

		memset(pThreadWMVDec,0,sizeof(tWMVDecInternalMember));
		pThreadWMVDec->nthreads = i;
		pThreadWMVDec->isThreadCtx = 1;
		pThreadWMVDec->pThreadStatus = THREAD_FREE;
		pThreadWMVDec->pThreadStop = 0;

#ifdef _IOS
		sem_unlink(sem_names[i*3]);
		sem_unlink(sem_names[i*3+1]);
		sem_unlink(sem_names[i*3+2]);
		pThreadWMVDec->pThreadStart = sem_open(sem_names[i*3],O_CREAT|O_EXCL,0644,0);
		pThreadWMVDec->pHasFinishedThread = sem_open(sem_names[i*3+1],O_CREAT|O_EXCL,0644,0);
		pThreadWMVDec->pHasHeaderDecoded = sem_open(sem_names[i*3+2],O_CREAT|O_EXCL,0644,0);
#else		
		pThreadWMVDec->pThreadStart = (sem_t*)wmvMalloc(pWMVDec,sizeof(sem_t),DHEAP_STRUCT);
		if(pThreadWMVDec->pThreadStart == NULL)
			return WMV_BadMemory;
		pThreadWMVDec->pHasFinishedThread = (sem_t*)wmvMalloc(pWMVDec,sizeof(sem_t),DHEAP_STRUCT);
		if(pThreadWMVDec->pHasFinishedThread  == NULL)
			return WMV_BadMemory;
		pThreadWMVDec->pHasHeaderDecoded = (sem_t*)wmvMalloc(pWMVDec,sizeof(sem_t),DHEAP_STRUCT);
		if(pThreadWMVDec->pHasHeaderDecoded == NULL)
			return WMV_BadMemory;
        sem_init(pThreadWMVDec->pThreadStart, 0, 0);
		sem_init(pThreadWMVDec->pHasFinishedThread, 0, 0);
		sem_init(pThreadWMVDec->pHasHeaderDecoded, 0, 0);
#endif

		sem_post(pThreadWMVDec->pHasFinishedThread);
		pthread_create(&pWMVDec->h_thread_dec[i], NULL, ThreadDecFrame, pThdCtx);

		pThreadWMVDec->g_StrmModedataCtl.pVideoBuffer = (U8_WMV*)wmvMalloc(pWMVDec,sizeof(U8_WMV)*1024*1024*2,DHEAP_STRUCT);
		if(pThreadWMVDec->g_StrmModedataCtl.pVideoBuffer == NULL)
			return WMV_BadMemory;

#ifdef VOANDROID
        {
			if(i==0)
			{
				char* t = "vc1 th0 ";
				prctl(PR_SET_NAME, (unsigned long)t  , 0 , 0 , 0);
			}
			if(i==1)
			{
				char* t = "vc1 th1 ";
				prctl(PR_SET_NAME, (unsigned long)t  , 0 , 0 , 0);
			}
			if(i==2)
			{
				char* t = "vc1 th2 ";
				prctl(PR_SET_NAME, (unsigned long)t  , 0 , 0 , 0);
			}
			if(i==3)
			{
				char* t = "vc1 th3 ";
				prctl(PR_SET_NAME, (unsigned long)t  , 0 , 0 , 0);
			}
        }
#endif
	}
#endif
	return 0;
}

void voVC1DestoryThreads(tWMVDecInternalMember *pWMVDec)
{   
#ifdef USE_FRAME_THREAD
    U32_WMV i;

    for(i=0;i<pWMVDec->CpuNumber;i++)
    {
		tWMVDecThdCtx* pThCtx = (tWMVDecThdCtx*)pWMVDec->pThreadCtx[i];
		tWMVDecInternalMember *pThdWMVDec = pThCtx->pThWMVDec;
		
		//if(!pWMVDec->error_flag_main)
		{
			volatile U32_WMV*status = &pThdWMVDec->pThreadStatus;
			while(*status == THREAD_BUSY)
				printf(".");
		}
#if	1
		pThdWMVDec->pThreadStop = 1;
		sem_post(pThdWMVDec->pThreadStart);
		thread_sleep(1);
        pthread_join(pWMVDec->h_thread_dec[i], 0);	
#endif

#ifdef _IOS
		sem_close(pThdWMVDec->pThreadStart);
		sem_close(pThdWMVDec->pHasFinishedThread);
		sem_close(pThdWMVDec->pHasHeaderDecoded);
		sem_unlink("sem_names[i*3]");
		sem_unlink("sem_names[i*3+1]");
		sem_unlink("sem_names[i*3+1]");
#else
		sem_destroy(pThdWMVDec->pThreadStart);
		sem_destroy(pThdWMVDec->pHasFinishedThread);	
		sem_destroy(pThdWMVDec->pHasHeaderDecoded);		
		FREE_PTR(pWMVDec,pThdWMVDec->pThreadStart);
		FREE_PTR(pWMVDec,pThdWMVDec->pHasFinishedThread);	
		FREE_PTR(pWMVDec,pThdWMVDec->pHasHeaderDecoded);	
#endif
		FREE_PTR(pWMVDec,pThdWMVDec->g_StrmModedataCtl.pVideoBuffer);	

		WMVInternalMemberDestroy(pThdWMVDec);
		FREE_PTR(pWMVDec,pThCtx->pThWMVDec);
		FREE_PTR(pWMVDec,pWMVDec->pThreadCtx[i]);		
		
    }
#endif
}
//#endif
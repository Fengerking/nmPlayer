
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/

/*!
************************************************************************
*
* \file		neighbor.c
*
* \brief
*		defines store neighboring infomation encoding/decoding
*
************************************************************************
*/
#if !BUILD_WITHOUT_C_LIB
#include <assert.h>
//#include <memory.h>
#endif//BUILD_WITHOUT_C_LIB
#include "global.h"
#include "nalu.h"




//#include "H264_C_Type.h"
#include "avd_neighbor.h"

#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
#include <sys/types.h>
#endif

#if defined(USE_JOBPOOL)
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
void VOH264CreateEvent(TMEvent *ev)
{
    pthread_mutex_init(&ev->mutex, 0);
    pthread_cond_init(&ev->cond, 0);
    ev->triggered = 0;
}
void VOH264SetEvent(TMEvent *ev)
{
    pthread_mutex_lock(&ev->mutex);
    ev->triggered = 1;
#if 1
    pthread_cond_signal(&ev->cond);
#else
    pthread_cond_broadcast(&ev->cond);
#endif
    pthread_mutex_unlock(&ev->mutex);
}
void VOH264ResetEvent(TMEvent *ev)
{
    pthread_mutex_lock(&ev->mutex);
    ev->triggered = 0;
    pthread_mutex_unlock(&ev->mutex);
}
void VOH264WaitForEvent(TMEvent *ev)
{
    pthread_mutex_lock(&ev->mutex);
    while (!ev->triggered)
        pthread_cond_wait(&ev->cond, &ev->mutex);
    pthread_mutex_unlock(&ev->mutex);
}

void VOH264DestroyEvent(TMEvent *ev)
{
    ev->triggered = 0;
    pthread_cond_destroy(&ev->cond);
    pthread_mutex_destroy(&ev->mutex);
}
#endif
#endif

#ifdef USE_JOBPOOL

void    InitJobPool(ImageParameters *img)
{
    if( img->vdLibPar->multiCoreNum > 1)
    {
        img->mbsProcessor =  voH264AlignedMalloc(img, 300, sizeof(avdNativeInt) * (img->sizeInfo->FrameHeightInMbs + SLICELIST_NUM));
        img->job_cnt = 0;
        img->frame_job_pos = 0;
        img->total_job = 0;
        img->is_cores_running = 0;
	 img->slices_flag = 0;
#if defined(WIN32) || defined(WINCE)
 //       img->JobEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
        img->EndOfFrameEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
        img->JobMutex = CreateMutex(NULL, FALSE, NULL);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
//        CreateEvent(&img->JobEvent);
        pthread_mutex_init(&img->JobMutex, 0);
        VOH264CreateEvent(&img->EndOfFrameEvent);
#endif
    }
    else
        img->mbsProcessor =  voH264AlignedMalloc(img, 300, sizeof(avdNativeInt) * 4);
}

void    ExitJobPool(ImageParameters *img)
{
    voH264AlignedFree( img, img->mbsProcessor);
    if( img->vdLibPar->multiCoreNum > 1)
    {
#if defined(WIN32) || defined(WINCE)
        CloseHandle(img->EndOfFrameEvent);
//        CloseHandle(img->JobEvent);
        CloseHandle(img->JobMutex);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
        VOH264DestroyEvent(&img->EndOfFrameEvent);
//        VOH264DestroyEvent(&img->JobEvent);
#endif
    }
}

static void    PrintJob(avdInt8 *s, ImageParameters *img, TMCoreThreadParam *threadParam, avdInt32 job)
{
    int i = 0, coreID = -1;
    if(threadParam != NULL)
    {
        for(i = 0; i < MAX_CORE_NUM; i++)
        {
            if( img->mcoreThreadParam[i] == threadParam)
            {
                coreID = i;
            }

        }
    }
    else
        coreID = 0;
#if  defined(ANDROID)
    AvdLog2(LL_ERROR, "%s  poc %d Core ID %d , thread pid %d is running on %d job all job %d\n", s, img->pocInfo->framepoc, coreID,   gettid(), job, img->job_cnt);
#else
    AvdLog2(LL_INFO, "%s  Core ID %d , is running on %d job all job %d job state %d\n", s, coreID,  job, img->job_cnt, img->mbsProcessor[job]->job_state);
#endif
}

void     AddAJobToPool(ImageParameters *img, avdNativeInt end_of_slice, avdNativeInt first_of_slice)
{

#if defined(WIN32) || defined(WINCE)
    WaitForSingleObject(img->JobMutex, INFINITE);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
    pthread_mutex_lock(&img->JobMutex);
#endif
    PrintJob("AddAJobToPool", img, NULL, img->job_cnt);
    if(img->error >= 0)
    {

        if(first_of_slice)
        {
            img->slices_flag = (img->slices_flag)  | (1 << (img->current_slice_nr - 1));
            img->mbsProcessor[img->job_cnt]->isFirstOfSlice = 1;
            img->min_slice_jobs[img->current_slice_nr-1] = img->job_cnt;
            img->max_slice_jobs[img->current_slice_nr-1] = img->job_cnt;
	     if(img->current_slice_nr == 1)
	     {
	         img->is_new_pic = 1;
		  img->need_clr_core = img->vdLibPar->multiCoreNum - 1;
	     }
        }
        else
        {
            img->max_slice_jobs[img->current_slice_nr-1] = img->job_cnt;
            img->mbsProcessor[img->job_cnt]->isFirstOfSlice = 0;
        }

        img->mbsProcessor[img->job_cnt]->current_slice_nr = img->current_slice_nr;
		img->mbsProcessor[img->job_cnt]->curr_job_pos = img->job_cnt;
        img->mbsProcessor[img->job_cnt++]->job_state = EMP_JOB_DATA_READY;
		if(img->job_cnt >= img->slice_max_jobs && !img->end_of_frame)
		{
		    img->slice_max_jobs = img->job_cnt;
			set_1bit(img, img->slice_max_jobs);
		}
    }
    else
    {
        int i;
        for(i = img->frame_job_pos; i < img->job_cnt; i++)
            img->mbsProcessor[i]->job_state = EMP_JOB_ERROR;
    }
#if defined(WIN32) || defined(WINCE)
    if(img->error >= 0)
    {
        int i =0;
	 for(i = 1; i < img->vdLibPar->multiCoreNum; i++)
            SetEvent(img->mcoreThreadParam[i]->JobEvent);
    }
    ReleaseMutex(img->JobMutex);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
    if(img->error >= 0)
    {
        int i =0;
	 for(i = 1; i < img->vdLibPar->multiCoreNum; i++)
            VOH264SetEvent(&img->mcoreThreadParam[i]->JobEvent);
    }
    pthread_mutex_unlock(&img->JobMutex);
#endif
}

void restart_cores(ImageParameters *img)
{
	  if( img->vdLibPar->multiCoreNum > 1)
		{
			if(img->is_cores_running)
			{
			      int i = 0;
#if defined(WIN32) || defined(WINCE)
                            AvdLog2(LL_INFO, "thread 0 restart_cores for a end of frame event\n");
				WaitForSingleObject(img->JobMutex, INFINITE); 
				ResetEvent(img->EndOfFrameEvent);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
				pthread_mutex_lock(&img->JobMutex);
                            VOH264ResetEvent(&img->EndOfFrameEvent);
#endif
				 img->end_of_core = 1;
				 img->need_restart_of_frame = 1;				 
	
#if defined(WIN32) || defined(WINCE)
                            for(i = 1; i < img->vdLibPar->multiCoreNum; i++)
				    SetEvent(img->mcoreThreadParam[i]->JobEvent);
				ReleaseMutex(img->JobMutex);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
                            for(i = 1; i < img->vdLibPar->multiCoreNum; i++)
				    VOH264SetEvent(&img->mcoreThreadParam[i]->JobEvent);
				    pthread_mutex_unlock(&img->JobMutex);
#endif
	
#if defined(WIN32) || defined(WINCE)
				AvdLog2(LL_INFO, "thread 0 for a end of frame event\n");
				WaitForSingleObject(img->EndOfFrameEvent, INFINITE);
				ResetEvent(img->EndOfFrameEvent);
			   AvdLog2(LL_INFO, "thread 0 receive a end of frame event\n");
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
				VOH264WaitForEvent(&img->EndOfFrameEvent);
				VOH264ResetEvent(&img->EndOfFrameEvent);
#endif
	
				 }
		}
    
}

static void print_data(ImageParameters *img)
{
    int i;
	for(i =0; i < MAX_SLICE_NUM; i++)
		AvdLog2(LL_INFO, "job %d curr %d min %d max %d\n", i, img->curr_slice_jobs[i], img->min_slice_jobs[i], img->max_slice_jobs[i]);
}


static  int GetFirst1Bit(avdUInt32 v)
{
    int i = 0;
    for(i = 0; i < MAX_SLICE_NUM; i++)
    {
        if((v >> i) & 1)
            break;
    }

    if(i == MAX_SLICE_NUM)
        return -1;
    else
        return i;

}


static TMBsProcessor *GetAJobFromPool(ImageParameters *img, TMCoreThreadParam *threadParam)
{
    TMBsProcessor *curr_job = NULL;
    avdUInt32 slices_flag = img->slices_flag;

    if(img->error < 0  || img->need_restart_of_frame)
    {
        return curr_job;
    }

    do
    {

        if(threadParam != NULL)
        {
            if(img->is_new_pic)
            {
                threadParam->got_a_slice = 0;
                img->need_clr_core--;
                if(!img->need_clr_core)
                    img->is_new_pic = 0;
            }
            if(!threadParam->got_a_slice)
            {
#if DECODE_BY_ROW
                if(0)
#else
                if(img->slices_flag)
#endif					
                {
                    int slice_nr = GetFirst1Bit(img->slices_flag);
                    int job_pos = img->min_slice_jobs[slice_nr];
                    curr_job = img->mbsProcessor[job_pos];
                    if(curr_job->job_state == EMP_JOB_DATA_READY)
                    {
                        threadParam->slice_job_pos = job_pos + 1;
						img->curr_slice_jobs[slice_nr] = job_pos;
                        threadParam->slice_pos = slice_nr;
                        img->slices_flag &= ~(1 << (curr_job->current_slice_nr - 1));
                        threadParam->got_a_slice = 1;
                        curr_job->job_state = EMP_JOB_DECODING;
#ifdef WIN32													
                        PrintJob("GetAJobFromPool0", img, threadParam, job_pos);
#endif
                    }
                }
                else
                {
                    int i = 0;
                    for(i = img->frame_job_pos; i < img->job_cnt; i++)
                    {
                        curr_job = img->mbsProcessor[i];
                        if(curr_job->job_state <= EMP_JOB_DATA_READY)
                        {
                            if(curr_job->job_state == EMP_JOB_DATA_READY)
                            {
                                curr_job->job_state = EMP_JOB_DECODING;
#ifdef WIN32															
                                PrintJob("GetAJobFromPool1", img, threadParam, i);
#endif
                                img->frame_job_pos = i;
                            }
                            else
                                curr_job = NULL;
                            break;
                        }
                    }


                    if(i >= img->job_cnt)
                        curr_job = NULL;

                    if(curr_job)
                    {
                        if(img->frame_job_pos == img->min_slice_jobs[curr_job->current_slice_nr-1])
                            img->slices_flag &= ~(1 << (curr_job->current_slice_nr - 1));

                    }
                }
                break;
            }
            else
            {
                int i = 0;
                if(img->min_slice_jobs[threadParam->slice_pos] < 0)
                {
                    threadParam->got_a_slice = 0;
                    curr_job = NULL;
                    break;
                }
                else
                {
                    for(i = img->curr_slice_jobs[threadParam->slice_pos]; i <= img->max_slice_jobs[threadParam->slice_pos]; i++)
                    {
                        curr_job = img->mbsProcessor[i];

                        if(curr_job->end_of_slice)
                            threadParam->got_a_slice = 0;

                        if(curr_job->job_state <= EMP_JOB_DATA_READY)
                        {


                            if(curr_job->job_state == EMP_JOB_DATA_READY)
                            {
                                curr_job->job_state = EMP_JOB_DECODING;
#ifdef WIN32															
                                PrintJob("GetAJobFromPool2", img, threadParam, i);
#endif
                            }
                            else
                                curr_job = NULL;
                            break;
                        }
                    }
					
                    img->curr_slice_jobs[threadParam->slice_pos] = i;
					
                    if(i > (img->max_slice_jobs[threadParam->slice_pos]) && !threadParam->got_a_slice)
                        curr_job = NULL;
                    else
                    {
                        if(i > img->max_slice_jobs[threadParam->slice_pos])
                            curr_job = NULL;
                        break;
                    }
                }

            }
        }
        else
        {

            if(!img->main_got_a_slice)
            {
#if DECODE_BY_ROW
				if(0)
#else
				if(img->slices_flag)
#endif					            
                {
                    int slice_nr = GetFirst1Bit(img->slices_flag);
                    int job_pos = img->min_slice_jobs[slice_nr];
                    curr_job = img->mbsProcessor[job_pos];
                    if(curr_job->job_state == EMP_JOB_DATA_READY)
                    {
                        img->main_slice_job_pos = job_pos;
						img->curr_slice_jobs[slice_nr] = job_pos;						
                        img->main_slice_pos = slice_nr;
                        img->slices_flag &= ~(1 << (curr_job->current_slice_nr - 1));
                        img->main_got_a_slice = 1;
                        curr_job->job_state = EMP_JOB_DECODING;
#ifdef WIN32													
                        PrintJob("GetAJobFromPool3", img, NULL, job_pos);
#endif
                    }
                }
                else
                {
                    int i = 0;
                    for(i = img->frame_job_pos; i < img->job_cnt; i++)
                    {
                        curr_job = img->mbsProcessor[i];
                        if(curr_job->job_state <= EMP_JOB_DATA_READY)
                        {
                            if(curr_job->job_state == EMP_JOB_DATA_READY)
                            {
                                curr_job->job_state = EMP_JOB_DECODING;
#ifdef WIN32															
                                PrintJob("GetAJobFromPool4", img, NULL, i);
#endif
                                img->frame_job_pos = i;
                            }
                            else
                                curr_job = NULL;
                            break;
                        }
                    }


                    if(i >= img->job_cnt)
                        curr_job = NULL;

                    if(curr_job)
                    {
                        if(img->frame_job_pos == img->min_slice_jobs[curr_job->current_slice_nr-1])
                            img->slices_flag &= ~(1 << (curr_job->current_slice_nr - 1));

                    }

                }
                break;
            }
            else
            {
                int i = 0;
                for(i = img->curr_slice_jobs[img->main_slice_pos]; i <= img->max_slice_jobs[img->main_slice_pos]; i++)
                {
                    curr_job = img->mbsProcessor[i];
                    if(curr_job->end_of_slice)
                        img->main_got_a_slice = 0;

                    if(curr_job->job_state <= EMP_JOB_DATA_READY)
                    {
                        if(curr_job->job_state == EMP_JOB_DATA_READY)
                        {
                            curr_job->job_state = EMP_JOB_DECODING;
#ifdef WIN32							
                            PrintJob("GetAJobFromPool5", img, NULL, i);
#endif
                        }
                        else
                            curr_job = NULL;
                        break;
                    }
                }

				img->curr_slice_jobs[img->main_slice_pos] = i;

                if(i > img->max_slice_jobs[img->main_slice_pos] && !img->main_got_a_slice)
                    curr_job = NULL;
                else
                {
                    if(i > img->max_slice_jobs[img->main_slice_pos])
                        curr_job = NULL;
                    break;
                }
            }
        }
    }
    while(img->error >= 0);

    if(curr_job)
    {
        if(threadParam)
            threadParam->job_cnt++;
        else
            img->main_job_cnt++;
        img->total_job++;
        //	     AvdLog2(LL_ERROR, "(mby, core ID)   ( %d , %d), job state %d\n", curr_job->mb_y, threadParam ? 1:0, curr_job->job_state);
        if(curr_job->job_state >= EMP_JOB_FINISHED)
        {
            AvdLog2(LL_ERROR,"frame %d slices_flag %d img error %d(got slice %d mby, core ID)   ( %d , %d), job state %d img->job_cnt %d\n",
                   img->ioInfo->outNumber,  slices_flag, img->error,
                   threadParam ? threadParam->got_a_slice : img->main_got_a_slice,
                   curr_job->mb_y, threadParam ? 1 : 0, curr_job->job_state, img->job_cnt);
        }
    }
	else
	{
	   print_data(img);
	}

#if defined(WIN32) || defined(WINCE)
    //    if(threadParam && (curr_job == NULL))
    //       ResetEvent(threadParam->JobEvent);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
    //    if(threadParam && (curr_job == NULL))
    //        img->JobEvent.triggered = 0;
#endif

    return curr_job;

}



TMBsProcessor *WaitForJobFromPool(ImageParameters *img, TMCoreThreadParam *threadParam)
{
    TMBsProcessor *curr_job = NULL;

    if(threadParam != NULL)
    {
        if(((curr_job = GetAJobFromPool(img, threadParam)) == NULL) || (curr_job->job_state ==EMP_JOB_ERROR) ||(img->error <0))
        {
#if CALC_FPS
            threadParam->WaitDataTimes++;
#endif

            if(img->error < 0 && img->end_of_frame)
            {
                img->err_end_of_core++;
#if defined(WIN32) || defined(WINCE)
                if(img->err_end_of_core == img->vdLibPar->multiCoreNum)
                    AvdLog2(LL_INFO, "Thread ID %p send EndOfSliceEvent\n",  GetCurrentThreadId());
                   SetEvent(img->EndOfFrameEvent);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
                if(img->err_end_of_core == img->vdLibPar->multiCoreNum)
                    AvdLog2(LL_INFO, "Thread ID %d send EndOfSliceEvent\n",  threadParam->coreID);

                VOH264SetEvent(&(img->EndOfFrameEvent));
#endif
            }

            if(img->need_restart_of_frame)
            {
                img->end_of_core++;  
				threadParam->got_a_slice = 0;
				if(img->end_of_core == img->vdLibPar->multiCoreNum)
				{
#if defined(WIN32) || defined(WINCE)
					 AvdLog2(LL_INFO, "need_restart Thread ID %p send EndOfSliceEvent\n",  GetCurrentThreadId());
					SetEvent(img->EndOfFrameEvent);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
					VOH264SetEvent(&(img->EndOfFrameEvent));
#endif				
				}
            }

            if(!img->need_restart_of_frame && (img->error >= 0) && img->last_job_flag[threadParam->coreID]) //working thread is reading/decoding
            {
#if defined(WIN32) || defined(WINCE)
                AvdLog2(LL_INFO, "Thread ID %p send EndOfSliceEvent\n",  GetCurrentThreadId());
                SetEvent(img->EndOfFrameEvent);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
                VOH264SetEvent(&(img->EndOfFrameEvent));
#endif
            }

          if(threadParam->status == ETSDB_STOP)
          {
#if defined(WIN32) || defined(WINCE)          
               ReleaseMutex(img->JobMutex);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
              pthread_mutex_unlock(&img->JobMutex);
#endif
		 return NULL;
          }

#if defined(WIN32) || defined(WINCE)
            ResetEvent(threadParam->JobEvent);
            AvdLog2(LL_INFO, "thread 1 for a receive event\n");
            ReleaseMutex(img->JobMutex);
            WaitForSingleObject(threadParam->JobEvent, INFINITE);
            ResetEvent(threadParam->JobEvent);
            AvdLog2(LL_INFO, "thread 1  receive a job event\n");
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
            //            printf("thread 1 for a receive event\n");
            VOH264ResetEvent(&threadParam->JobEvent);
            pthread_mutex_unlock(&img->JobMutex);
            VOH264WaitForEvent(&threadParam->JobEvent);
            VOH264ResetEvent(&threadParam->JobEvent);
            //		printf( "thread 1  receive a job event\n");
#endif
            return NULL;
        }

    }
    else
    {
        if((curr_job = GetAJobFromPool(img, threadParam)) == NULL)
        {
#if defined(WIN32) || defined(WINCE)
            AvdLog2(LL_INFO, "thread 0 for a end of frame event\n");
//            ResetEvent(img->EndOfFrameEvent);
            ReleaseMutex(img->JobMutex);
            WaitForSingleObject(img->EndOfFrameEvent, INFINITE);
            ResetEvent(img->EndOfFrameEvent);
           AvdLog2(LL_INFO, "thread 0 receive a end of frame event\n");
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
//            VOH264ResetEvent(&img->EndOfFrameEvent);
            pthread_mutex_unlock(&img->JobMutex);
            VOH264WaitForEvent(&img->EndOfFrameEvent);
            VOH264ResetEvent(&img->EndOfFrameEvent);
#endif
            return NULL;
        }

    }

    return curr_job;
}

#endif

#ifdef USE_JOBPOOL
int Alloc_Neighbor_Info(ImageParameters *img)
{
    TMBsProcessor *info ;// = img->mbsProcessor;
    MacroBlock	**neighborMBs;
    TSizeInfo			*sizeInfo = img->sizeInfo;
    avdNativeInt		mbColumnCount = (sizeInfo->width >> 4);
    avdNativeInt		i, k;
    avdNativeInt		coreNum 		= img->vdLibPar->multiCoreNum;
    avdNativeInt		rowNumOfMBs;
    avdNativeInt		size = 2;
    avdNativeInt          maxJobs = GetMbAffFrameFlag(img) ? ((sizeInfo->FrameHeightInMbs / 2   +  (sizeInfo->FrameHeightInMbs & 1)) + SLICELIST_NUM / 2) : (sizeInfo->FrameHeightInMbs + SLICELIST_NUM) ;
    avdNativeInt		newMemId = 3000;
    int					mbsProcessorNum = coreNum > 1 ?  max(2, maxJobs) : max(2, coreNum) ; //at least we need 2 mbsProcessor
    int					mbsProcessorNum2 = mbsProcessorNum - 1 ;
    H264VdLibParam *params = img->vdLibPar;
    int			isPossibleMBAff = img->active_sps->mb_adaptive_frame_field_flag;

    img->max_jobs = maxJobs;

    //TMBBitStream*	mbBitStream;
    //seq_parameter_set_rbsp_t* sps = img->active_sps;
    k = 0;

    if(coreNum >= 2 || img->active_sps->frame_mbs_only_flag == 0) //dual core or interlace/mbaff, use BY_MB
    {
        img->deblockType	= DEBLOCK_BY_MB;

    }
    else
    {
        img->deblockType	= DEBLOCK_BY_ROW;

    }
    rowNumOfMBs = mbColumnCount;

    do
    {
        info = img->mbsProcessor[k++] = (TMBsProcessor *)voH264AlignedMalloc(img, newMemId++, sizeof(TMBsProcessor));
        info->neighborMBsActivate = info->neighborMBs = (MacroBlock *) voH264AlignedMalloc(img, newMemId++, rowNumOfMBs * sizeof(MacroBlock));
#if FEATURE_INTERLACE
        //info->neighborMBsActivate = info->neighborMBs;
        if(isPossibleMBAff)//GetMbAffFrameFlag(img))
        {
            info->neighborMBsBottom = (MacroBlock *) voH264AlignedMalloc(img, newMemId++, rowNumOfMBs * sizeof(MacroBlock));
        }
#endif//FEATURE_INTERLACE
        {
            AVDIPDeblocker *ipDeblocker;
            int sizeTmp, mbW = mbColumnCount, sizeY, sizeUV;

            int width = sizeInfo->width;

            int isInterlace = img->active_sps->frame_mbs_only_flag == 0;
            //info->mbsParser->rowCof = (avdInt16 *)voH264AlignedMalloc(img,29, 24 * 16 * mbW * sizeof(avdInt16));CHECK_ERR_RET_INT
            ipDeblocker = GetIPDeblocker(info) = (AVDIPDeblocker *)voH264AlignedMalloc(img, 330, sizeof(AVDIPDeblocker));
            //sizeTmp = (sizeInfo->width<<1);
            sizeY	= (width + isInterlace * width) * (isInterlace + 1);
            sizeUV  = sizeY;
            ipDeblocker->topY = (avdUInt8 *) voH264AlignedMalloc(img, 25, (sizeY + sizeUV) * sizeof(avdUInt8));
            ipDeblocker->topUV[0] = ipDeblocker->topY + sizeY;
            ipDeblocker->topUV[1] = ipDeblocker->topUV[0] + (sizeUV >> 1);
            //force the numRuns 8 bytes aligned

        }
        if(k == 1 || coreNum >= 2)
        {
            avdUInt8 *tmp;
            int advance = 0;
            info->mbsParser = (TMBsParser *) voH264AlignedMalloc(img, newMemId++, sizeof(TMBsParser));
            tmp = (avdUInt8 *)info->mbsParser->m7[16] + 8;
            advance = 8 - ((int)(tmp + 8) & 7);
            if(advance != 8)
                tmp += advance;
            info->mbsParser->numRuns = tmp;

            tmp = (avdUInt8 *)info->mbsParser->m7[MPR_ROW];
            advance = 16 - ((int)(tmp) & 15);
            if(advance != 16)
                tmp += advance;
            info->mbsParser->mpr = tmp;

        }
        else//single core
        {
            info->mbsParser = img->mbsProcessor[0]->mbsParser;
        }
    }
    while(--mbsProcessorNum > 0);

    //single-direction loop list
    for (k = 0; k < mbsProcessorNum2; k++)
    {
        img->mbsProcessor[k+1]->anotherProcessor = img->mbsProcessor[k];
    }
    img->mbsProcessor[0]->anotherProcessor = img->mbsProcessor[k];

    return AVC_TRUE;
}

int Free_Neighbor_Info(ImageParameters *img)
{
    TMBsProcessor *info ;
    avdNativeInt i, j, size, k = 0;

    avdNativeInt		coreNum 		= img->vdLibPar->multiCoreNum;
    avdNativeInt		mbsProcessorNum = coreNum > 1 ?  max(2, img->max_jobs) : max(2, coreNum); //at least we need 2 mbsProcessor
    avdNativeInt		mbsProcessorNum2 = mbsProcessorNum;
    seq_parameter_set_rbsp_t		*active_sps = img->active_sps;
    int			isPossibleMBAff = active_sps && active_sps->mb_adaptive_frame_field_flag;
    AVDIPDeblocker *ipDeblocker;

    if(img->is_cores_running)
    {
        for( k = 1; k < coreNum; k++)
            if(img->mcoreThreadParam[k])
                img->mcoreThreadParam[k]->status = ETSDB_STOP;

        for( k = 1; k < coreNum; k++)
            ExitDCoreThread(img, k);
    }
	
    k = 0;

    if(img->mbsProcessor)
    {
        do
        {

            info = img->mbsProcessor[k++];
            if (!info || !info->neighborMBs)
                continue;
            if(k == 1 || coreNum >= 2)
            {

                SafevoH264AlignedFree(img, info->mbsParser);
            }
            ipDeblocker = GetIPDeblocker(info);
            SafevoH264AlignedFree(img, ipDeblocker->topY);
            SafevoH264AlignedFree(img, ipDeblocker);
            SafevoH264AlignedFree(img, info->neighborMBs);
#if FEATURE_INTERLACE
            if(isPossibleMBAff)
               SafevoH264AlignedFree(img, info->neighborMBsBottom);
#endif//FEATURE_INTERLACE

        }
        while(--mbsProcessorNum > 0);
        for (k = 0; k < mbsProcessorNum2; k++)
        {
            SafevoH264AlignedFree(img, img->mbsProcessor[k]);
        }
    }

    return AVC_TRUE;
}

#else
int Alloc_Neighbor_Info(ImageParameters *img)
{
    TMBsProcessor *info ;// = img->mbsProcessor;
    MacroBlock	**neighborMBs;
    TSizeInfo			*sizeInfo = img->sizeInfo;
    avdNativeInt		mbColumnCount = (sizeInfo->width >> 4);
    avdNativeInt		i, k;
    avdNativeInt		coreNum 		= img->vdLibPar->multiCoreNum;
    avdNativeInt		rowNumOfMBs;
    avdNativeInt		size = 2;
    avdNativeInt		newMemId = 3000;
    int					mbsProcessorNum = max(2, coreNum); //at least we need 2 mbsProcessor
    int					mbsProcessorNum2 = mbsProcessorNum - 1 ;
    H264VdLibParam *params = img->vdLibPar;
    int			isPossibleMBAff = img->active_sps->mb_adaptive_frame_field_flag;

    //TMBBitStream*	mbBitStream;
    //seq_parameter_set_rbsp_t* sps = img->active_sps;
    k = 0;

    if(coreNum >= 2 || img->active_sps->frame_mbs_only_flag == 0) //dual core or interlace/mbaff, use BY_MB
    {
        img->deblockType	= DEBLOCK_BY_MB;

    }
    else
    {
        img->deblockType	= DEBLOCK_BY_ROW;

    }
    rowNumOfMBs = mbColumnCount;

    do
    {
        info = img->mbsProcessor[k++] = (TMBsProcessor *)voH264AlignedMalloc(img, newMemId++, sizeof(TMBsProcessor));
        info->neighborMBsActivate = info->neighborMBs = (MacroBlock *) voH264AlignedMalloc(img, newMemId++, rowNumOfMBs * sizeof(MacroBlock));
#if FEATURE_INTERLACE
        //info->neighborMBsActivate = info->neighborMBs;
        if(isPossibleMBAff)//GetMbAffFrameFlag(img))
        {
            info->neighborMBsBottom = (MacroBlock *) voH264AlignedMalloc(img, newMemId++, rowNumOfMBs * sizeof(MacroBlock));
        }
#endif//FEATURE_INTERLACE
        {
            AVDIPDeblocker *ipDeblocker;
            int sizeTmp, mbW = mbColumnCount, sizeY, sizeUV;

            int width = sizeInfo->width;

            int isInterlace = img->active_sps->frame_mbs_only_flag == 0;
            //info->mbsParser->rowCof = (avdInt16 *)voH264AlignedMalloc(img,29, 24 * 16 * mbW * sizeof(avdInt16));CHECK_ERR_RET_INT
            ipDeblocker = GetIPDeblocker(info) = (AVDIPDeblocker *)voH264AlignedMalloc(img, 330, sizeof(AVDIPDeblocker));
            //sizeTmp = (sizeInfo->width<<1);
            sizeY	= (width + isInterlace * width) * (isInterlace + 1);
            sizeUV  = sizeY;
            ipDeblocker->topY = (avdUInt8 *) voH264AlignedMalloc(img, 25, (sizeY + sizeUV) * sizeof(avdUInt8));
            ipDeblocker->topUV[0] = ipDeblocker->topY + sizeY;
            ipDeblocker->topUV[1] = ipDeblocker->topUV[0] + (sizeUV >> 1);
            //force the numRuns 8 bytes aligned

        }
        if(k == 1 || coreNum >= 2)
        {
            avdUInt8 *tmp;
            int advance = 0;
            info->mbsParser = (TMBsParser *) voH264AlignedMalloc(img, newMemId++, sizeof(TMBsParser));
            tmp = (avdUInt8 *)info->mbsParser->m7[16] + 8;
            advance = 8 - ((int)(tmp + 8) & 7);
            if(advance != 8)
                tmp += advance;
            info->mbsParser->numRuns = tmp;

            tmp = (avdUInt8 *)info->mbsParser->m7[MPR_ROW];
            advance = 16 - ((int)(tmp) & 15);
            if(advance != 16)
                tmp += advance;
            info->mbsParser->mpr = tmp;

        }
        else//single core
        {
            info->mbsParser = img->mbsProcessor[0]->mbsParser;
        }
    }
    while(--mbsProcessorNum > 0);

    //single-direction loop list
    for (k = 0; k < mbsProcessorNum2; k++)
    {
        img->mbsProcessor[k+1]->anotherProcessor = img->mbsProcessor[k];
    }
    img->mbsProcessor[0]->anotherProcessor = img->mbsProcessor[k];

    return AVC_TRUE;
}

int Free_Neighbor_Info(ImageParameters *img)
{
    TMBsProcessor *info ;
    avdNativeInt i, j, size, k = 0;

    avdNativeInt		coreNum 		= img->vdLibPar->multiCoreNum;
    avdNativeInt		mbsProcessorNum = max(2, coreNum); //at least we need 2 mbsProcessor
    avdNativeInt		mbsProcessorNum2 = mbsProcessorNum;
    seq_parameter_set_rbsp_t		*active_sps = img->active_sps;
    int			isPossibleMBAff = active_sps && active_sps->mb_adaptive_frame_field_flag;
    AVDIPDeblocker *ipDeblocker;
    do
    {

        info = img->mbsProcessor[k++];
        if (!info || !info->neighborMBs)
            continue;
        if(k == 1 || coreNum >= 2)
        {

            SafevoH264AlignedFree(img, info->mbsParser);
        }
        ipDeblocker = GetIPDeblocker(info);
        SafevoH264AlignedFree(img, ipDeblocker->topY);
        SafevoH264AlignedFree(img, ipDeblocker);
        SafevoH264AlignedFree(img, info->neighborMBs);
#if FEATURE_INTERLACE
        if(isPossibleMBAff)
            SafevoH264AlignedFree(img, info->neighborMBsBottom);
#endif//FEATURE_INTERLACE
        if(coreNum >= 2)
            ExitDCoreThread(img, info);

    }
    while(--mbsProcessorNum > 0);
    for (k = 0; k < mbsProcessorNum2; k++)
    {
        SafevoH264AlignedFree(img, img->mbsProcessor[k]);
    }


    return AVC_TRUE;
}

#endif

int Init_Slice_Neighbor_Info(ImageParameters *img)
{
    //////TMBsProcessor *info  = img->mbsProcessor;

    //info->prevMBY = img->GetMBY(info);

    return AVC_TRUE;
}



static void update_MB_neighbor_info(ImageParameters *img, TMBsProcessor *info, MacroBlock *mbInfo, avdNativeUInt isForLeftBdry)
{

    avdUInt8			*numRuns = &info->mbsParser->numRuns[0];
    Macroblock			*currMB = info->currMB;
    TMBsParser			 *parser = GetMBsParser(info);
    TMBBitStream		*mbBits = GetMBBits(currMB);
    TSizeInfo	*sizeInfo = img->sizeInfo;
#if FEATURE_CABAC||FEATURE_T8x8//non baseline
    TCABACInfo	*cabacInfo = img->cabacInfo;
    //niMarkFrameMb(mbInfo);

#endif //VOI_H264D_BASELINE_SINGLE_SLICE
    int tmp;
    tmp = (parser->cbp & 15);
    mbSetCBPHasLuma(mbBits, tmp);
    tmp = (parser->cbp > 15);
    mbSetCBPHasChroma(mbBits, tmp);
#if FEATURE_CABAC//VOI_H264D_NON_BASELINE
    if (!IsVLCCoding(img->active_pps))
    {

        niSetCabacCBP(mbInfo, parser->cbp);
        niGetCabacNeighborStruct(mbInfo)->cbpBits = parser->cbpBits;
        tmp = (!mbGetCPredMode(mbBits) || !IS_INTRA(currMB) || IS_IPCM(currMB)) ? 0 : 1;
        niSetCabacNonZeroIntraChromaMode(mbInfo, tmp);

    }
    else
#endif
    {
        niSetCavlcSkippedOrLumaChromaAllACZeros(mbInfo, mbIsSkipped(info->currMB), parser->cbp);

#ifndef VOI_H264D_BLOCK_IPCM
        if (IS_IPCM(info->currMB))
        {
            niGetCavlcNeighborStruct(mbInfo)->numNonZerosLuma |=
                (0x10 | (0x10 << 5) | (0x10 << 10) | (0x10 << 15));
            niGetCavlcNeighborStruct(mbInfo)->numNonZerosLumaLeft |=
                (0x10 | (0x10 << 5) | (0x10 << 10) | (0x10 << 15));
            niSetCavlcNumNonZerosChroma(mbInfo, 0xf, 0xf,
                                        0xf, 0xf);
            niSetCavlcNumNonZerosChromaLeft(mbInfo, 0xf, 0xf,
                                            0xf, 0xf);
            return;
        }
#endif

        // numRun are in SubBlockScan order;
        //if (!isForLeftBdry)
        {
            niGetCavlcNeighborStruct(mbInfo)->numNonZerosLuma |=
                ( numRuns[10 + COEFF_LUMA_START_IDX] |
                  (numRuns[11 + COEFF_LUMA_START_IDX] << 5) |
                  (( numRuns[14 + COEFF_LUMA_START_IDX]) << 10) |
                  (( numRuns[15 + COEFF_LUMA_START_IDX]) << 15));
            niSetCavlcNumNonZerosChroma(mbInfo,
                                        numRuns[3 + COEFF_CHROMA_V_AC_START_IDX],
                                        numRuns[2 + COEFF_CHROMA_V_AC_START_IDX],
                                        numRuns[3 + COEFF_CHROMA_U_AC_START_IDX],
                                        numRuns[2 + COEFF_CHROMA_U_AC_START_IDX]);
            //}
            //else { // get right boundary used for neighborA later;

            niGetCavlcNeighborStruct(mbInfo)->numNonZerosLumaLeft |=
                ( numRuns[5 + COEFF_LUMA_START_IDX] |
                  (numRuns[7 + COEFF_LUMA_START_IDX] << 5) |
                  (( numRuns[13 + COEFF_LUMA_START_IDX]) << 10) |
                  (( numRuns[15 + COEFF_LUMA_START_IDX]) << 15));
            niSetCavlcNumNonZerosChromaLeft(mbInfo,
                                            numRuns[3 + COEFF_CHROMA_V_AC_START_IDX],
                                            numRuns[1 + COEFF_CHROMA_V_AC_START_IDX],
                                            numRuns[3 + COEFF_CHROMA_U_AC_START_IDX],
                                            numRuns[1 + COEFF_CHROMA_U_AC_START_IDX]);
        }
    }
}
#if FEATURE_INTERLACE
//ref the Table6-4 - Specification of mbAddrN and yM
MacroBlock *GetNeighborMB(ImageParameters *img, TMBsProcessor *info, int addr)
{
    TMBsProcessor		*info2	= info->anotherProcessor;
    avdNativeInt		mb_x	= GetMBX(info);
    MacroBlock  *value = NULL;
    switch (addr)
    {
    case MBADDR_A_TOP:
        value = &info->neighborMBs[mb_x-1];
        break;
    case MBADDR_A_BOTTOM:
        value = &info->neighborMBsBottom[mb_x-1];
        break;
    case MBADDR_B_TOP:
        value = &info2->neighborMBs[mb_x];
        break;
    case MBADDR_B_BOTTOM:
        value = &info2->neighborMBsBottom[mb_x];
        break;
    case MBADDR_C_TOP:
        value = &info2->neighborMBs[mb_x+1];
        break;
    case MBADDR_C_BOTTOM:
        value = &info2->neighborMBsBottom[mb_x+1];
        break;
    case MBADDR_D_TOP:
        value = &info2->neighborMBs[mb_x-1];
        break;
    case MBADDR_D_BOTTOM:
        value = &info2->neighborMBsBottom[mb_x-1];
        break;
    case MBADDR_Curr_TOP:
        value = &info->neighborMBs[mb_x];
        break;
    case MBADDR_Curr_BOTTOM:
        value = &info->neighborMBsBottom[mb_x];
        break;
    }
    return value;
}
int GetYPosition(int yPosIdx, int yN, int maxH)
{
    int value = yN;
    switch (yPosIdx)
    {
        //case YPOS_YN:
        //	break;
    case YPOS_YN_PLUS_MAXH_DIV_2:
        value = (yN + maxH) >> 1;
        break;
    case YPOS_YN_MUL_2:
        value = yN * 2;
        break;
    case YPOS_YN_DIV_2:
        value = yN >> 1;
        break;
    case YPOS_YN_MUL_2_SUB_MAXH:
        value = (yN * 2) - maxH;
        break;
    case YPOS_YN_MUL_2_PLUS_1:
        value = (yN * 2) + 1;
        break;
    case YPOS_YN_MUL_2_SUB_15:
        value = (yN * 2) + 1 - maxH;
        break;
    }
    return value;
}


typedef TMBAddrNYPos TMBAddrNYPosArray[2][2];
typedef TMBAddrNYPos TMBAddrNYPosArray2[2];
const static TMBAddrNYPos  mbAddrA_Field_Bot[2][2] = //[mbXisFrame][yN<(maxH/2)]
{
    //mbA is Field
    {
        //yN>=(maxH/2)
        {MBADDR_A_BOTTOM, YPOS_YN, 0},
        //yN<(maxH/2)
        {MBADDR_A_BOTTOM, YPOS_YN, 0},
    },
    //mbA is Frame
    {
        //yN>=(maxH/2)
        {MBADDR_A_BOTTOM, YPOS_YN_MUL_2_SUB_15, 0},
        //yN<(maxH/2)
        {MBADDR_A_TOP, YPOS_YN_MUL_2_PLUS_1, -16},
    },
};
const static TMBAddrNYPos  mbAddrA_Field_Top[2][2] = //[mbXisFrame][yN<(maxH/2)]
{
    //mbA is Field
    {
        {MBADDR_A_TOP, YPOS_YN, 0},
        {MBADDR_A_TOP, YPOS_YN, 0},
    },
    //mbA is Frame
    {
        //yN>=(maxH/2)
        {MBADDR_A_BOTTOM, YPOS_YN_MUL_2_SUB_MAXH, 16},
        //yN<(maxH/2)
        {MBADDR_A_TOP, YPOS_YN_MUL_2, 0},
    },
};
const static TMBAddrNYPos  mbAddrA_Frame_Bot[2][2] = //[mbXisFrame][(yN&1)==0]
{
    //mbA is Field
    {
        {MBADDR_A_BOTTOM, YPOS_YN_PLUS_MAXH_DIV_2, 0},
        {MBADDR_A_TOP, YPOS_YN_PLUS_MAXH_DIV_2, -16},
    },
    //mbA is Frame
    {
        {MBADDR_A_BOTTOM, YPOS_YN, 0},
        {MBADDR_A_BOTTOM, YPOS_YN, 0},
    },
};
const static TMBAddrNYPos  mbAddrA_Frame_Top[2][2] = //[mbXisFrame][(yN&1)==0]
{
    //mbA is Field
    {
        {MBADDR_A_BOTTOM, YPOS_YN_DIV_2, 16},
        {MBADDR_A_TOP, YPOS_YN_DIV_2, 0},
    },
    //mbA is Frame
    {
        {MBADDR_A_TOP, YPOS_YN, 0},
        {MBADDR_A_TOP, YPOS_YN, 0},
    },
};
const static TMBAddrNYPosArray *NeighborMBAddrA[2][2] = //[isCurrFrame][isCurrTop]
{

    //currMB is field
    {
        mbAddrA_Field_Bot,
        mbAddrA_Field_Top,

    },
    //currMB is Frame
    {
        mbAddrA_Frame_Bot,
        mbAddrA_Frame_Top,
    }

};

const static TMBAddrNYPos  mbAddrB_Field_Bot[2] = { {MBADDR_B_BOTTOM, YPOS_YN, -32}, {MBADDR_B_BOTTOM, YPOS_YN, -32}};
const static TMBAddrNYPos  mbAddrB_Field_Top[2] = { {MBADDR_B_TOP, YPOS_YN, -32}, {MBADDR_B_BOTTOM, YPOS_YN_MUL_2, -16}};
const static TMBAddrNYPos  mbAddrB_Frame_Bot[2] = { {MBADDR_Curr_TOP, YPOS_YN, -16}, {MBADDR_Curr_TOP, YPOS_YN, -16}};
const static TMBAddrNYPos  mbAddrB_Frame_Top[2] = { {MBADDR_B_BOTTOM, YPOS_YN, -16}, {MBADDR_B_BOTTOM, YPOS_YN, -16}};
const static TMBAddrNYPosArray2 *NeighborMBAddrB[2][2] = //[isCurrFrame][isCurrTop]
{

    //currMB is field
    {
        mbAddrB_Field_Bot,
        mbAddrB_Field_Top,//Frame and field is same

    },
    //currMB is Frame
    {
        mbAddrB_Frame_Bot,
        mbAddrB_Frame_Top,
    }

};
const static TMBAddrNYPos  mbAddrC_Field_Bot[2] = { {MBADDR_C_BOTTOM, YPOS_YN, -32}, {MBADDR_C_BOTTOM, YPOS_YN, -32}};
const static TMBAddrNYPos  mbAddrC_Field_Top[2] = { {MBADDR_C_TOP, YPOS_YN, -32}, {MBADDR_C_BOTTOM, YPOS_YN_MUL_2, -16}};
//const static TMBAddrNYPos  mbAddrC_Frame_Bot[2]= { {MBADDR_Curr_TOP, YPOS_YN},{MBADDR_Curr_TOP, YPOS_YN}};
const static TMBAddrNYPos  mbAddrC_Frame_Top[2] = { {MBADDR_C_BOTTOM, YPOS_YN, -16}, {MBADDR_C_BOTTOM, YPOS_YN, -16}};
const static TMBAddrNYPosArray2 *NeighborMBAddrC[2][2] = //[isCurrFrame][isCurrTop]
{

    //currMB is field
    {
        mbAddrC_Field_Bot,
        mbAddrC_Field_Top,//Frame and field is same

    },
    //currMB is Frame
    {
        NULL,
        mbAddrC_Frame_Top,
    }

};

const static TMBAddrNYPos  mbAddrD_Field_Bot[2] = { {MBADDR_D_BOTTOM, YPOS_YN, -32}, {MBADDR_D_BOTTOM, YPOS_YN, -32}};
const static TMBAddrNYPos  mbAddrD_Field_Top[2] = { {MBADDR_D_TOP, YPOS_YN, -32}, {MBADDR_D_BOTTOM, YPOS_YN_MUL_2, -16}};
const static TMBAddrNYPos  mbAddrD_Frame_Bot[2] = { {MBADDR_A_BOTTOM, YPOS_YN_PLUS_MAXH_DIV_2, 0}, {MBADDR_A_TOP, YPOS_YN, -16}};
const static TMBAddrNYPos  mbAddrD_Frame_Top[2] = { {MBADDR_D_BOTTOM, YPOS_YN, -16}, {MBADDR_D_BOTTOM, YPOS_YN, -16}};
const static TMBAddrNYPosArray2 *NeighborMBAddrD[2][2] = //[isCurrFrame][isCurrTop]
{

    //currMB is field
    {
        mbAddrD_Field_Bot,
        mbAddrD_Field_Top,//Frame and field is same

    },
    //currMB is Frame
    {
        mbAddrD_Frame_Bot,
        mbAddrD_Frame_Top,
    }

};
#endif//#if FEATURE_INTERLACE
int Init_MB_Neighbor_Info(ImageParameters *img, TMBsProcessor *info, int isParsing)
{

    // update data of current block;
    MacroBlock	*tmpMbInfo;
    //TMBsProcessor *info  = img->mbsProcessor;
    TSizeInfo	*sizeInfo = img->sizeInfo;
    TPosInfo  *posInfo  = GetPosInfo(info);
    avdNativeInt		mb_x = GetMBX(info);
    avdNativeInt		mb_y = GetMBY(info);
    avdNativeInt		diff;
    MacroBlock *currMB = info->currMB = GetCurrMB(info, mb_x);
    TMBBitStream	*mbBits = GetMBBits(currMB);


    TMBsParser			 *parser = GetMBsParser(info);

    {
        int current_mb_nr = GetCurPos(info, sizeInfo);
        int diff = current_mb_nr - info->start_mb_nr;
        TMBsProcessor *info2 = info->anotherProcessor;
        GetLeftMB(info) = (mb_x && diff) ?
                          //info->leftMB
                          &info->neighborMBs[mb_x-1]
                          : NULL;
        GetUpMB(info) = (mb_y && diff >= sizeInfo->PicWidthInMbs) ?
                        &info2->neighborMBs[mb_x] : NULL;
        GetUpRightMB(info) = (mb_y && mb_x < sizeInfo->PicWidthInMbs - 1
                              && diff >= sizeInfo->PicWidthInMbs - 1) ?
                             &info2->neighborMBs[mb_x + 1] : NULL;
        GetUpLeftMB(info) = (mb_y && mb_x
                             && diff >= sizeInfo->PicWidthInMbs + 1) ?
                            &info2->neighborMBs[mb_x-1] : NULL;


    }



    //VOI_H264D_BASELINE_SINGLE_SLICE
    if(isParsing)
    {

#ifdef USE_JOBPOOL
        int 		sliceNR = img->vdLibPar->multiCoreNum < 2 ? img->current_slice_nr - 1 : info->current_slice_nr - 1;
#else
        int			sliceNR = img->current_slice_nr - 1;
#endif
        mbResetMB(currMB);
        mbSetQP(currMB, img->qp);
        mbSetSliceNR(currMB, sliceNR);
        mbUnMarkMbField(currMB);
        mbResetMBBSFlag(mbBits);
        parser->cbpBits = 0;
        if (IsVLCCoding(img->active_pps))
        {
            niResetCavlcNiFlag2(info->currMB);
        }
        info->neighborMBsActivate	= info->neighborMBs;
#if DEBUG_POS
        currMB->mb_y = mb_y;
        currMB->mb_x = mb_x;
#endif//

    }
    parser->cof = GetRowCof(currMB);
    return AVC_TRUE;
}

#if FEATURE_INTERLACE
TMBAddrNYPos  GetNeighborAFrame2Field(void *info2, TNeighborA *neighborAs, int yN, int maxH)
{
    TMBsProcessor *info = (TMBsProcessor *)info2;
    return GetLeftMBYPos(info) = (*neighborAs)[(yN&1)==0];
}
TMBAddrNYPos  GetNeighborAField2Frame(void *info2, TNeighborA *neighborAs, int yN, int maxH)
{
    TMBsProcessor *info = (TMBsProcessor *)info2;
    return GetLeftMBYPos(info) = (*neighborAs)[yN<(maxH/2)];
}
MacroBlock *GetLeftMBAffMB2(ImageParameters *img, TMBsProcessor *info, avdNativeInt yOff, avdNativeInt  maxH, int *outYW)
{
    MacroBlock		*mbInfo = GetLeftMB(info);
    if(mbInfo)
        *outYW = yOff;
    if(mbInfo == NULL && GetLeftMBAFF(info))
    {
        TMBAddrNYPos	addrNYPos = info->getNeighborAFunc(info, info->neighborA, yOff, maxH);
        int yN = GetYPosition(addrNYPos.yPosIdx, yOff, maxH);
        mbInfo = GetNeighborMB(img, info, addrNYPos.mbAddr);
        *outYW = (yN + maxH) & (maxH - 1);
    }
    return mbInfo;
}
void SetActiveNeighborMBAff(ImageParameters *img, TMBsProcessor *info)
{
    TMBsParser			 *parser = GetMBsParser(info);
    if(info->mb_y & 1) //next is bottom
    {
        info->neighborMBsActivate	= info->neighborMBsBottom;
        //parser->mbBitStreamActivate = parser->mbBitStreamBottom;
        //parser->rowCofActivate		= parser->rowCofBottom;
        //parser->ipDeblockerActivate = parser->ipDeblockerBottom;
    }
    else//curr is bottom
    {
        info->neighborMBsActivate	= info->neighborMBs;
        //parser->mbBitStreamActivate = parser->mbBitStream;
        //parser->rowCofActivate		= parser->rowCof;
        //parser->ipDeblockerActivate = parser->ipDeblocker;
    }
    info->currMB = GetCurrMB(info, GetMBX(info));
}

int GetFieldByPos(ImageParameters *img, TMBsProcessor *info, int mb_y, int mb_x)
{
    int pos = 2 * (img->sizeInfo->PicWidthInMbs) * (mb_y / 2) + (mb_x) * 2 + (mb_y & 1); //GetCurPos(info,img->sizeInfo);
    return mbIsMbField2(pos);
}

void SetFieldByPos(ImageParameters *img, TMBsProcessor *info, int mb_y, int mb_x, int isField)
{
    int pos = 2 * (img->sizeInfo->PicWidthInMbs) * (mb_y / 2) + (mb_x) * 2 + (mb_y & 1); //GetCurPos(info,img->sizeInfo);
    mbSetMbFiled2(pos, isField);
}
int Init_MB_Neighbor_InfoMBAff(ImageParameters *img, TMBsProcessor *info, int isParsing)
{

    // update data of current block;
    MacroBlock	*tmpMbInfo, *currMB;
    //TMBsProcessor *info  = img->mbsProcessor;
    TSizeInfo	*sizeInfo = img->sizeInfo;
    TPosInfo  *posInfo  = GetPosInfo(info);
    avdNativeInt		mb_x = GetMBX(info);
    avdNativeInt		mb_y2, mb_y = GetMBY(info);
    avdNativeInt		diff;
    TMBBitStream	*mbBits;
    int isCurrTop;
    int isCurrFrame;
    int widthMBAff = sizeInfo->PicWidthInMbs;
    int width = sizeInfo->width;
    TMBsParser			 *parser = GetMBsParser(info);
    SetActiveNeighborMBAff(img, info);
    currMB = info->currMB = GetCurrMB(info, mb_x);
    mbBits = GetMBBits(currMB);
    isCurrTop	= CurrMBisTop(info);
    isCurrFrame = !mbIsMbField(currMB);
    if(isParsing)
    {

#ifdef USE_JOBPOOL
        int 		sliceNR = img->vdLibPar->multiCoreNum < 2 ? img->current_slice_nr - 1 : info->current_slice_nr - 1;
#else
        int 		sliceNR = img->current_slice_nr - 1;
#endif
        TMBBitStream *mbBits = GetMBBits(currMB);
        mbResetMBBSFlag(mbBits);
        mbResetMB(currMB);
        mbSetQP(currMB, img->qp);
        mbSetSliceNR(currMB, sliceNR);
        mbSetMbFieldValue(currMB, !isCurrFrame);
        SetFieldByPos(img, info, mb_y, mb_x, !isCurrFrame);
        if(isCurrTop)
            mbSetMbFieldValue(GetBottomMB(info, mb_x), !isCurrFrame);

#if DEBUG_POS
        currMB->mb_y = mb_y;
        currMB->mb_x = mb_x;
        currMB->isField = !isCurrFrame;
#endif//
    }

    if(!isCurrFrame && !isCurrTop)
        widthMBAff *= 2;//sizeInfo->PicWidthInMbs + (!isCurrFrame*sizeInfo->PicWidthInMbs;

    {

        int yPos = isCurrTop ? mb_y : mb_y - 1;
        int current_mb_nr = yPos * sizeInfo->PicWidthInMbs + mb_x;
        int diff = current_mb_nr - info->start_mb_nr;
        TMBsProcessor *info2 = info->anotherProcessor;
        TMBAddrNYPosArray2  *addr;
        MacroBlock *mbAddrX;
        int					isMB_Frame ;
        TMBAddrNYPos	addrNYPos;
        //Get Left MB
        if(mb_x && diff)
        {
            TMBAddrNYPosArray *addr		= NeighborMBAddrA[isCurrFrame][isCurrTop];
            mbAddrX		= GetTopMB(info, mb_x - 1);
            isMB_Frame  = !mbIsMbField(mbAddrX);
            if(isCurrFrame == isMB_Frame)
            {
                addrNYPos = (*addr)[isMB_Frame][0];
                GetLeftMB(info)		= GetNeighborMB(img, info, addrNYPos.mbAddr);
                GetLeftMBYPos(info)	= addrNYPos;
                ResetNeighborA_MBAFF(info);
            }
            else
            {
                GetLeftMBAFF(info)		= (TNeighborA *)(*addr)[isMB_Frame];
                info->getNeighborAFunc	= isCurrFrame ? GetNeighborAFrame2Field : GetNeighborAField2Frame;
                GetLeftMB(info)			= NULL;//this value will be recalculated later,it may be mbAddrX+1
                //GetLeftMBYPos(info)	= 0;
            }

        }
        else
        {
            GetLeftMB(info)		=  NULL;
            ResetNeighborA_MBAFF(info);
        }
#define GETYPOSITION(x) (x)	//GetYPosition(x, -1,16)	
        current_mb_nr	= GetCurPos(info, sizeInfo);
        diff			= current_mb_nr - info->start_mb_nr;
        if((mb_y && diff >= widthMBAff))
        {
            addr		= NeighborMBAddrB[isCurrFrame][isCurrTop];
            if((int)(addr) != (int)mbAddrB_Frame_Bot)
            {
                mbAddrX = GetTopMB(info2, mb_x);
                isMB_Frame = !GetFieldByPos(img, info, mb_y - 2, mb_x);
                mbSetMbFieldValue3(mbAddrX, !isMB_Frame); //for dual core
                mbSetMbFieldValue3(GetBottomMB(info2, mb_x), !isMB_Frame); //for dual core
            }
            else
            {
                mbAddrX = currMB;
                isMB_Frame = isCurrFrame;
                mbSetMbFieldValue3(mbAddrX, !isMB_Frame); //for dual core
            }

            addrNYPos = (*addr)[isMB_Frame];
            GetUpMB(info)		= GetNeighborMB(img, info, addrNYPos.mbAddr);

            GetUpMBYPos(info)	= GETYPOSITION(addrNYPos) ;

        }
        else
            GetUpMB(info) =  NULL;

        addr		= NeighborMBAddrC[isCurrFrame][isCurrTop];
        if((mb_y && mb_x < sizeInfo->PicWidthInMbs - 1 && diff >= widthMBAff - 1) && addr)
        {
            mbAddrX		= GetTopMB(info2, mb_x + 1);
            isMB_Frame  = !GetFieldByPos(img, info, mb_y - 2, mb_x + 1);
            mbSetMbFieldValue3(mbAddrX, !isMB_Frame); //for dual core
            mbSetMbFieldValue3(GetBottomMB(info2, mb_x + 1), !isMB_Frame); //for dual core
            addrNYPos	= (*addr)[isMB_Frame];
            GetUpRightMB(info)		=  GetNeighborMB(img, info, addrNYPos.mbAddr);
            GetUpRightMBYPos(info)	= GETYPOSITION(addrNYPos) ;
        }
        else
            GetUpRightMB(info) =  NULL;



        if(mb_y && mb_x && diff >= widthMBAff + 1)
        {
            addr		= NeighborMBAddrD[isCurrFrame][isCurrTop];
            if((int)(addr) != (int)mbAddrD_Frame_Bot)
            {
                mbAddrX = GetTopMB(info2, mb_x - 1);
                isMB_Frame = !GetFieldByPos(img, info, mb_y - 2, mb_x - 1);
                mbSetMbFieldValue3(mbAddrX, !isMB_Frame); //for dual core
                mbSetMbFieldValue3(GetBottomMB(info2, mb_x - 1), !isMB_Frame); //for dual core
            }
            else
            {
                mbAddrX = GetTopMB(info, mb_x - 1);
                isMB_Frame = !GetFieldByPos(img, info, mb_y, mb_x - 1); //!mbIsMbField(mbAddrX);
                mbSetMbFieldValue3(mbAddrX, !isMB_Frame); //for dual core
                mbSetMbFieldValue3(GetBottomMB(info, mb_x - 1), !isMB_Frame); //for dual core
            }
            addrNYPos = (*addr)[isMB_Frame];
            GetUpLeftMB(info)		= GetNeighborMB(img, info, addrNYPos.mbAddr);
            GetUpLeftMBYPos(info)	= GETYPOSITION(addrNYPos) ;

        }
        else
            GetUpLeftMB(info) =  NULL;

    }
    if(isParsing)
    {

        parser->cbpBits = 0;
        if (IsVLCCoding(img->active_pps))
        {
            niResetCavlcNiFlag2(info->currMB);
        }
        if(mbIsMbField(currMB)) //TBD: the two parmas should be MB dependent
        {
            img->num_ref_idx_l0_active <<= 1;
            img->num_ref_idx_l1_active <<= 1;
        }


    }
    parser->cof = GetRowCof(currMB);
#if 0
    if(isParsing)
        AvdLog(DUMP_SLICE, DUMP_DCORE"\nneighborField_parsing(mb=%d):%d:%d,%d,%d", GetCurPos(info, sizeInfo), isParsing, GetUpMB(info) ? mbIsMbField(GetUpMB(info)) : -1, GetUpRightMB(info) ? mbIsMbField(GetUpRightMB(info)) : -1,
               GetUpLeftMB(info) ? mbIsMbField(GetUpLeftMB(info)) : -1);
    else
        AvdLog(DUMP_SLICE, DUMP_DCORE"\nneighborField_parsing(mb=%d):%d:%d,%d,%d", GetCurPos(info, sizeInfo), isParsing, GetUpMB(info) ? mbIsMbField3(GetUpMB(info)) : -1, GetUpRightMB(info) ? mbIsMbField3(GetUpRightMB(info)) : -1,
               GetUpLeftMB(info) ? mbIsMbField3(GetUpLeftMB(info)) : -1);
#endif
    return AVC_TRUE;
}




void NeighborAB_Inference_CABAC2(ImageParameters *img, TMBsProcessor *info,
                                 MacroBlock *currMB,
                                 avdNativeInt mb_x,
                                 avdNativeInt mb_y,
                                 MacroBlock *nMBs[2])
{


    // update data of current block;
    MacroBlock	*tmpMbInfo;
    //TMBsProcessor *info  = img->mbsProcessor;
    TSizeInfo	*sizeInfo = img->sizeInfo;
    TPosInfo  *posInfo  = GetPosInfo(info);
    avdNativeInt		diff;
    TMBBitStream	*mbBits;
    int isCurrTop;
    int isCurrFrame;
    int widthMBAff = sizeInfo->PicWidthInMbs;
    TMBsParser			 *parser = GetMBsParser(info);
    mbBits = GetMBBits(currMB);
    isCurrTop	= (mb_y & 1) == 0;
    isCurrFrame = !mbIsMbField(currMB);

#if DEBUG_POS
    currMB->mb_y = mb_y;
    currMB->mb_x = mb_x;
    currMB->isField = !isCurrFrame;
#endif//


    if(!isCurrFrame && !isCurrTop)
        widthMBAff *= 2;//sizeInfo->PicWidthInMbs + (!isCurrFrame*sizeInfo->PicWidthInMbs;

    {

        int yPos = isCurrTop ? mb_y : mb_y - 1;
        int current_mb_nr = yPos * sizeInfo->PicWidthInMbs + mb_x;
        int diff = current_mb_nr - info->start_mb_nr;
        TMBsProcessor *info2 = info->anotherProcessor;
        TMBAddrNYPosArray2  *addr;
        MacroBlock *mbAddrX;
        int					isMB_Frame ;
        TMBAddrNYPos	addrNYPos;
        //Get Left MB
        if(mb_x && diff)
        {
            TMBAddrNYPosArray *addr		= NeighborMBAddrA[isCurrFrame][isCurrTop];
            mbAddrX		= GetTopMB(info, mb_x - 1);
            isMB_Frame  = !mbIsMbField(mbAddrX);
            if(isCurrFrame == isMB_Frame)
            {
                addrNYPos = (*addr)[isMB_Frame][0];
                GetLeftMB(info)		= GetNeighborMB(img, info, addrNYPos.mbAddr);
                GetLeftMBYPos(info)	= addrNYPos;
                ResetNeighborA_MBAFF(info);
            }
            else
            {
                GetLeftMBAFF(info)		= (TNeighborA *)(*addr)[isMB_Frame];
                info->getNeighborAFunc	= isCurrFrame ? GetNeighborAFrame2Field : GetNeighborAField2Frame;
                GetLeftMB(info)			= NULL;//this value will be recalculated later,it may be mbAddrX+1
                //GetLeftMBYPos(info)	= 0;
            }
            nMBs[NEIGHBOR_A] = currMB - 1;
        }
        else
        {
            GetLeftMB(info)		=  NULL;
            ResetNeighborA_MBAFF(info);
        }
#define GETYPOSITION(x) (x)	//GetYPosition(x, -1,16)	
        current_mb_nr	= GetCurPos(info, sizeInfo);
        diff			= current_mb_nr - info->start_mb_nr;
        if((mb_y && diff >= widthMBAff))
        {
            addr		= NeighborMBAddrB[isCurrFrame][isCurrTop];
            if((int)(addr) != (int)mbAddrB_Frame_Bot)
            {
                mbAddrX = GetTopMB(info2, mb_x);
                isMB_Frame = !GetFieldByPos(img, info, mb_y - 2, mb_x); //!mbIsMbField3(mbAddrX);
            }
            else
            {
                mbAddrX = currMB;
                isMB_Frame = !mbIsMbField(mbAddrX);
            }
            addrNYPos = (*addr)[isMB_Frame];
            GetUpMB(info)		= GetNeighborMB(img, info, addrNYPos.mbAddr);
            GetUpMBYPos(info)	= GETYPOSITION(addrNYPos) ;
            nMBs[NEIGHBOR_B]	= isCurrTop ? GetTopMB(info2, mb_x) : GetBottomMB(info2, mb_x);
        }
        else
            GetUpMB(info) =  NULL;
    }
    return;
}


int ExitOneMBAff(ImageParameters *img, TMBsProcessor *info)
{

    //int pos = 2*(img->sizeInfo->PicWidthInMbs)*(info->mb_y/2)+ (info->mb_x)*2 + (info->mb_y&1);//GetCurPos(info,img->sizeInfo);
    //mbSetMbFiled2(pos,mbIsMbField(info->currMB));
#if DEBUG_POS
    //info->currMB->isField = mbIsMbField(info->currMB);
#endif//
    if(GetMbAffFrameFlag(img))
    {
        avdNativeUInt		mbY			= GetMBY(info);
        avdNativeInt		currIsTop	= (mbY & 1) == 0;
        if(currIsTop)//update the next MB info
        {
            GetMBY(info) = mbY + 1;
            info->currMBisBottom = 1;
        }
        else //if(GetMBX(info)!=sizeInfo->PicWidthInMbs-1)
        {
            GetMBY(info) = mbY - 1;
            info->currMBisBottom = 0;
        }
    }

}
#endif//FEATURE_INTERLACE
int Finish_MB_Neighbor_Info(ImageParameters *img, TMBsProcessor *info)
{
    //TMBsProcessor *info  = img->mbsProcessor;
    TSizeInfo	*sizeInfo = img->sizeInfo;
    TPosInfo  *posInfo  = GetPosInfo(info);
    avdNativeUInt		mbX = GetMBX(info);

    update_MB_neighbor_info(img, info, info->currMB, AVC_FALSE);
    if(GetMbAffFrameFlag(img))
    {
        ExitOneMBAff(img, info);
    }
    return AVC_TRUE;
}



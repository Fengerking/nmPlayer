
/*!
 *************************************************************************************
 * \file mb_access.c
 *
 * \brief
 *    Functions for macroblock neighborhoods
 *
 *  \author
 *      Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Karsten Sühring          <suehring@hhi.de>
 *************************************************************************************
 */
#include "global.h"
#include "mb_access.h"
#define MAX_LAG_NUM 2
#define MAX_LAG_NUM_MBAFF 2

#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
#include <errno.h>
#include <sys/syscall.h>
#endif


void voH264OS_Sleep (VO_U32 nTime)
{
#ifdef _WIN32
	Sleep (nTime);
#elif defined _LINUX
	usleep (1000 * nTime);
#elif defined __SYMBIAN32__
	User::After (nTime * 1000);
#elif defined _IOS
	usleep (1000 * nTime);
#elif defined _MAC_OS
	usleep (1000 * nTime);
#endif // _WIN32
}




#define GetReadPos(info,stride) ((info)->mb_y*(stride)+ (info)->read_mb_num)
#define GetDecPos(info,stride) ((info)->mb_y*(stride)+ (info)->dec_mb_num)
#define GetTopYMBAff(info) ((info)->mb_y - ((info)->mb_y&1))
//#define GetDecPosMBAff(info,stride) ((info)->dec_mbaff_pos)

#ifdef USE_JOBPOOL
#if defined(ANDROID)
static int  voSchedSetaffinity(avdUInt32 pid, int mask)
{
    int err = 0, syscallres;

    voLog_264_debug("voSchedSetaffinity pid %p mask %x\n", pid, mask);
    syscallres = syscall(__NR_sched_setaffinity, pid, sizeof(mask), &mask);
    if (syscallres)
    {
        err = errno;
        voLog_264_debug("Error in the syscall setaffinity: mask=%d=0x%x err=%d=0x%x\n", mask, mask, err, err);
    }
    return err;
}
static void  voSchedGetaffinity(void)
{
    int err = 0, syscallres, mask;
    pid_t pid = gettid();
    syscallres = syscall(__NR_sched_getaffinity, pid, sizeof(mask), &mask);
    voLog_264_debug("voSchedGetaffinity pid %p mask %x\n", pid, mask);
    if (syscallres)
    {
        err = errno;
        voLog_264_debug("Error in the syscall voSchedGetaffinity: mask=%d=0x%x err=%d=0x%x\n", mask, mask, err, err);
    }
    else
    {
        int i = 0;
        for( i = 0; i < MAX_CORE_NUM; i++)
        {
            if(mask & ( 1 << i))
                voLog_264_debug("XXXCPU: CPU %d\n", i);
        }
    }
}
#endif
#endif
int		IsReadyRead(ImageParameters *img, TMBsProcessor *info, TMBsProcessor *another)
{
    int ready;
    /*int diff = GetMbAffFrameFlag(img)? GetTopYMBAff(another) -  GetTopYMBAff(info) :another->mb_y -  info->mb_y;
    int rowDiff = GetMbAffFrameFlag(img)+1;
    int rowNum = img->sizeInfo->PicWidthInMbs*rowDiff;*/

    int coreNum = img->vdLibPar->multiCoreNum;
    if(coreNum < 2)
        return 1;
    if(img->error < 0)
    {
        return 0;
    }

    if(!GetMbAffFrameFlag(img))
    {
        int diff = another->mb_y -  info->mb_y;
        int rowNum = img->sizeInfo->PicWidthInMbs;
        ready = (((another->end_of_slice == 0) && (info->end_of_slice == 0))
                 &&//not eos, readable
                 (((another->read_mb_num == rowNum) && (diff == -1)) //prev row is end_of_read
                  || (diff == coreNum - 1)) //cur row is lower
                );
    }
    else
    {
        int diff = GetTopYMBAff(another) -  GetTopYMBAff(info);
        int rowNum = img->sizeInfo->PicWidthInMbs * 2;
        ready = (((another->end_of_slice == 0) && (info->end_of_slice == 0))
                 &&//not eos, readable
                 (((another->read_mb_num == rowNum) && (diff == -2)) //prev row is end_of_read
                  || (diff == 2 * (coreNum - 1))) //cur row is lower
                );
        //TAvdLog(LL_INFO,"@!@s:another(%d,%d),status=%d,red=%d,dec=%d,eos=%d,curr(%d,%d) status =%d,red=%d,dec=%d,eos=%d not ReadyRead\n",
        //	GetMBX(info->anotherProcessor),GetMBY(info->anotherProcessor),info->anotherProcessor->status,//info->anotherProcessor->sliceID,
        //	info->anotherProcessor->read_mb_num,info->anotherProcessor->dec_mb_num,
        //	info->anotherProcessor->end_of_slice,GetMBX(info),GetMBY(info),info->status,//info->sliceID,
        //	info->read_mb_num,info->dec_mb_num,
        //	info->end_of_slice);
    }


#ifdef DEBUG_INFO1
    if(ready == 0)
    {
        static int waitNum = 0;
        if(1)
        {
            AvdLog2(LL_ERROR, "@@@@@@@@!%d @:another(%d,%d),status=%d,red=%d,dec=%d,eos=%d,curr(%d,%d) status =%d,red=%d,dec=%d,eos=%d not ReadyRead\n",
                    info->mcoreID, GetMBX(info->anotherProcessor), GetMBY(info->anotherProcessor), info->anotherProcessor->status, //info->anotherProcessor->sliceID,
                    info->anotherProcessor->read_mb_num, info->anotherProcessor->dec_mb_num,
                    info->anotherProcessor->end_of_slice, GetMBX(info), GetMBY(info), info->status, //info->sliceID,
                    info->read_mb_num, info->dec_mb_num,
                    info->end_of_slice);
            waitNum = 0;
        }
    }
#endif

    return ready &= (another->end_of_slice == 0) && (GetMBY(info) < img->sizeInfo->PicHeightInMbs); //for thread safe
}


int		IsReadyDecode(ImageParameters *img, TMBsProcessor *info, TMBsProcessor *another)
{
    int ready;
    int diff;
    int anotherDecPos, currDecPos;
    int PicWidthInMbs = img->sizeInfo->PicWidthInMbs;
    //int rowDiff = GetMbAffFrameFlag(img)+1;
    //int rowNum = PicWidthInMbs*rowDiff;
    int coreNum = img->vdLibPar->multiCoreNum;
    if(coreNum < 2)
        return 1;
    if(img->error < 0)
        return 0;

#ifdef USE_JOBPOOL
#if 1
    if(info->isFirstOfSlice)
        return 1;

    if(!GetMbAffFrameFlag(img))
    {
        if( info->mb_y == 0)
            return 1;
    }
    else
    {
        if( info->mb_y == 0 || info->mb_y == 1)
            return 1;
    }
    if(another->job_state == EMP_JOB_FINISHED)
        return 1;
#else
    if(info->mb_y == (img->currentSlice->start_mb_nr / PicWidthInMbs))
        return 1;
#endif
#endif


    if(!GetMbAffFrameFlag(img))
    {
        diff = another->mb_y -  info->mb_y;
        anotherDecPos  = GetDecPos(another, PicWidthInMbs);
        currDecPos	   = GetDecPos(info, PicWidthInMbs);
        ready = ((diff == -1) && ( anotherDecPos + PicWidthInMbs - currDecPos > MAX_LAG_NUM)) || (diff == coreNum - 1);
    }
    else
    {
        diff = GetTopYMBAff(another) -  GetTopYMBAff(info);
        anotherDecPos  = (another->dec_mbaff_pos + 1) >> 1;
        currDecPos	   = (info->dec_mbaff_pos + 1) >> 1;
        ready = ((diff == -2) && (anotherDecPos + PicWidthInMbs - currDecPos > MAX_LAG_NUM_MBAFF)) || (diff == 2 * (coreNum - 1));
    }

#ifdef DEBUG_INFO
    if(ready == 0)
    {
        static int waitNum = 0;
        if(waitNum++ > 2000)
        {
            AvdLog2(LL_INFO, "@!@:another(%d,%d),status=%d,red=%d,dec=%d,eos=%d,curr(%d,%d) status =%d,red=%d,dec=%d,eos=%d dec_mbaff_pos(ano=%d,cur=%d)not ReadyDecode\n",
                    GetMBX(info->anotherProcessor), GetMBY(info->anotherProcessor), info->anotherProcessor->status, //info->anotherProcessor->sliceID,
                    info->anotherProcessor->read_mb_num, info->anotherProcessor->dec_mb_num,
                    info->anotherProcessor->end_of_slice, GetMBX(info), GetMBY(info), info->status, //info->sliceID,
                    info->read_mb_num, info->dec_mb_num,
                    info->end_of_slice
                    , another->dec_mbaff_pos
                    , info->dec_mbaff_pos);
            waitNum = 0;
        }
    }
#endif

    return ready;
}


#ifdef USE_JOBPOOL

void InitNewPicProcessor(ImageParameters *img)//,TMBsProcessor* info)
{
    avdNativeInt		i, j, coreNum 		= img->vdLibPar->multiCoreNum;
    avdNativeInt		mbsProcessorNum = max(2, coreNum); //at least we need 2 mbsProcessor
    TMBsProcessor		*info;
    avdNativeInt            n;

    if( img->vdLibPar->multiCoreNum > 1)
    {

#if defined(WIN32) || defined(WINCE)
        WaitForSingleObject(img->JobMutex, INFINITE);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
        pthread_mutex_lock(&img->JobMutex);
#endif

        img->job_cnt = 0;
        img->frame_job_pos = 0;
        img->end_of_frame = 0;
        img->next_slice_nr = 0;
        img->need_restart_of_frame = 0;

        n =  img->field_pic_flag ? img->sizeInfo->PicHeightInMbs : img->sizeInfo->FrameHeightInMbs;
        n  = n / (1 + GetMbAffFrameFlag(img));

		img->slice_max_jobs = n;

        if(n <= 32)
        {
            if(n == 32)
                img->statea = -1;
            else
                img->statea = (1 << n)  - 1;
            img->stateb = 0;
            img->statec = 0;
			img->stated = 0;
        }
        else if( n <= 64)
        {
            img->statea = -1;
            if(n == 64)
                img->stateb = -1;
            else
                img->stateb = (1 << (n - 32)) - 1;
            img->statec = 0;
			img->stated = 0;
        }
        else if( n <= 96)
        {
            img->statea = -1;
			img->stateb = -1;
            if(n == 64)
                img->statec = -1;
            else
                img->statec = (1 << (n - 64)) - 1;
			img->stated = 0;        
        }
		else
        {
            img->statea = -1;
            img->stateb = -1;
			img->statec = -1;
            img->stated = (1 << (n - 96)) - 1;
        }

        img->end_of_core = 1;
        img->err_end_of_core = 1;
        img->restart_end_of_core = 1;
        img->slices_flag = 0;
        img->is_new_pic = 0;

        for(i = 0; i < img->max_jobs; i++)
        {
            info = img->mbsProcessor[i];
            SetProcessorStatus(img, info, EMP_READING);
            GetMBY(info) = (1 + GetMbAffFrameFlag(img)) * i;
            GetMBX(info) = -1;
            info->dec_mb_num =
                info->read_mb_num =
                    info->dec_mbaff_pos = 0;
            info->job_state = EMP_JOB_INIT;
        }

        for(i = 0; i < img->vdLibPar->multiCoreNum; i++)
            img->last_job_flag[i] = 0;

        for(i = 0; i < MAX_SLICE_NUM; i++)
        {
            img->min_slice_jobs[i] = -1;
            img->max_slice_jobs[i] = -1;
        }

#if defined(WIN32) || defined(WINCE)
        ReleaseMutex(img->JobMutex);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
        pthread_mutex_unlock(&img->JobMutex);
#endif

        if(!img->is_cores_running)
        {
#ifdef ANDROID
#if   SETAFFINITY
            if(img->vdLibPar->for_unittest)
                voSchedSetaffinity(gettid(), 1);
#endif
#endif
            for( i = 0; i < MAX_SLICE_NUM; i++)
				img->curr_slice_img[i] = (ImageParameters *)voH264AlignedMalloc(img, 4000, sizeof(ImageParameters));

            for (i = 1; i < mbsProcessorNum; i++) //other processors
            {
                CreateDCoreThread(img, i);
            }
        }

        if(!img->is_cores_running)
            img->is_cores_running = 1;

    }
    else
    {
        //set main processor
        info = img->mbsProcessor[0];
        GetMBY(info) = 0;
        GetMBX(info) = -1;
        info->dec_mb_num =
            info->read_mb_num =
                info->dec_mbaff_pos = 0;
        info->mcoreThreadParam = NULL;
        SetProcessorStatus(img, info, EMP_READING);
        for (i = 1; i < mbsProcessorNum; i++) //other processors
        {
            info = img->mbsProcessor[i];
            GetMBY(info) = (1 + GetMbAffFrameFlag(img)) * i;
            GetMBX(info) = -1;
            info->dec_mb_num =
                info->read_mb_num =
                    info->dec_mbaff_pos = 0;
            if(info->mcoreThreadParam)
                info->mcoreThreadParam->errorID = 0;
            SetProcessorStatus(img, info, EMP_WAIT_DATA);
        }
    }

}

#else

void InitNewPicProcessor(ImageParameters *img)//,TMBsProcessor* info)
{
    avdNativeInt		i, j, coreNum 		= img->vdLibPar->multiCoreNum;
    avdNativeInt		mbsProcessorNum = max(2, coreNum); //at least we need 2 mbsProcessor
    TMBsProcessor		*info;

    //set main processor
    info = img->mbsProcessor[0];
    GetMBY(info) = 0;
    GetMBX(info) = -1;
    info->dec_mb_num =
        info->read_mb_num =
            info->dec_mbaff_pos = 0;
    info->mcoreThreadParam = NULL;
    SetProcessorStatus(img, info, EMP_READING);
    for (i = 1; i < mbsProcessorNum; i++) //other processors
    {
        info = img->mbsProcessor[i];
        GetMBY(info) = (1 + GetMbAffFrameFlag(img)) * i;
        GetMBX(info) = -1;
        info->dec_mb_num =
            info->read_mb_num =
                info->dec_mbaff_pos = 0;
        if(info->mcoreThreadParam)
            info->mcoreThreadParam->errorID = 0;
        SetProcessorStatus(img, info, EMP_WAIT_DATA);
        if(info->mcoreThreadParam == NULL && coreNum > 1)
            CreateDCoreThread(img, info);
    }

}

#endif
void ExitNewPicProcessor(ImageParameters *img)//,TMBsProcessor* info)
{

}

void InitNewSliceProcessor(ImageParameters *img)//,TMBsProcessor* info)
{
#ifdef  USE_JOBPOOL
    avdNativeInt		i, j, coreNum 		= img->vdLibPar->multiCoreNum;
    avdNativeInt		mbsProcessorNum		= max(2, coreNum); //at least we need 2 mbsProcessor
    TMBsProcessor		*info;
    for (i = 0; (i < mbsProcessorNum) && (coreNum <= 1); i++)
    {
        info = img->mbsProcessor[i];
        info->end_of_slice = 0;
        info->dec_mbaff_pos = 0;
        if(info->mcoreThreadParam)
            info->mcoreThreadParam->errorID = 0;
        if(coreNum <= 1) //for multi-core, the start_mb_nr is not thread safe
        {
            info->start_mb_nr =	img->currentSlice->start_mb_nr;
        }

    }
#else
    avdNativeInt		i, j, coreNum 		= img->vdLibPar->multiCoreNum;
    avdNativeInt		mbsProcessorNum		= max(2, coreNum); //at least we need 2 mbsProcessor
    TMBsProcessor		*info;
    for (i = 0; i < mbsProcessorNum; i++)
    {
        info = img->mbsProcessor[i];
        info->end_of_slice = 0;
        info->dec_mbaff_pos = 0;
        if(info->mcoreThreadParam)
            info->mcoreThreadParam->errorID = 0;
        if(coreNum <= 1) //for multi-core, the start_mb_nr is not thread safe
        {
            info->start_mb_nr =	img->currentSlice->start_mb_nr;
        }

    }
#endif
}
void ExitNewSliceProcessor(ImageParameters *img)
{
}
void DumpMbsInfo(TMBsProcessor *info, int oldStatus)
{
#ifdef DEBUG_INFO1
    TAvdLog(LL_INFO, "@!@ChangeStatus:another(%d,%d),status=%d,red=%d,dec=%d,mbaff=%d,eos=%d,flag=%d,curr(%d,%d) status =%d->%d,red=%d,dec=%d,mbaff=%d,eos=%d,flag=%d\n",
            GetMBX(info->anotherProcessor), GetMBY(info->anotherProcessor), info->anotherProcessor->status, //info->anotherProcessor->sliceID,
            info->anotherProcessor->read_mb_num, info->anotherProcessor->dec_mb_num, info->anotherProcessor->dec_mbaff_pos,
            info->anotherProcessor->end_of_slice, info->anotherProcessor->flag, GetMBX(info), GetMBY(info), oldStatus, info->status, //info->sliceID,
            info->read_mb_num, info->dec_mb_num, info->dec_mbaff_pos,
            info->end_of_slice, info->flag);
#endif
}
void SetProcessorStatus(ImageParameters *img, TMBsProcessor *info, EMPStatus status)
{
    if(img->vdLibPar->multiCoreNum >= 2 && status != info->status)
    {
        EMPStatus oldStatus = 	info->status;
        info->status = status;
#ifdef DEBUG_INFO
        DumpMbsInfo(info, oldStatus);
#endif


    }
}
EMPStatus GetProcessorStatus(TMBsProcessor *info)
{
    return info->status;
}

TMBsProcessor *GetMainProcessor(ImageParameters *img)
{

    int pos0 = GetCurPos(img->mbsProcessor[0], img->sizeInfo);
    int pos1 = GetCurPos(img->mbsProcessor[1], img->sizeInfo);
    if(pos0 < pos1)
        return img->mbsProcessor[0];
    else
        return img->mbsProcessor[1];

}
void	SetMainProcessor(ImageParameters *img, TMBsProcessor *info)
{

    //info->dec_mb_num	= MAIN_PROCESSOR_FLAG;
    //info->anotherProcessor->dec_mb_num = 0;
}
#if !DISABLE_MT
#include "voThread.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
	 using namespace _VONAMESPACE;
#endif

static int cheatCompiler = 0;
void	Wait2(ImageParameters *img, TMBsProcessor *info, int ms)
{
    //TAvdLog(LL_INFO,"@!@WAIT$$$:another(%d,%d),status=%d,sliceID=%d,eos=%d,curr(%d,%d) status =%d,sliceID=%d,eos=%d$$$\n",GetMBX(info->anotherProcessor),GetMBY(info->anotherProcessor),info->anotherProcessor->status,info->anotherProcessor->sliceID,info->anotherProcessor->end_of_slice,GetMBX(info),GetMBY(info),info->status,info->sliceID,info->end_of_slice);
    if(img->vdLibPar->multiCoreNum == 2)
    {
        voH264OS_Sleep(0);
#if CALC_FPS
        img->WaitDecTimes++;
#endif
    }
    else
    {
#if 0
        int sleep = 1000;
        while(--sleep)
        {
            cheatCompiler += sleep;
            if (cheatCompiler > 0x7fff)
            {
                cheatCompiler = voH264IntDiv(cheatCompiler, 7631 + sleep);
            }
        }
#else
        voH264OS_Sleep(0);
#if CALC_FPS
        img->WaitDecTimes++;
#endif
#endif

    }

}

#ifdef USE_JOBPOOL
static avdUInt32 voDualCoreProc (void *pParam)
{
    //
    TMCoreThreadParam  *threadParam = (TMCoreThreadParam *)pParam;
    ImageParameters		*img = threadParam->img;
    AvdLog2(LL_INFO, "@!@DCoreThread: enter \n");
    voLogAndroidInfo("@!@VO_H264_DCoreThread: enter \n");
#ifdef ANDROID
#if SETAFFINITY
    if(img->vdLibPar->for_unittest)
        voSchedSetaffinity(gettid(), (1 << threadParam->coreID ));
#endif
#endif
    threadParam->status = ETSDB_START;
    threadParam->job_cnt = 0;
    threadParam->got_a_slice = 0;
#if CALC_FPS
    threadParam->WaitDataTimes = 0;
    threadParam->WaitDecTimes = 0;
#endif

#if defined(WIN32) || defined(WINCE)
    WaitForSingleObject(img->JobMutex, INFINITE);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
    pthread_mutex_lock(&img->JobMutex);
#endif

    do
    {

        threadParam->currInfo = WaitForJobFromPool(img,  threadParam);

        if(threadParam->status == ETSDB_STOP)
            break;

        if(threadParam->currInfo != NULL)
            threadParam->currInfo->mcoreThreadParam = threadParam;
        else
        {
#if defined(WIN32) || defined(WINCE)
            WaitForSingleObject(img->JobMutex, INFINITE);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
            pthread_mutex_lock(&img->JobMutex);
#endif
            continue;
        }

#if defined(WIN32) || defined(WINCE)
        ReleaseMutex(img->JobMutex);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
        pthread_mutex_unlock(&img->JobMutex);
#endif


        if(threadParam->currInfo->status != EMP_WAIT_DATA_ERROR)
        {
            processmbs(img, threadParam->currInfo);
        }
        else
        {
            voH264OS_Sleep (0);
        }
        //it is thread safe to check the status
        if((threadParam->currInfo->status == EMP_WAIT_DATA_ERROR) && (threadParam->errorID == 0))
            SetProcessorStatus(img, threadParam->currInfo, EMP_WAIT_DATA);


        //for thread safe, only when error flag is still signaled and the current status is not XX_ERROR, then set the status
        if((threadParam->errorID < 0) && (threadParam->currInfo->status != EMP_WAIT_DATA_ERROR))
            SetProcessorStatus(img, threadParam->currInfo, EMP_WAIT_DATA_ERROR);


#if 0
        if(threadParam->currInfo->flag & 8 || threadParam->currInfo->end_of_slice )
        {
            clr_1bit(img, (GetMBY(threadParam->currInfo) >> GetMbAffFrameFlag(img)));
			AvdLog2(LL_ERROR, "statea %x stateb %x statec %x\n", img->statea, img->stateb, img->statec);
            if(!img->statea && !img->stateb && !img->statec)
                img->last_job_flag[threadParam->coreID] = 1;
        }
#else
        clr_1bit(img, threadParam->currInfo->curr_job_pos);
        AvdLog2(LL_INFO, "curr job %d max job %d, statea %x stateb %x statec %x stated %x\n", 
			                                threadParam->currInfo->curr_job_pos, 
			                                img->slice_max_jobs,
			                                img->statea, img->stateb, img->statec, img->stated);
        if(!img->statea && !img->stateb && !img->statec && !img->stated)
	        img->last_job_flag[threadParam->coreID] = 1;
#endif


    }
    while(1);

    if(threadParam->currInfo)
    {
#if defined(WIN32) || defined(WINCE)
        ReleaseMutex(img->JobMutex);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
        pthread_mutex_unlock(&img->JobMutex);
#endif
    }

    threadParam->status = ETSDB_EXIT;

    AvdLog2(LL_INFO, "@!@DCoreThread: exit\n");
    voLogAndroidInfo("@!@VO_H264_DCoreThread: exit\n\n");
    return 0;
}

#else
static avdUInt32 voDualCoreProc (void *pParam)
{
    //
    TMCoreThreadParam  *threadParam = (TMCoreThreadParam *)pParam;
    ImageParameters		*img = threadParam->img;
    AvdLog2(LL_INFO, "@!@DCoreThread: enter \n");
    voLogAndroidInfo("@!@VO_H264_DCoreThread: enter \n");
    threadParam->status = ETSDB_START;
    do
    {
        if(threadParam->status == ETSDB_STOP)
            break;
        if(threadParam->currInfo->status != EMP_WAIT_DATA_ERROR)
        {
            processmbs(img, threadParam->currInfo);
        }
        else
        {
            voH264OS_Sleep (0);
        }
        //it is thread safe to check the status
        if((threadParam->currInfo->status == EMP_WAIT_DATA_ERROR) && (threadParam->errorID == 0))
            SetProcessorStatus(img, threadParam->currInfo, EMP_WAIT_DATA);


        //for thread safe, only when error flag is still signaled and the current status is not XX_ERROR, then set the status
        if((threadParam->errorID < 0) && (threadParam->currInfo->status != EMP_WAIT_DATA_ERROR))
            SetProcessorStatus(img, threadParam->currInfo, EMP_WAIT_DATA_ERROR);


    }
    while(1);
    threadParam->status = ETSDB_EXIT;

    AvdLog2(LL_INFO, "@!@DCoreThread: exit\n");
    voLogAndroidInfo("@!@VO_H264_DCoreThread: exit\n\n");
    return 0;
}
#endif

#ifdef USE_JOBPOOL

void CreateDCoreThread(ImageParameters *img, avdNativeInt coreID)
{
    void *tmpHandle = 0;
    int dwID;
    avdUInt32 errType;
    TMCoreThreadParam  *threadParam = img->mcoreThreadParam[coreID] = (TMCoreThreadParam *)voH264AlignedMalloc(img, 4000, sizeof(TMCoreThreadParam));
    threadParam->currInfo = NULL;//the inital processor, which may be changed on the fly
    threadParam->img	  = img;
    threadParam->coreID = coreID;
#if defined(WIN32) || defined(WINCE)
    threadParam->JobEvent =  CreateEvent (NULL, TRUE, FALSE, NULL);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
    VOH264CreateEvent(&threadParam->JobEvent);
#endif

#if defined (_IOS) || defined(_MAC_OS)
    pthread_create((pthread_t *)&threadParam->thread_id, NULL, voDualCoreProc, threadParam);
#else
    voThreadCreate (&tmpHandle, (avdUInt32 *)&dwID, (voThreadProc) voDualCoreProc, threadParam, 0);
#endif
    voH264OS_Sleep(1);
}

static int is_allcore_exit(ImageParameters *img)
{
    int i = 0;
    for(i = 1; i < img->vdLibPar->multiCoreNum; i++)
        if(img->mcoreThreadParam[i]->status != ETSDB_EXIT)
            break;
    if(i ==  img->vdLibPar->multiCoreNum)
        return 1;
    else
        return 0;
}
void  ExitDCoreThread(ImageParameters *img, avdNativeInt coreID)
{
    //TMCoreThreadParam* threadParam = img->mcoreThreadParam ;////for thread safe ,do not use the temporal virable
    if(img->mcoreThreadParam[coreID])
    {

#if defined(WIN32) || defined(WINCE)
        SetEvent(img->mcoreThreadParam[coreID]->JobEvent);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
        VOH264SetEvent(&img->mcoreThreadParam[coreID]->JobEvent);
#endif

        voH264OS_Sleep (0);
        while((coreID == (img->vdLibPar->multiCoreNum - 1)) && !is_allcore_exit(img))
        {

            TAvdLog(LL_INFO, "@!@DCoreThread: trying to exit,%d\n", img->mcoreThreadParam[coreID]->status);
            voH264OS_Sleep (0);
        }

        if(coreID ==  (img->vdLibPar->multiCoreNum - 1))
        {
            int i = 0;

            for( i = 0; i < MAX_SLICE_NUM; i++)
			    SafevoH264AlignedFree(img, img->curr_slice_img[i]);
			
            for(i = 1; i < img->vdLibPar->multiCoreNum; i++)
            {
#if defined(WIN32) || defined(WINCE)
                CloseHandle(img->mcoreThreadParam[i]->JobEvent);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
                VOH264DestroyEvent(&img->mcoreThreadParam[i]->JobEvent);
#endif
                AvdLog2(LL_INFO,"Core %d job cnt %d, total job %d, %.2f%%\n", i, img->mcoreThreadParam[i]->job_cnt,  img->total_job,  (double)(img->mcoreThreadParam[i]->job_cnt * 100) / (double)img->total_job);
                SafevoH264AlignedFree(img, img->mcoreThreadParam[i]);
            }
        }
    }

}

#else
void CreateDCoreThread(ImageParameters *img, TMBsProcessor *info)
{
    void *tmpHandle = 0;
    int dwID;
    avdUInt32 errType;
    TMCoreThreadParam  *threadParam = info->mcoreThreadParam = (TMCoreThreadParam *)voH264AlignedMalloc(img, 4000, sizeof(TMCoreThreadParam));
    threadParam->currInfo = info;//the inital processor, which may be changed on the fly
    threadParam->img	  = img;
    voThreadCreate (&tmpHandle, (avdUInt32 *)&dwID, (voThreadProc) voDualCoreProc, threadParam, 0);
    voH264OS_Sleep(1);
}

void  ExitDCoreThread(ImageParameters *img, TMBsProcessor *info)
{
    //TMCoreThreadParam* threadParam = img->mcoreThreadParam ;////for thread safe ,do not use the temporal virable
    if(info->mcoreThreadParam)
    {
        info->mcoreThreadParam->status = ETSDB_STOP;

        voH264OS_Sleep (1);
        while(info->mcoreThreadParam->status != ETSDB_EXIT)
        {

            TAvdLog(LL_INFO, "@!@DCoreThread: trying to exit,%d\n", info->mcoreThreadParam->status);
            voH264OS_Sleep (1);
        }
        SafevoH264AlignedFree(img, info->mcoreThreadParam);
    }

}
#endif
#else//DISABLE_MT

void	Wait2(ImageParameters *img, TMBsProcessor *info, int ms)
{

}
#ifdef USE_JOBPOOL
void CreateDCoreThread(ImageParameters *img, avdNativeInt coreID)
{
}

void  ExitDCoreThread(ImageParameters *img, avdNativeInt coreID)
{
}

#else
void CreateDCoreThread(ImageParameters *img, TMBsProcessor *info)
{

}
void  ExitDCoreThread(ImageParameters *img, TMBsProcessor *info)
{
}
#endif
#endif//DISABLE_MT



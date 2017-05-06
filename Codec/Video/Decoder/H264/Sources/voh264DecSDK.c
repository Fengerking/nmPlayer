#include		"global.h"
#if !BUILD_WITHOUT_C_LIB
#include		<stdlib.h>
#include		<string.h>
#include		<assert.h>
#endif//BUILD_WITHOUT_C_LIB
static const  char* innerVer="VOH264VER:2011.0110.1355:for test2";

#include		"vlc.h"
#include		"mbuffer.h"
#include		"nalucommon.h"
#include		"h264dec.h"

#include		"voH264.h"
#include		"voCheck.h"
#if CALC_FPS
#ifdef LINUX
#include <stdio.h>
#include <sys/time.h>
#endif
#endif

VO_U32 VO_API  voCheckLibInit2 (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst,VO_LIB_OPERATOR* op)
{
	return 0;
}
VO_U32 VO_API  voCheckLibCheckVideo2 (VO_PTR hCheck, VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_FORMAT * pOutInfo)
{
	return 0;
}
VO_U32 VO_API  voCheckLibResetVideo2 (VO_PTR hCheck, VO_VIDEO_BUFFER * pOutBuffer)
{
	return 0;
}
VO_U32 VO_API  voCheckLibUninit2 (VO_PTR hCheck)
{
	return 0;
}
//avdUInt32 VO_API  voCheckLibInit (void** phCheck, avdUInt32 nID, avdUInt32 nFlag, void* hInst,VO_LIB_OPERATOR*);
//avdUInt32 VO_API  voCheckLibCheckVideo (void* hCheck, void* pOutBuffer, void* pOutInfo);
//avdUInt32 VO_API  voCheckLibResetVideo (void* hCheck, void* pOutBuffer);
//avdUInt32 VO_API  voCheckLibUninit (void* hCheck);
#if defined(WIN32)||defined(_WIN32_WCE)
#ifndef YOUTUBE_HIGH_PROFILE
extern void* hWin32Module;
#else
void* hWin32Module = NULL;
#endif
#endif//


#if 	CALC_FPS
#define MAX_TEST_FPS 1000000
#define MAX_TEST_GAP 10
#endif


VOH264DECRETURNCODE VOCODECAPI voH264DecInitNew(HVOCODEC *phCodec,VO_CODEC_INIT_USERDATA * pUserData2);

VO_U32  VO_API  voH264DecInit2 (VO_HANDLE * phDec,VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{

	VOH264DECRETURNCODE returnCode;
	GlobalH264Data* gData ;
	ImageParameters *img;
	void* pData=NULL;
	TIOInfo	*ioInfo;
	avdUInt32 flag=0x9100;
	PrintVersion();
#if ENABLE_TRACE
	InitLog2();
#endif//ENABLE_TRACE

	AvdLog2(LL_FLOW,"voH264DecInit2 begin,%X",pUserData?pUserData->memflag:0);
	
	
	returnCode = voH264DecInitNew(phDec,pUserData);

	if(returnCode)
	{
		return returnCode;
	}
	gData = (GlobalH264Data*)(*phDec);
	if(gData==NULL)
		return VO_ERR_INVALID_ARG;

	img= gData->img;
	img->pocInfo->poc_interval		=	1;	
	ioInfo = img->ioInfo;

#if 	CALC_FPS
    img->total_time = 0; 
    img->total_count = 0;
    img->WaitDecTimes = 0;
    img->pfps = calloc(sizeof(double), MAX_TEST_FPS); 
#endif	

	
#ifndef DISABLE_LICENSE
	AvdLog2(LL_INFO,"voH264DecInit2:LICENSE_CHECK_ENABLED\n");
#if defined(WIN32)||defined(_WIN32_WCE)
	img->licenseCheck->instance = hWin32Module;
#endif//
	if (pUserData)
	{
		if(pUserData->memflag&0x10)
			pData	=	pUserData->libOperator;
		flag	=	pUserData->memflag;
	}
	img->licenseCheck->init					= (avdUInt32 (VOAPI  *) (void** phCheck, avdUInt32 nID, avdUInt32 nFlag, void* hInst,void*))voCheckLibInit;
	img->licenseCheck->checkVideo		= (avdUInt32 (VOAPI  *) (void* hCheck, void* pOutBuffer, void* pOutInfo))voCheckLibCheckVideo;
	img->licenseCheck->resetVideo		= (avdUInt32 (VOAPI  *) (void* hCheck, void* pOutBuffer))voCheckLibResetVideo;
	img->licenseCheck->unInit				= (avdUInt32 (VOAPI  *) (void* hCheck))voCheckLibUninit;
	img->licenseCheck->init(&(img->licenseCheck->hCheck),VO_INDEX_DEC_H264,flag,img->licenseCheck->instance,pData);
#else//DISABLE_LICENSE
	AvdLog2(LL_INFO,"voH264DecInit2:LICENSE_CHECK_DISABLED\n");
	img->licenseCheck->init = (avdUInt32 (VOAPI  *) (void** phCheck, avdUInt32 nID, avdUInt32 nFlag, void* hInst,void*))voCheckLibInit2;
	img->licenseCheck->checkVideo=(avdUInt32 (VOAPI  *) (void* hCheck, void* pOutBuffer, void* pOutInfo))voCheckLibCheckVideo2;
	img->licenseCheck->resetVideo = (avdUInt32 (VOAPI  *) (void* hCheck, void* pOutBuffer))voCheckLibResetVideo2;
	img->licenseCheck->unInit			=(avdUInt32 (VOAPI  *) (void* hCheck))voCheckLibUninit2;
#endif//DISABLE_LICENSE
	AvdLog2(LL_FLOW,"voH264DecInit2 end");
	ioInfo->outNumber=0;
	ioInfo->inNumber=0;
	return returnCode;
}
#define LIMITOUT 0//10000
VO_U32	VO_API  voH264DecSetInputData2 (VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{

	GlobalH264Data* gData = (GlobalH264Data*)hDec;
	ImageParameters *img;

	avdUInt8* innerBuf;
	if(gData==NULL)
		return VO_ERR_INVALID_ARG;
	img=gData->img;
	img->vdLibPar  = gData->params;
	
#if TEST_DEINTERLACE
	img->vdLibPar->optFlag |= OPT_ENABLE_DEINTERLACE|OPT_FULLPIX_NONREF;//do not use it in formal release
#endif
#if 0//for test
	img->vdLibPar->optFlag = OPT_DISABLE_ERRORLOG;
	img->vdLibPar->enableJump2I = 0;
	img->vdLibPar->enableReuseValidSPS = 1;
#endif
	//img->vdLibPar->optFlag = 0;img->vdLibPar->deblockFlag = 0;|OPT_FULLPIX_NONREF img->vdLibPar->deblockFlag = EDF_ONLYMBEDGE;
#if LIMITOUT
	if (img->ioInfo->outNumber>LIMITOUT)
	{
		AvdLog2(LL_INFO,"this is a limited version,max=%d",LIMITOUT);
		return LIMITOUT ;
	}
#endif
	if(img->vdLibPar->enableCopyInput)
	{
		if(img->ioInfo->in_buffer==NULL)
			img->ioInfo->in_buffer =  (avdUInt8 *)voH264AlignedMalloc(img,5, MAX_CODED_FRAME_SIZE);CHECK_ERR_RET_INT

		innerBuf = img->ioInfo->in_buffer;
		if(pInput->Length>MAX_CODED_FRAME_SIZE)
			return -11;
		memcpy(innerBuf,pInput->Buffer,pInput->Length);
		img->vdLibPar->inBuf = img->ioInfo->in_buffer;
		img->vdLibPar->inBufDataLen = pInput->Length;
	}
	else
	{
		img->vdLibPar->inBuf		= pInput->Buffer;
		img->vdLibPar->inBufDataLen = pInput->Length;
	}
	img->ioInfo->hasNewInput = 1;
#if ENABLE_TRACE
	AvdInData(pInput->Buffer,pInput->Length);	
#endif

	img->vdLibPar->timeStamp = pInput->Time;
	//voH264DecSetParameter(hDec,VO_ID_H264_TIMESTAMP,(signed long)&(pInput->Time));

	//CleanOutDataQueue(
	AvdLog2(LL_INOUT,"input_%d: len=%d,intime=%d\n",img->ioInfo->inNumber,pInput->Length,pInput->Time);
	img->ioInfo->inNumber++;
	//DumpBufInfo(img);
	return 0;
}

#if CALC_FPS
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
static unsigned long GetTickCount(){
#if 1
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
#else
	return clock();
#endif
}
#endif
#endif


VO_U32	VO_API  voH264DecGetOutputData2 (VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo)
{

	H264VdLibParam* params;
	GlobalH264Data* gData = (GlobalH264Data*)hDec;
	ImageParameters *img;
#if 	CALC_FPS
	unsigned long start, end;
#endif	
	OutDataQueueItem item={0};
	VO_U32 ret=0;
	if(gData==NULL)
		return VO_ERR_INVALID_ARG;

#if 	CALC_FPS
	start = GetTickCount();
#endif

	img=gData->img;
	//AvdLog2(LL_INFO,"output_%d\n",img->number);
	params = gData->params;
	if (img->licenseCheck->lastVideo)
	{
		img->licenseCheck->resetVideo(img->licenseCheck->hCheck,NULL);
		img->licenseCheck->lastVideo=NULL;
	}
	if(img->ioInfo->hasNewInput)
	{
		
		VOCODECDATABUFFER inData;
		VOCODECVIDEOBUFFER outData;
		VOCODECVIDEOFORMAT outFormat;
		VOH264DECRETURNCODE returnCode;
		inData.buffer = params->inBuf;
		pOutInfo->InputUsed = inData.length = params->inBufDataLen;
		img->ioInfo->hasNewInput = 0;
		voH264DecProcess(hDec,&inData,&outData,&outFormat);
	}

	//	support output status. The Flag is 0 mean no more output. 1, has more output data.
	if(img->ioInfo->outDataQueue.frameNum)
		pOutInfo->Flag = 1;
	else
		pOutInfo->Flag = 0;

	PopOneOutData(img,&item);

	ret = img->error;
	if(img->error<0&&img->specialInfo->nal_reference_idc != 0)
	{
		if(params->enableJump2I)
			H264Flushing(img);
	}
	if(item.outData.data_buf[0])
	{
#ifdef DEBUG_INFO
		static unsigned long prevTime;
		
		unsigned long curTime = voH264GetCurTime();
		if(img->ioInfo->outNumber == 0)
			prevTime = voH264GetCurTime();
		AvdLog2(LL_INFO,"%u,out_%d:%X,poc=%d,time=%u\n",curTime-prevTime,img->ioInfo->outNumber,item.outData.data_buf[0],item.outData.video_type,item.time);
		prevTime = curTime;
#endif//DEBUG_INFO

		pOutInfo->Format.Type =item.type;
		pOutput->ColorType=VO_COLOR_YUV_PLANAR420;
#if TEST_DEINTERLACE
		if (item.flag & VO_H264FLAG_DEINTERLACE)
		{
			int height = params->sizeInfo.frameHeight;
			int width  = params->sizeInfo.frameWidth;
			int stride = item.outData.stride[0];
			int i,j;
			if (params->deInterlaceOutBuf[0]==NULL)
			{
				//allocateDeInterlaceBuf(params,height,stride);
				int size = height*stride;
				params->deInterlaceOutBuf[0] = (avdUInt8*)voH264AlignedMalloc(img,999,size*3/2);
				params->deInterlaceOutBuf[1] = params->deInterlaceOutBuf[0]+size;
				params->deInterlaceOutBuf[2] = params->deInterlaceOutBuf[1]+size/4;
			}
			{
				VOCODECVIDEOBUFFER * par=&item.outData;
				char* buf  = (char*)par->data_buf[0];
				char* dest = (char*)params->deInterlaceOutBuf[0];
				for(i=0;i<height;i++,buf+=stride,dest+=stride)
				{
					memcpy(dest,buf,width);
				}	
				width	/=2;
				height	/=2;
				buf	=  (char*)par->data_buf[1];
				dest = (char*)params->deInterlaceOutBuf[1];
				for(i=0;i<height;i++,buf+=par->stride[1],dest+=par->stride[1])
				{
					memcpy(dest,buf,width); 
				}	

				buf	=  (char*)par->data_buf[2];
				dest = (char*)params->deInterlaceOutBuf[2];
				for(i=0;i<height;i++,buf+=par->stride[2],dest+=par->stride[2])
				{
					memcpy(dest,buf,width); 
				}	
			}
			AvdLog2(LL_INFO,"@Interlace deInterlace the ref frame\n");
			interpolate16(params->deInterlaceOutBuf,stride,params->sizeInfo.frameWidth,params->sizeInfo.frameHeight);
			pOutput->Buffer[0]=params->deInterlaceOutBuf[0];
			pOutput->Buffer[1]=params->deInterlaceOutBuf[1];
			pOutput->Buffer[2]=params->deInterlaceOutBuf[2];
		}
		else
#endif//USE_SEINTERLACE
		{
			pOutput->Buffer[0]=item.outData.data_buf[0];
			pOutput->Buffer[1]=item.outData.data_buf[1];
			pOutput->Buffer[2]=item.outData.data_buf[2];
		}
		
		pOutput->Stride[0]=item.outData.stride[0];
		pOutput->Stride[1]=item.outData.stride[1];
		pOutput->Stride[2]=item.outData.stride[2];
		pOutput->Time		= item.time;
		img->prevOutPicture = item.picture;
#if 1//ndef _WIN32_WINNT	

		img->licenseCheck->checkVideo(img->licenseCheck->hCheck,pOutput,&pOutInfo->Format);
		img->licenseCheck->lastVideo=(avdUInt8 *)pOutput;

#endif//_WIN32_WINNT	
		img->ioInfo->outNumber++;

	}
	else
	{
		pOutput->Buffer[0]=
			pOutput->Buffer[1]=
			pOutput->Buffer[2]=0;
		pOutInfo->Format.Type =VO_VIDEO_FRAME_NULL;
#ifndef MOTO
		if (ret==0)
		{
			ret = VO_ERR_INPUT_BUFFER_SMALL;
		}
#endif
	}
	pOutInfo->Format.Width  =  params->sizeInfo.frameWidth;
	pOutInfo->Format.Height =  params->sizeInfo.frameHeight;
	//AvdLog2(LL_INFO,"@output_%d,ret=%x(%d)\n",img->number,ret,ret);d

#if 	CALC_FPS
	end = GetTickCount();
	img->total_time += end - start; 	
	if ((((img->ioInfo->outNumber) % MAX_TEST_GAP) == 0) && (img->ioInfo->outNumber < MAX_TEST_FPS))
	{
//	 if((end-start) != 0)
//	     img->pfps[img->total_count++] = (double)1000 / (end-start);
	  img->pfps[img->total_count++] = ((double)img->ioInfo->outNumber * 1000 / (double)img->total_time);
	}
#endif

	return ret;
}
VO_U32	VO_API  voH264DecSetParam2(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
	VOH264DECRETURNCODE returnCode=0;
	GlobalH264Data* gData = (GlobalH264Data*)hDec;
	if(gData)
	{
		
			returnCode = voH264DecSetParameter(hDec,uParamID,(signed long)pData);
	}
	return returnCode;

}
VO_U32	VO_API  voH264DecGetParam2 (VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
	VOH264DECRETURNCODE returnCode=0;
	GlobalH264Data* gData = (GlobalH264Data*)hDec;
	if(gData)
	{
		returnCode = voH264DecGetParameter(hDec,uParamID,(signed long)pData);
	}
	return returnCode;
}
VO_U32	VO_API  voH264DecUninit2(VO_HANDLE hDec)
{
#if CALC_FPS    
	FILE* fp;
#ifdef	ANDROID
    char buff[512];
#else
    char buff[64];
#endif
#endif
	GlobalH264Data* gData = (GlobalH264Data*)hDec;
	ImageParameters *img;
	VO_U32 result=0;
	AvdLog2(LL_FLOW,"voH264DecUninit2 begin");
	if(gData==NULL)
		return VO_ERR_INVALID_ARG;
	img=gData->img;
	img->licenseCheck->unInit(img->licenseCheck->hCheck);
#if 	CALC_FPS
#ifndef ANDROID
#ifdef WIN32
	fp = fopen("d:/h264profile.txt","w");
#else
    sprintf(buff, "/data/local/h264profile_%dx%d.txt",  img->vdLibPar->sizeInfo.frameWidth, img->vdLibPar->sizeInfo.frameHeight);
	fp = fopen(buff,"w");
#endif
#endif
#ifdef ANDROID
    {
	    int i;
	    for( i = 1; i < img->vdLibPar->multiCoreNum; i++)
	    {
	       sprintf(buff, "Core %d job cnt %d, total job %d, %.2f%% wait data times %d wait decode times %d frame number %d \n", i, img->mcoreThreadParam[i]->job_cnt,   img->total_job,  
																					(double)(img->mcoreThreadParam[i]->job_cnt * 100) / (double)img->total_job, 
																					 img->mcoreThreadParam[i]->WaitDataTimes, img->WaitDecTimes, img->ioInfo->outNumber); 
		   voLogAndroidInfo(buff);
	    }
	  
	   for( i = 0; i < img->total_count; i++)
	   {
		   sprintf(buff, " ########### %.2f fps #################\n", img->pfps[i]); 	
		   voLogAndroidInfo(buff);
	   }
    }
#else
	if (fp)
	{
	  int i;
	  for( i = 1; i < img->vdLibPar->multiCoreNum; i++) 	
		 fprintf(fp, "Core %d job cnt %d, total job %d, %.2f%% wait data times %d wait decode times %d frame number %d \n", i, img->mcoreThreadParam[i]->job_cnt,	img->total_job,  
		                                                                              (double)(img->mcoreThreadParam[i]->job_cnt * 100) / (double)img->total_job, 
		                                                                               img->mcoreThreadParam[i]->WaitDataTimes, img->WaitDecTimes, img->ioInfo->outNumber); 
		
	  for( i = 0; i < img->total_count; i++)
		  fprintf(fp, " ########### %.2f fps #################\n", img->pfps[i]);	   
	  fclose(fp);
	}
#endif	
	if(img->pfps)
	  free(img->pfps);
#endif	
	
#if OUT_YUV	
	fclose(outFile);
#endif
#if USE_SEINTERLACE
	
		if (img->vdLibPar->deInterlaceOutBuf[0])
		{
			voH264AlignedFree(img,img->vdLibPar->deInterlaceOutBuf[0]);
			img->vdLibPar->deInterlaceOutBuf[0]=NULL;
		}
#endif//USE_SEINTERLACE
#if TEST_TIME 
		{
			FILE* fp = fopen("e:/h264profile.txt","w");
			if (fp)
			{
				TProfile* profile = &img->profile;
				int i;
				//read
				int readCBPSum=0;
				int readLumaDC16x16Sum=0;
				int readACSum=0;
				int readCDCSum=0;
				int iPredModeSum=0;
				int readCoefSum=0;
				int readMotionSum=0;
				int readIpModeSum=0;
				int readrunLevelSum=0;

				//dec
				int idctSum=0;
				int weightSum=0;
				int lmcSum=0;
				int cmcSum=0;
				int lmcSum2=0;
				int cmcSum2=0;
				int intra4x4Sum=0;
				int intra16Sum=0;
				int intra8x8Sum=0;
				int intracSum=0;

				//deblock
				int strcalSum=0;
				int filterSum=0;

				//summary
				int readSum=0;
				int readSliceSum=0;
				int decSum=0;
				int deblockSum=0;
				int total=0;
				char frameType[]={'P','B','I'};
				fprintf(fp,"profile=%d,size=(%d,%d),refNum=%d,interlace=%d,mbaff=%d\ncabac=%d,weight=%d,I=%d,P=%d,B=%d\n",
					profile->profile,profile->width,profile->height,profile->refNum,profile->interlace,profile->mbaff,
					profile->cabac,profile->weight,profile->INum,profile->PNum,profile->BNum);
				for(i=0;i<profile->curIdx;i++)
				{
					TFrameProfile* fprofile = &profile->list[i];
					fprintf(fp,"%d:%c,rs=%d,r=%d,dec=%d,db=%d,total=%d\n ",
						i,frameType[fprofile->frameType],fprofile->readSliceSum,fprofile->readSum,fprofile->decSum,fprofile->deblockSum,fprofile->total);
#define SUM2(a) a+=fprofile->a;
					SUM2(readCBPSum)
						SUM2(readLumaDC16x16Sum)
						SUM2(readACSum)
						SUM2(readCDCSum)
						SUM2(iPredModeSum)
						SUM2(readCoefSum)
						SUM2(readMotionSum)
						SUM2(readIpModeSum)
						SUM2(readrunLevelSum)

						//dec
						SUM2(idctSum)
						SUM2(weightSum)
						SUM2(lmcSum)
						SUM2(cmcSum)
						SUM2(lmcSum2)
						SUM2(cmcSum2)
						SUM2(intra4x4Sum)
						SUM2(intra16Sum)
						SUM2(intra8x8Sum)
						SUM2(intracSum)

						//deblock
						SUM2(strcalSum)
						SUM2(filterSum)

						//summary
						SUM2(readSum)
						SUM2(readSliceSum)
						SUM2(decSum)
						SUM2(deblockSum)
						SUM2(total)
				}
				//#define PRINT2(a,b)	printf(" " #a "=%f\n",a*100.0/b);
#define PRINT(a,b) fprintf(fp," " #a "=%f\n",a*100.0/b);
				PRINT(readCBPSum,readSum)
					PRINT(readLumaDC16x16Sum,readSum)
					PRINT(readACSum,readSum)
					PRINT(readCDCSum,readSum)
					PRINT(iPredModeSum,readSum)
					PRINT(readCoefSum,readSum)
					PRINT(readMotionSum,readSum)
					PRINT(readIpModeSum,readSum)
					PRINT(readrunLevelSum,readSum)

					//dec
					PRINT(idctSum,decSum)
					PRINT(weightSum,decSum)
					PRINT(lmcSum,decSum)
					PRINT(cmcSum,decSum)
					PRINT(lmcSum2,decSum)
					PRINT(cmcSum2,decSum)
					PRINT(intra4x4Sum,decSum)
					PRINT(intra16Sum,decSum)
					PRINT(intra8x8Sum,decSum)
					PRINT(intracSum,decSum)

					//deblock
					PRINT(strcalSum,deblockSum)
					PRINT(filterSum,deblockSum)

					//summary
					PRINT(readSum,total)
					PRINT(readSliceSum,total)
					PRINT(decSum,total)
					PRINT(deblockSum,total)
					PRINT(total,total)
					fclose(fp);
			}

		}

#endif//TEST_TIME
	result =  voH264DecUninit(hDec);
	AvdLog2(LL_FLOW,"voH264DecUninit2 end");

#if ENABLE_TRACE
	UninitLog2();
#endif//ENABLE_TRACE
}

#if !defined(COMBINE_ALL_PROFILE)


VO_S32 VO_API voGetH264DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	if(pDecHandle==NULL)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init					    = voH264DecInit2;
	pDecHandle->GetOutputData		=	voH264DecGetOutputData2;
	pDecHandle->SetInputData		=	voH264DecSetInputData2;
	pDecHandle->GetParam			=	voH264DecGetParam2;
	pDecHandle->SetParam			=	voH264DecSetParam2;
	pDecHandle->Uninit					=	voH264DecUninit2;
	return VO_ERR_NONE;
}

#endif//(COMBINE_ALL_PROFILE)






































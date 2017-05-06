#ifdef COMBINE_ALL_PROFILE
#include		"global.h"
#if !BUILD_WITHOUT_C_LIB
#include		<stdlib.h>
#include		<string.h>
#include		<assert.h>
#endif//BUILD_WITHOUT_C_LIB

#include		"h264dec.h"
#include		"voH264.h"

VO_U32  VO_API  voH264DecInit2 (VO_HANDLE * phDec,VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData);
VO_U32	VO_API  voH264DecSetInputData2 (VO_HANDLE hDec, VO_CODECBUFFER * pInput);
VO_U32	VO_API  voH264DecGetOutputData2 (VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo);
VO_U32	VO_API  voH264DecSetParam2(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData);
VO_U32	VO_API  voH264DecGetParam2 (VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData);
VO_U32	VO_API  voH264DecUninit2(VO_HANDLE hDec);

VO_U32  VO_API  voH264DecInit2BP (VO_HANDLE * phDec,VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData);
VO_U32	VO_API  voH264DecSetInputData2BP (VO_HANDLE hDec, VO_CODECBUFFER * pInput);
VO_U32	VO_API  voH264DecGetOutputData2BP (VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo);
VO_U32	VO_API  voH264DecSetParam2BP(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData);
VO_U32	VO_API  voH264DecGetParam2BP (VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData);
VO_U32	VO_API  voH264DecUninit2BP(VO_HANDLE hDec);
typedef struct{
	VO_VIDEO_CODINGTYPE	   vType;
	int						inited;
	VO_VIDEO_DECAPI			api;
	VO_CODEC_INIT_USERDATA userData;
	GlobalH264Data			handle;
}TInternalAPI;

static int GetProfile(int format,unsigned char* sequenceHead,int size)
{
	if(format==VO_H264_ANNEXB||format==VO_H264_ANNEXB_COMPLETE)
	{

		avdUInt8*	head = sequenceHead;
		avdUInt8*       end  = head + size;

		do {
			if (head[0]) {// faster if most of bytes are not zero;	
				head++;
				continue;
			}
			// find 00xx
			if (head[1]){
				head += 2; // skip two bytes;
				continue;
			}
			// find 0000xx
			switch(head[2]){
				case 0:
					//find 000000xx
					if (!head[3]){ // 00000000xx; just skip 1 avdUInt8;
						head++;
						continue; 
					}
					if (head[3] == 1)
					{
						if((head[4]&0x07)==0x07)
						{
							return head[5];
						}
					}

					head += 4; //skip four bytes and continue search;
					break;
				case 1:
					// find 000001 header;
					if((head[3]&0x07)==0x07)
					{
						return head[4];
					}

					head +=3; //skip three bytes and continue search;
					break;

				default: // other than 0, 1, 3;
					head += 3; // skip three bytes;
					break;
			} // switch;

		} while (head+3 < end);
	}
	else if(format==VO_H264_AVC)
	{
		char* pData = (char*)sequenceHead;
		int numOfPictureParameterSets;
		int configurationVersion	= pData[0];
		int AVCProfileIndication	= pData[1];//not relaiable
		int profile_compatibility	= pData[2];
		int AVCLevelIndication		= pData[3];
		int numOfSequenceParameterSets = pData[5]&0x1f;
		int actualProfile			= pData[9];
		return actualProfile;
	}
	else if((sequenceHead[0]&0x07)==0x07)//complete NALU:14496-15
	{
		return sequenceHead[1];
	}
	return 0;
}

static int initEngine(TInternalAPI *api2)
{
	VO_VIDEO_DECAPI* pDecHandle= &api2->api;
	GlobalH264Data* gData=&api2->handle;
	if(api2->inited)
		return 0;
	if(gData->currProfile==PRO_BASELINE)
	{
		pDecHandle->Init			    =   voH264DecInit2BP;
		pDecHandle->GetOutputData		=	voH264DecGetOutputData2BP;
		pDecHandle->SetInputData		=	voH264DecSetInputData2BP;
		pDecHandle->GetParam			=	voH264DecGetParam2BP;
		pDecHandle->SetParam			=	voH264DecSetParam2BP;
		pDecHandle->Uninit				=	voH264DecUninit2BP;
	}
	else
	{
		pDecHandle->Init			    =   voH264DecInit2;
		pDecHandle->GetOutputData		=	voH264DecGetOutputData2;
		pDecHandle->SetInputData		=	voH264DecSetInputData2;
		pDecHandle->GetParam			=	voH264DecGetParam2;
		pDecHandle->SetParam			=	voH264DecSetParam2;
		pDecHandle->Uninit				=	voH264DecUninit2;
	}


	gData = &api2->handle;
	api2->userData.reserved1 = gData->currProfile;
	api2->userData.reserved2 = PREALLOCATE_BUF;

	if(0==pDecHandle->Init(&gData,api2->vType,&api2->userData))
	{
		api2->inited = 1;
	}
	else
	{
		AvdLog2(LL_INFO,"fail to create h264 engine\n");
	}
	return !api2->inited;
}

VO_U32  VO_API  voH264DecInit3 (VO_HANDLE * phDec,VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{

	//AvdLog2(LL_INFO,"(TInternalAPI*)calloc(sizeof(TInternalAPI),1),%d\n",sizeof(TInternalAPI));
	TInternalAPI *api2 = (TInternalAPI*)calloc(sizeof(TInternalAPI),1);
	//AvdLog2(LL_INFO,"after (TInternalAPI*)calloc(sizeof(TInternalAPI),1),%d\n",sizeof(TInternalAPI));
	if(api2)
	{
		api2->vType = vType;
		//AvdLog2(LL_INFO,"before calloc2,%X\n",pUserData);
		api2->handle.params = (H264VdLibParam*)calloc(sizeof(H264VdLibParam),1);
		api2->handle.params->enableOutputASAP = 1;
		api2->handle.params->enableReuseValidSPS = 0;//set it 1 by default
#ifndef TRACE_MEM//for trace mem, disable the log to trace more issues
		api2->handle.params->enableJump2I	  = 1;
#endif//
#if   SETAFFINITY
        api2->handle.params->for_unittest = 0;
#endif
		if(pUserData)
			memcpy(&api2->userData,pUserData,sizeof(VO_CODEC_INIT_USERDATA));
	}
	*phDec = api2;
#if ENABLE_TRACE
	InitLog2();
#endif//ENABLE_TRACE
	return 0;
}
VO_U32	VO_API  voH264DecSetInputData3 (VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{

	TInternalAPI *api2 = (TInternalAPI*)hDec;
	if(api2->inited==0)
	{
		api2->handle.currProfile = GetProfile(api2->handle.params->fileFormat,pInput->Buffer,pInput->Length);
		if(api2->handle.currProfile)
		{
			initEngine(api2);
		}
	}
	if(api2->inited)
	{
		return api2->api.SetInputData(&(api2->handle),pInput);
	}
	else
		return 0;
}
VO_U32	VO_API  voH264DecGetOutputData3 (VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo)
{


	TInternalAPI *api2 = (TInternalAPI*)hDec;
	if(api2&&api2->api.GetOutputData &&api2->inited)
	{
		return api2->api.GetOutputData(&(api2->handle),pOutput,pOutInfo);
	}
	else
	{
		memset(pOutput,0,sizeof(VO_VIDEO_BUFFER));
		return VO_ERR_INPUT_BUFFER_SMALL;
	}


}

VO_U32	VO_API  voH264DecSetParam3(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR lValue)
{
	TInternalAPI *api2 = (TInternalAPI*)hDec;
	VO_U32 ret = 0;
	if(api2)
	{
		if(api2->inited==0)
		{

			H264VdLibParam *inPa;
			inPa = api2->handle.params;
			switch(uParamID)
			{
			case VO_ID_H264_AVCCONFIG:
				/*{
					unsigned char* config= (unsigned char*)lValue;
					api2->handle.currProfile = config[1];
					break;
				}		*/			
			case VO_PID_COMMON_HEADDATA:
			case VO_ID_H264_SEQUENCE_PARAMS:
			case VO_ID_H264_PICTURE_PARAMS:
				{
					VOCODECDATABUFFER* params=  (VOCODECDATABUFFER*) lValue;
					api2->handle.currProfile = GetProfile(api2->handle.params->fileFormat,params->buffer,params->length);
				}
				break;
			default:
				ret = InternalSetParam(api2->handle.img,inPa,uParamID,(void*)lValue);
				break;

			}

			if(api2->handle.currProfile)
			{
				initEngine(api2);
			}

		}

		if(api2->inited)
			return api2->api.SetParam(&(api2->handle),uParamID,lValue);
		else
			return ret;

	}
	else
		return 0;

}
VO_U32	VO_API  voH264DecGetParam3 (VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
	TInternalAPI *api2 = (TInternalAPI*)hDec;
	if(api2&&api2->api.GetParam &&api2->inited)
	{
		return api2->api.GetParam(&(api2->handle),uParamID,pData);
	}
	else
		return 0;
}
#define SafeFree(a) if(a) free(a);a=NULL;

VO_U32	VO_API  voH264DecUninit3(VO_HANDLE hDec)
{
	TInternalAPI *api2 = (TInternalAPI*)hDec;
	if(api2)
	{
		if(api2->api.Uninit &&api2->inited)
		{
			api2->api.Uninit(&(api2->handle));
			SafeFree(api2->handle.params->customMem2);
			SafeFree(api2->handle.params->sharedMem);
			SafeFree(api2->handle.params->sharedMemInfo);
		}
		
		SafeFree(api2->handle.params);
		SafeFree(api2);
	}
	return 0;
}

VO_S32 VO_API voGetH264DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	if(pDecHandle==NULL)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init					    = voH264DecInit3;
	pDecHandle->GetOutputData		=	voH264DecGetOutputData3;
	pDecHandle->SetInputData		=	voH264DecSetInputData3;
	pDecHandle->GetParam			=	voH264DecGetParam3;
	pDecHandle->SetParam			=	voH264DecSetParam3;
	pDecHandle->Uninit					=	voH264DecUninit3;
	return VO_ERR_NONE;
}

#endif//COMBINE_ALL_PROFILE
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2012		*
*																		*
************************************************************************/
/*******************************************************************************
	File:		voIOMXDec.cpp

	Contains:	VisualOn IOMX decoder source file

	Written by:	East Zhou

	Change History (most recent first):
	2012-04-20	East		Create file

*******************************************************************************/
#include "voIOMXDec.h"
#include "CvoIOMXComponent.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <CvoIOMXData.h>

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "voIOMXDec"
#endif

#include "voLog.h"
VO_PTR g_hInst = NULL;
#ifdef _WIN32
#include <windows.h>
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
        g_hInst = (VO_PTR) hModule;
        return TRUE;
}
#endif // _WIN32

#define VOME_CHECK_HANDLE if(hDec == NULL)\
	return VO_ERR_INVALID_ARG;\
	CvoIOMXComponent * pDec = (CvoIOMXComponent *)hDec;

VO_U32 IOMXDecInit(VO_HANDLE * phDec, VO_VIDEO_CODINGTYPE eVideoCodec, VO_CODEC_INIT_USERDATA * pUserData)
{
        OMX_VIDEO_CODINGTYPE eCodec = OMX_VIDEO_CodingUnused;
        if(eVideoCodec == VO_VIDEO_CodingMPEG4)
                eCodec = OMX_VIDEO_CodingMPEG4;
        else if(eVideoCodec == VO_VIDEO_CodingH264)
                eCodec = OMX_VIDEO_CodingAVC;
        else
        {
                VOLOGE("currently we just support mpeg4, avc. video codec %d", eVideoCodec);
                return OMX_ErrorNotImplemented;
        }

		if(NULL != pUserData)
		{
			VOLOGINIT((char*)(pUserData->reserved1));
		}
		CvoIOMXComponent * pDec = new CvoIOMXComponent(eCodec);
        if(pDec == NULL)
                return VO_ERR_OUTOF_MEMORY;

        VO_U32 nRC = pDec->Init();
        if(VO_ERR_NONE != nRC)
        {
                delete pDec;
                return nRC;
        }

        *phDec = pDec;
        return VO_ERR_NONE;
}

VO_U32 IOMXDecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
        VOME_CHECK_HANDLE

        return pDec->SetInputData(pInput);
}

VO_U32 IOMXDecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
        VOME_CHECK_HANDLE

        return pDec->GetOutputData(pOutBuffer, pOutInfo);
}

VO_U32 IOMXDecUninit(VO_HANDLE hDec)
{
        VOME_CHECK_HANDLE

		VOLOGUNINIT();
        pDec->Uninit();
        delete pDec;
        return VO_ERR_NONE;
}

int voIOMXProbe(VO_PTR pData)
{
        OMX_VIDEO_CODINGTYPE eCodec = OMX_VIDEO_CodingAVC;
        CvoIOMXComponent * pDec = new CvoIOMXComponent(eCodec);

        if(pDec == NULL)
                return VO_ERR_OUTOF_MEMORY;
        pDec->m_bProbeMode = OMX_TRUE;
        pDec->Init();

        /*
         * Set video format
         */
        VO_VIDEO_FORMAT m_format;
        m_format.Width = 176;
        m_format.Height = 144;
        pDec->SetParam(VO_PID_VIDEO_FORMAT,&m_format);

        pDec->SetParam(VO_PID_IOMXDEC_SetSurface,pData);

        /*
         * Set sequence head
         */
        VO_CODECBUFFER headData;
        headData.Buffer = head_data;
        headData.Length = sizeof(head_data);
        pDec->SetParam(VO_PID_COMMON_HEADDATA,&headData);

        /*
         * Set inputdata always on key frame;
         */
        VO_CODECBUFFER pInput;
        pInput.Buffer = key_frame;
        pInput.Length = sizeof(key_frame);


        /*
         * Set 16 times key frame
         */

        for(int i = 0; i < 16; i ++)
        {
                pDec->SetInputData(&pInput);
        }
        /*
         * Get outputdata try 10 times
         */
        VO_VIDEO_BUFFER out_buffer;
        VO_VIDEO_OUTPUTINFO out_info;
        int result;
        for(int j = 0; j < 10; j ++)
        {
                pDec->SetInputData(&pInput);
                result =  pDec->GetOutputData(&out_buffer,&out_info); //return buffer success and return 0
                if(result == OMX_ErrorNone)
                {
                	IOMXDecUninit(pDec);
                        return 0;
                }
        }
        if(result != OMX_ErrorNone)
        {
        	IOMXDecUninit(pDec);
                return -1; //else return  -1
        }

        pDec->Uninit();
        delete pDec;
        return -1;
}

VO_U32 IOMXDecSetParam(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
        VOME_CHECK_HANDLE

        if( uParamID == VO_PID_IOMXDEC_PROBE /* || uParamID == VO_PID_IOMXDEC_SetSurface*/)
        {
                return voIOMXProbe(pData);
                IOMXDecUninit(pDec);
        }
        else 
                return pDec->SetParam(uParamID, pData);
}

VO_U32 IOMXDecGetParam(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
        VOME_CHECK_HANDLE

        return pDec->GetParam(uParamID, pData);
}

VO_S32 voGetIOMXDecAPI(VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
        pDecHandle->Init			= IOMXDecInit;
        pDecHandle->SetInputData	= IOMXDecSetInputData;
        pDecHandle->GetOutputData	= IOMXDecGetOutputData;
        pDecHandle->SetParam		= IOMXDecSetParam;
        pDecHandle->GetParam		= IOMXDecGetParam;
        pDecHandle->Uninit			= IOMXDecUninit;

        return VO_ERR_NONE;
}

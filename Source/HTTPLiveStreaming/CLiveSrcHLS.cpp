	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CLiveSrcHLS.cpp

	Contains:	CLiveSrcHLS class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-11-01		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "CLiveSrcHLS.h"
#include "voOSFunc.h"
#include "cmnFile.h"
#include "voLog.h"
#include "voLiveSource.h"
#include "vompType.h"

//#pragma warning (disable : 4996)

#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

_CrtMemState s1;
_CrtMemState s2;
_CrtMemState s3;
char OutputStr[256];
#endif	//_WIN32


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CLiveSrcHLS::CLiveSrcHLS(void)
	: CLiveSrcBase ()
	, m_pHLS (NULL)
{
#ifdef _WIN32
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtMemCheckpoint( &s1 );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
#endif
	memset (&m_sVideoSample, 0, sizeof (VO_LIVESRC_SAMPLE));
	memset (&m_sAudioSample, 0, sizeof (VO_LIVESRC_SAMPLE));
	m_sAudioSample.nTrackID = 1;
}

CLiveSrcHLS::~CLiveSrcHLS(void)
{
	if (m_pHLS != NULL)
	{
		m_pHLS->close ();
		delete m_pHLS;
		m_pHLS = NULL;
	}
#ifdef _WIN32
	memset(OutputStr, 0, 256);
	_CrtMemCheckpoint( &s2 );
	if ( _CrtMemDifference( &s3, &s1, &s2) )
		_CrtMemDumpStatistics( &s3 );
	sprintf(OutputStr,"ThreadID: %d\n",GetCurrentThreadId());
	OutputDebugStringA(OutputStr);
#endif
}


VO_U32 CLiveSrcHLS::Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{

#ifdef _HLS_SOURCE2_
	return VO_ERR_LIVESRC_OK;
#else
	Close ();

	m_pUserData = m_pOrigUserData = pUserData;
	m_fStatus	= fStatus;
	m_fSendData = fSendData;

	m_pHLS = new vo_http_live_streaming ();
	m_pHLS->set_datacallback (this, SendOutData);
	m_pHLS->set_eventcallback (this, NotifyEvent);

	return VO_ERR_LIVESRC_OK;
#endif
}

VO_U32 CLiveSrcHLS::Close()
{
	if (m_pHLS != NULL)
	{
		m_pHLS->close ();
		delete m_pHLS;
		m_pHLS = NULL;
	}

	return VO_ERR_LIVESRC_OK;
}

VO_U32 CLiveSrcHLS::Start()
{
	if (m_pHLS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	m_pHLS->run ();

	return VO_ERR_LIVESRC_OK;
}

VO_U32 CLiveSrcHLS::Pause()
{
	if (m_pHLS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	m_pHLS->pause ();

	return VO_ERR_LIVESRC_OK;
}

VO_U32 CLiveSrcHLS::Stop()
{
	if (m_pHLS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	m_pHLS->close();

	return VO_ERR_LIVESRC_OK;
}

VO_U32 CLiveSrcHLS::SetDataSource(VO_PTR pSource, VO_U32 nFlag)
{
	if (m_pHLS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	VO_BOOL bRC = m_pHLS->open ((VO_CHAR *)pSource, (VO_BOOL)nFlag);

	return bRC == VO_TRUE ? VO_ERR_LIVESRC_OK : VO_ERR_LIVESRC_FAIL;
}

VO_U32 CLiveSrcHLS::Seek(VO_U64 nTimeStamp)
{
	if (m_pHLS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

	m_pHLS->set_pos ((VO_S32)nTimeStamp);

	return VO_ERR_LIVESRC_OK;
}

VO_U32 CLiveSrcHLS::GetParam(VO_U32 nParamID, VO_PTR pValue)
{
    VO_U32    ulRet = VO_ERR_LIVESRC_OK;
	if (m_pHLS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;


    switch(nParamID)
    {
        case VO_PID_LIVESRC_SOURCE_TYPE:
		{
			if (m_pHLS->is_live ())
			{
			    *(int *)pValue = 1;
			}
		    else
			{
			    *(int *)pValue = 0;
		    }

			break;
		}
		case VO_PID_LIVESRC_DURATION:
		{
			*(int *)pValue = m_pHLS->get_duration ();
			break;
		}
		case VO_PID_LIVESRC_TRACKNUMBER:
		{
		    *(int *)pValue = 2;
			break;
		}
		default:
		{
			ulRet = CLiveSrcBase::GetParam (nParamID, pValue);
			break;
		}
    }

	return ulRet;
}

VO_U32 CLiveSrcHLS::SetParam(VO_U32 nParamID, VO_PTR pValue)
{
    VO_U32   ulRet = 0;
	VOLOGI("SetParam  nParamID:%d", nParamID);
	
	if (m_pHLS == NULL)
		return VO_ERR_LIVESRC_WRONG_STATUS;

    switch(nParamID)
    {
        case VO_PID_LIVESRC_HLS_DRM:
		{
			m_pHLS->set_DRM (pValue);			
			break;
		}
		case VO_PID_LIVESRC_VIDEODELAY:
		{
			m_pHLS->set_videodelay( (int *)pValue );
			break;
		}
		case VO_PID_LIVESRC_LIBOP:
		{
			m_pHLS->set_libop((void *) pValue);
			break;
		}
		case VO_PID_LIVESRC_HLS_AD_FILTER:
		{
			m_pHLS->Add_AdFilterInfo((void *) pValue);
			break;
		}
		case VO_PID_LIVESRC_WORKPATH:
		{
			m_pHLS->setWorkPath((const char *) pValue);
			break;
		}
		case VO_PID_LIVESRC_CPU_ABILITY:
		{
			m_pHLS->setCpuInfo(pValue );
			break;
		}
		case VO_PID_LIVESRC_CAP_PROFILE:
		{
			m_pHLS->setCapInfo(pValue);
			break;
		}

        case VO_PID_LIVESRC_USERNAME:
        {
            m_pHLS->SetUserName((VO_VOID*)pValue);
            break;            
        }
        case VO_PID_LIVESRC_PASSWD:
        {
            m_pHLS->SetUserPassWd((VO_VOID*)pValue);
			break;			  
		}
		default:
		{
			ulRet = CLiveSrcBase::SetParam (nParamID, pValue);
			break;
		}

    }

	return ulRet;
}

VO_VOID	CLiveSrcHLS::SendOutData (VO_VOID * pUserData, VO_LIVESRC_SAMPLE * ptr_sample)
{
	CLiveSrcHLS * pHLS = (CLiveSrcHLS *)pUserData;

	pHLS->m_fSendData (pHLS->m_pUserData, (VO_U16)ptr_sample->nTrackID , ptr_sample );
}

VO_VOID	CLiveSrcHLS::NotifyEvent (VO_VOID * pUserData, Event * pEvent)
{
	CLiveSrcHLS * pHLS = (CLiveSrcHLS *)pUserData;

	switch( pEvent->id )
	{
	case VO_LIVESRC_STATUS_MEDIATYPE_CHANGE:
		pHLS->m_fStatus( pHLS->m_pUserData , VO_LIVESRC_STATUS_MEDIATYPE_CHANGE , pEvent->param1 , 0 );
		break;
	case VO_LIVESRC_STATUS_DRM_ERROR:
		pHLS->m_fStatus( pHLS->m_pUserData , VO_LIVESRC_STATUS_DRM_ERROR , pEvent->param1 , 0 );
		break;
	case VO_LIVESRC_STATUS_HSL_AD_APPLICATION:
		//Filter ID in param2
		pHLS->m_fStatus( pHLS->m_pUserData , VO_LIVESRC_STATUS_HSL_AD_APPLICATION, (VO_U32)pEvent->param2, pEvent->param1);
		break;
	case VO_LIVESRC_STATUS_HSL_FRAME_SCRUB:
		//Filter thumbnailinfo list in param1, thumbnail count in parma2
		pHLS->m_fStatus( pHLS->m_pUserData , VO_LIVESRC_STATUS_HSL_FRAME_SCRUB, (VO_U32)pEvent->param1, pEvent->param2);
		break;        
    case VO_LIVESRC_STATUS_HSL_CHANGE_BITRATE:
        //Notify the Bitrate Change
        pHLS->m_fStatus( pHLS->m_pUserData , VO_LIVESRC_STATUS_HSL_CHANGE_BITRATE, (VO_U32)pEvent->param1, pEvent->param2);
        break;
    case VO_LIVESRC_STATUS_HSL_EVENT_THUMBNAIL:
        //Notify the Bitrate Change
        pHLS->m_fStatus( pHLS->m_pUserData , VO_LIVESRC_STATUS_HSL_EVENT_THUMBNAIL, (VO_U32)pEvent->param1, pEvent->param2);
        break;        
	}
}

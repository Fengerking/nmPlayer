	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CExtVOMEPlayer.cpp

	Contains:	CExtVOMEPlayer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-08-31		JBF			Create file

*******************************************************************************/

#include <stdio.h>
#include <string.h>

#define LOG_TAG "CExtVOMEPlayer"

#include "voOSFunc.h"
#include "CExtVOMEPlayer.h"

#include "voLog.h"

// ----------------------------------------------------------------------------

VO_VOID DataCallback( VO_VOID * ptr_obj , OMX_BUFFERHEADERTYPE * ptr_buffer );

 VO_CHAR * testurl[] = {"http://devimages.apple.com/iphone/samples/bipbop/gear1/prog_index.m3u8" , 
 						"http://devimages.apple.com/iphone/samples/bipbop/gear2/prog_index.m3u8" ,
 						"http://devimages.apple.com/iphone/samples/bipbop/gear3/prog_index.m3u8" ,
 						"http://devimages.apple.com/iphone/samples/bipbop/gear4/prog_index.m3u8" };

CExtVOMEPlayer::CExtVOMEPlayer(void)
	: CVOMEPlayer ()
	, m_pCompSrc (NULL)
	, m_pCompAD (NULL)
	, m_pCompVD (NULL)
	, m_pCompAR (NULL)
	, m_pCompVR (NULL)
	, m_pCompClock (NULL)
	, m_hThreadMedia (NULL)
	, m_bMediaStop (false)
{
}

CExtVOMEPlayer::~CExtVOMEPlayer()
{
	Uninit ();
}

int CExtVOMEPlayer::BuildGraph (void)
{
	if (m_nStatus >= 0)
		return 0;

	if (m_nSourceType != 2)
		return CVOMEPlayer::BuildGraph ();

	//char * pExt = strrchr (m_szFile, '.');
	//if (strstr (pExt, "m3u") == NULL)
		//return CVOMEPlayer::BuildGraph ();

	//VOLOGE("%s", m_szFile);
	if( strstr( m_szFile , ".m3u" ) == NULL )
	{
		//VOLOGE("shit");
		return CVOMEPlayer::BuildGraph ();
	}

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	errType = AddComponent ("OMX.VisualOn.DataSource", &m_pCompSrc);
	if (errType != OMX_ErrorNone)
		return errType;

	VOLOGE( "AddComponent OK!" );

	errType = AddComponent ("OMX.VisualOn.Clock.Time", &m_pCompClock);

	errType = AddComponent ("OMX.VisualOn.Audio.Decoder.XXX", &m_pCompAD);
	errType = AddComponent ("OMX.VisualOn.Audio.Sink", &m_pCompAR);

	errType = AddComponent ("OMX.VisualOn.Video.Decoder.XXX", &m_pCompVD);
	errType = AddComponent ("OMX.VisualOn.Video.Sink", &m_pCompVR);

	errType = ConnectPorts (m_pCompSrc, 0, m_pCompAD, 0, OMX_TRUE);
	errType = ConnectPorts (m_pCompAD, 1, m_pCompAR, 0, OMX_TRUE);

	errType = ConnectPorts (m_pCompSrc, 1, m_pCompVD, 0, OMX_TRUE);
	errType = ConnectPorts (m_pCompVD, 1, m_pCompVR, 0, OMX_TRUE);

	errType = ConnectPorts (m_pCompClock, 0, m_pCompSrc, 2, OMX_TRUE);
	errType = ConnectPorts (m_pCompClock, 1, m_pCompVR, 1, OMX_TRUE);
	errType = ConnectPorts (m_pCompClock, 2, m_pCompAR, 1, OMX_TRUE);

	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkRef;
	clkRef.eClock = OMX_TIME_RefClockAudio;
//	clkRef.eClock = OMX_TIME_RefClockVideo;
	m_pCompClock->SetConfig (m_pCompClock, OMX_IndexConfigTimeActiveRefClock, &clkRef);

	m_bSeeking = false;
	m_nSourceType = 0;
	m_nStatus = 0;
	m_bSourceLoading = false;

	m_bMediaStop = false;
	if (m_hThreadMedia == NULL)
		voOMXThreadCreate (&m_hThreadMedia, &m_nMediaThreadID, (voOMX_ThreadProc)ReadMediaDataProc, this, 0);

	return 0;
}

int CExtVOMEPlayer::Run (void)
{
	int nRC = CVOMEPlayer::Run ();

	return nRC;
}

int CExtVOMEPlayer::Pause (void)
{
	int nRC = CVOMEPlayer::Pause ();

	return nRC;
}

int CExtVOMEPlayer::Stop (void)
{
	VOLOGE("Stop");
	m_streaming.close();
	int nRC = CVOMEPlayer::Stop ();

	return nRC;
}

int CExtVOMEPlayer::SetPos (int	nPos)
{
	int nRC = 0;//CVOMEPlayer::SetPos (nPos);

	return nRC;
}

int CExtVOMEPlayer::GetPos (int * pPos)
{
	int nRC = 0;//CVOMEPlayer::GetPos (pPos);

	return nRC;
}

int CExtVOMEPlayer::GetDuration (int * pPos)
{
	int nRC = 0;//CVOMEPlayer::GetDuration (pPos);

	return nRC;
}

int CExtVOMEPlayer::Reset (void)
{
	VOLOGE("Reset");
	int nRC = CVOMEPlayer::Reset ();

	return nRC;
}

int CExtVOMEPlayer::Uninit (void)
{
	VOLOGE("Uninit");
	m_streaming.close();
	int nRC = CVOMEPlayer::Uninit ();

	return nRC;
}

OMX_ERRORTYPE CExtVOMEPlayer::EnumComponentName (OMX_STRING pCompName, OMX_U32 nIndex)
{
	if (m_hPlay == NULL)
		return OMX_ErrorInvalidState;

	return m_fComp.EnumComponentName (m_hPlay, pCompName, nIndex);
}

OMX_ERRORTYPE CExtVOMEPlayer::GetRolesOfComponent (OMX_STRING pCompName, OMX_U32 * pNumRoles, OMX_U8 ** ppRoles)
{
	if (m_hPlay == NULL)
		return OMX_ErrorInvalidState;

	return m_fComp.GetRolesOfComponent (m_hPlay, pCompName, pNumRoles, ppRoles);
}

OMX_ERRORTYPE CExtVOMEPlayer::GetComponentsOfRole (OMX_STRING pRole, OMX_U32 * pNumComps, OMX_U8 ** ppCompNames)
{
	if (m_hPlay == NULL)
		return OMX_ErrorInvalidState;

	return m_fComp.GetComponentsOfRole (m_hPlay, pRole, pNumComps, ppCompNames);
}

OMX_ERRORTYPE CExtVOMEPlayer::AddComponent (OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent)
{
	if (m_hPlay == NULL)
		return OMX_ErrorInvalidState;

	return m_fComp.AddComponent (m_hPlay, pName, ppComponent);
}

OMX_ERRORTYPE CExtVOMEPlayer::RemoveComponent (OMX_COMPONENTTYPE * pComponent)
{
	if (m_hPlay == NULL)
		return OMX_ErrorInvalidState;

	return m_fComp.RemoveComponent (m_hPlay, pComponent);
}

OMX_ERRORTYPE CExtVOMEPlayer::ConnectPorts (OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
										OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel)
{
	if (m_hPlay == NULL)
		return OMX_ErrorInvalidState;

	return m_fComp.ConnectPorts (m_hPlay, pOutputComp, nOutputPort, pInputComp, nInputPort, bTunnel);
}

int CExtVOMEPlayer::ReadMediaDataProc (OMX_PTR pParam)
{
	CExtVOMEPlayer * pPlayer = (CExtVOMEPlayer *) pParam;

	pPlayer->ReadMediaDataLoop ();

	return 0;
}

int CExtVOMEPlayer::ReadMediaDataLoop (void)
{
	m_streaming.close();
	m_streaming.set_datacallback( this , DataCallback );
	m_streaming.open( m_szFile , VO_TRUE );

	return 1;
}

VO_VOID CExtVOMEPlayer::data_callback( OMX_BUFFERHEADERTYPE * ptr_buffer )
{
	OMX_ERRORTYPE errType = OMX_ErrorUndefined;
	VO_S32 trycount = 0;

	while (errType != OMX_ErrorNone && trycount < 5 )
	{
		errType = m_pCompSrc->SetParameter (m_pCompSrc, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, ptr_buffer);

		if (errType != OMX_ErrorNone)
		{
			voOS_Sleep (200);
			trycount++;
		}
	}

	if( trycount >= 5 )
		VOLOGE( "Frame Drop!" );
}


VO_VOID DataCallback( VO_VOID * ptr_obj , OMX_BUFFERHEADERTYPE * ptr_buffer )
{
	CExtVOMEPlayer * ptr_player = ( CExtVOMEPlayer * )ptr_obj;

	ptr_player->data_callback( ptr_buffer );
}


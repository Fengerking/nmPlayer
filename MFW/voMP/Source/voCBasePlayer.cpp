	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCBasePlayer.cpp

	Contains:	voCBasePlayer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifdef __SYMBIAN32__
#include <stdlib.h>
#elif defined _IOS
#include <stdlib.h>
#elif defined _MAC_OS
#include <stdlib.h>
#else
#include <malloc.h>
#endif // __SYMBIAN32__

#include <string.h>
#include <stdio.h>

#include "voAudio.h"
#include "voVideo.h"
#include "voFile.h"

#include "voCBasePlayer.h"

#define LOG_TAG "voCBasePlayer"
#include "voLog.h"

voCBasePlayer::voCBasePlayer(VOMPListener pListener, void * pUserData)
	: m_pListener (pListener)
	, m_pUserData (pUserData)
	, m_hInst (NULL)
	, m_fThreadCreate (NULL)
{
	memset(&m_cbVOLOG, 0, sizeof(VO_LOG_PRINT_CB));
}

voCBasePlayer::~voCBasePlayer(void)
{

}

int voCBasePlayer::SetDataSource (void * pSource, int nFlag)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::SendBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::GetVideoBuffer (VOMP_BUFFERTYPE ** ppBuffer)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::GetAudioBuffer (VOMP_BUFFERTYPE ** ppBuffer)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::Run (void)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::Pause (void)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::Stop (void)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::Flush (void)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::GetStatus (VOMP_STATUS * pStatus)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::GetDuration (int * pDuration)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::GetCurPos (int * pCurPos)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::SetCurPos (int nCurPos)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::GetParam (int nID, void * pValue)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::SetParam (int nID, void * pValue)
{
	return VOMP_ERR_Implement;
}

int voCBasePlayer::CloseSource (bool bSync)
{
	return VOMP_ERR_Implement;
}

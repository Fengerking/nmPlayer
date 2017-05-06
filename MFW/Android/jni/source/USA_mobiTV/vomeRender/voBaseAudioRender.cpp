
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		voBaseAudioRender.cpp

Contains:	voBaseAudioRender class file

Written by:	Tom Yu Wei 

Change History (most recent first):
2010-08-31		Tom Yu			Create file

*******************************************************************************/


#include <cutils/properties.h>

#include "voBaseAudioRender.h"

#define LOG_TAG "VOBaseAudioRender"


namespace android {

VOBaseAudioRender::VOBaseAudioRender()
	: m_bOpen (false)
	, m_nStreamType (AudioSystem::MUSIC)
	, m_dLeftVolume (1.0)
	, m_dRightVolume (1.0)
{
}

VOBaseAudioRender::~VOBaseAudioRender()
{
	close();
}

void VOBaseAudioRender::close()
{
    m_bOpen = false;
}

}
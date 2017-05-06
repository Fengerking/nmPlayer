	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompCLivePlayer.h

	Contains:	vompCLivePlayer header file

	Written by:	Jeff Huang

	Change History (most recent first):
	2011-08-30		Jeff		Create file

*******************************************************************************/

#ifndef __vompCLivePlayer_H__
#define __vompCLivePlayer_H__

#include "vompCPlayer.h"

class vompCLivePlayer : public vompCPlayer
{
public:
	// Used to control the image drawing
	vompCLivePlayer (VO_PLAYER_OPEN_PARAM* pParam);
	virtual ~vompCLivePlayer (void);
	
	virtual int initVomp();
	
	virtual int SendBuffer (VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer);
};

#endif // __vompCLivePlayer_H__

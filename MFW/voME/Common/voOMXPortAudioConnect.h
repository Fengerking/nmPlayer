	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXPortAudioConnect.h

	Contains:	voOMXPortAudioConnect header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voOMXPortAudioConnect_H__
#define __voOMXPortAudioConnect_H__

OMX_ERRORTYPE	voOMXConnectAudioPort (OMX_COMPONENTTYPE * pPrev, OMX_U32 nOutputPort, 
										OMX_COMPONENTTYPE * pNext, OMX_U32 nInputPort);

#endif //__voOMXPortAudioConnect_H__

	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXPortVideoConnect.h

	Contains:	voOMXPortVideoConnect header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voOMXPortVideoConnect_H__
#define __voOMXPortVideoConnect_H__

OMX_ERRORTYPE	voOMXConnectVideoPort (OMX_COMPONENTTYPE * pPrev, OMX_U32 nOutputPort, 
										OMX_COMPONENTTYPE * pNext, OMX_U32 nInputPort);

#endif //__voOMXPortVideoConnect_H__

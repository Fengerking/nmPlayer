/*
 *  voCTS.cpp
 *  voCTS
 *
 *  Created by Lin Jun on 4/21/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "volspAPI.h"
#include "CLSPlayer.h"

#define PLAYER_OBJ ((CLSPlayer*)hPlayer)

/**
 * Initialize a Player session.
 * This function should be called first for a session.
 * \param phPlayer [out] PLAYER handle.
 * \param pOpenParam [in] PLAYER open parameters
 */
VO_U32 voplayerOpen(VO_HANDLE* phPlayer, VO_PLAYER_OPEN_PARAM* pOpenParam)
{
	CLSPlayer* pPalyer = new CLSPlayer;
	*phPlayer			= pPalyer;
	return pPalyer->Open(pOpenParam);
}


/**
 * Close the PLAYER session.
 * This function should be called last of the session.
 * \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
 */
VO_U32 voplayerClose(VO_HANDLE hPlayer)
{
	if(!PLAYER_OBJ)
		return VO_ERR_PLAYER_NULL_POINTER;
	
	VO_U32 nRet = PLAYER_OBJ->Close();
	delete PLAYER_OBJ;
	
	return nRet;
}

/**
 * Set the video draw area in the view window
 * \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
 * \param nLeft [in] The left position of draw video area
 * \param nTop [in] The top position of draw video area
 * \param nRight [in] The right position of draw video area
 * \param nBottom [in] The bottom position of draw video area
 */
VO_U32 voplayerSetDrawArea(VO_HANDLE hPlayer, VO_U16 nLeft, VO_U16 nTop, VO_U16 nRight, VO_U16 nBottom)
{
	if(!PLAYER_OBJ)
		return VO_ERR_PLAYER_NULL_POINTER;
	
	return PLAYER_OBJ->SetDrawArea(nLeft, nTop, nRight, nBottom);
}


/**
 * Set source URL
 * \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
 * \param pSource [in] The Data source URL
 */
VO_U32 voplayerSetDataSource (VO_HANDLE hPlayer, VO_PTR pSource)
{
	if(!PLAYER_OBJ)
		return VO_ERR_PLAYER_NULL_POINTER;
	
	return PLAYER_OBJ->SetDataSource(pSource);
}

/**
 * Send the buffer into player
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param bVideo [in] indicator of buffer type
 * \param pBuffer [in] It is audio or video stream buffer.
 * \retval VO_ERR_PLAYER_OK Succeeded
 */
VO_U32 voplayerSendBuffer (VO_HANDLE hPlayer, VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer)
{
	if(!PLAYER_OBJ)
		return VO_ERR_PLAYER_NULL_POINTER;
	
	return PLAYER_OBJ->SendBuffer(eInputType, pBuffer);	
}


/**
 * start to play
 * \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
 */
VO_U32 voplayerRun(VO_HANDLE hPlayer)
{
	if(!PLAYER_OBJ)
		return VO_ERR_PLAYER_NULL_POINTER;
	
	return PLAYER_OBJ->Run();
}


/**
 * pause
 * \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
 */
VO_U32 voplayerPause(VO_HANDLE hPlayer)
{
	if(!PLAYER_OBJ)
		return VO_ERR_PLAYER_NULL_POINTER;
	
	return PLAYER_OBJ->Pause();
}

/**
 * stop playing
 * \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
 */
VO_U32 voplayerStop(VO_HANDLE hPlayer)
{
	if(!PLAYER_OBJ)
		return VO_ERR_PLAYER_NULL_POINTER;
	
	return PLAYER_OBJ->Stop();
}


VO_U32 voplayerGetStatus(VO_HANDLE hPlayer, VOMP_STATUS * pStatus)
{
	if(!PLAYER_OBJ)
		return VO_ERR_PLAYER_NULL_POINTER;
	
	return PLAYER_OBJ->GetStatus(pStatus);	
}

/**
 * Get the special value from param ID
 * \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
 * \param nParamID [in] The param ID
 * \param pParam [out] The return value depend on the param ID.
 */
VO_U32 voplayerGetParam(VO_HANDLE hPlayer, VO_U32 nParamID, VO_PTR pParam)
{
	if(!PLAYER_OBJ)
		return VO_ERR_PLAYER_NULL_POINTER;
	
	return PLAYER_OBJ->GetParam(nParamID, pParam);
}


/**
 * Set the special value from param ID
 * \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
 * \param nParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
VO_U32 voplayerSetParam(VO_HANDLE hPlayer, VO_U32 nParamID, VO_PTR pParam)
{
	if(!PLAYER_OBJ)
		return VO_ERR_PLAYER_NULL_POINTER;
	
	return PLAYER_OBJ->SetParam(nParamID, pParam);
}


VO_U32 VO_API voGetLSPAPI(VO_PLAYER_API* pHandle)
{
	pHandle->voplayerOpen					= voplayerOpen;
	pHandle->voplayerClose					= voplayerClose;
	pHandle->voplayerSetDrawArea			= voplayerSetDrawArea;
	pHandle->voplayerSetDataSource			= voplayerSetDataSource;
	pHandle->voplayerSendBuffer				= voplayerSendBuffer;
	pHandle->voplayerRun					= voplayerRun;
	pHandle->voplayerPause					= voplayerPause;
	pHandle->voplayerStop					= voplayerStop;
	pHandle->voplayerGetStatus				= voplayerGetStatus;
	pHandle->voplayerGetParam				= voplayerGetParam;
	pHandle->voplayerSetParam				= voplayerSetParam;
	
	return VO_ERR_PLAYER_OK;
}
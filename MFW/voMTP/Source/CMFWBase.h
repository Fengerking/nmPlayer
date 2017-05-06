/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2010			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CMFWBase.h

	Contains:	Base Media Framework Class

	Written by:	Lin Jun

	Change History (most recent first):
	2010-10-18		Jun			Create file

*******************************************************************************/

#pragma once
#include "voType.h"
#include "vomtpAPI.h"
#include "voLiveSource.h"

/**
 * Audio Video data buffer, usually used as output.
 */
typedef struct
{
	VO_BOOL				bVideo;				/*!< Video buffer flag */
	VO_BYTE*	 		pBuffer[3];			/*!< Buffer pointer */
	VO_U32				nStride[3];			/*!< Buffer stride */
	VO_U32				nSize;				/*!< Buffer total size */
	VO_U32				nColorType;			/*!< Color Type */
	VO_U64				llTime;				/*!< The time of the buffer */
	VO_VOID*			nReserved1;			/*!< Reserved parameter 1 */
	VO_VOID*			nReserved2;			/*!< Reserved parameter 2 */
}MFW_OUTPUT_BUFFER;

/**
 * Audio Video data buffer, usually used as input.
 */
typedef struct  
{
	VO_BOOL				bVideo;			/*!< video frame flag */
	VO_U32				nTrackID;		/*!< live source track ID */
	VO_U32				nCodecType;		/*!< live source codec type */
	VO_PBYTE			pBuffer;		/*!< Buffer pointer */
	VO_U32				nSize;			/*!< Buffer size in byte */
	VO_U64				llTime;			/*!< The time of the buffer */
	VO_U32				nFlag;			/*!< The buffer flag */
	VO_U32				nReserve;		/*!< The reserve value */
}MFW_SAMPLE;


typedef VO_VOID (VO_API * VOMFWSTATUS) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
typedef VO_VOID (VO_API * VOMFWOUTPUTDATA) (VO_PTR pUserData, MFW_OUTPUT_BUFFER* pBuf);


typedef struct
{
	VO_HANDLE		hDrawWnd;			/*!< The window's handle to draw video */
	VO_U32			nTrasferMode;		/*!< The Audio Video data transfer mode, refer to MTP_AV_TRANSFER_MODE */
	VO_PTR			pUserData;			/*!< The user data. It will use in call back function */
	VOMFWOUTPUTDATA	fOutputData;		/*!< Callback function for Output audio and video from Media Framework */
	VOMFWSTATUS		fStatus;			/*!< Status callback function. */
}MFW_OPEN_PARAM;




class CMFWBase
{
public:
	CMFWBase(void);
	virtual ~CMFWBase(void);

public:
	virtual VO_U32 Open(MFW_OPEN_PARAM* pParam)=0;
	virtual VO_U32 Close()=0;
	virtual VO_U32 SetDataSource(VO_PTR pSource)=0;
	virtual VO_U32 SendBuffer(MFW_SAMPLE* pSample)=0;
	virtual VO_U32 SetDrawArea(VO_U16 nLeft, VO_U16 nTop, VO_U16 nRight, VO_U16 nBottom)=0;
	virtual VO_U32 Seek(VO_U64 nPosition)=0;
	virtual VO_U32 GetPosition(VO_U64* pPosition)=0;
	virtual VO_U32 Start()=0;
	virtual VO_U32 Stop()=0;
	virtual VO_U32 Pause()=0;

	virtual VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam)=0;
	virtual VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam)=0;

	virtual VO_U32 OnTrackInfo(VO_LIVESRC_TRACK_INFO* pTrackInfo);
	virtual VO_VOID	Flush();

protected:
	VO_U32	OutputData(MFW_OUTPUT_BUFFER* pBuf);

protected:
	MFW_OPEN_PARAM	m_OpenParam;
};

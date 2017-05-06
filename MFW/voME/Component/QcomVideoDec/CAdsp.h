	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAdsp.h

	Contains:	QCOM ADSP wrapper header file

	Written by:	East Zhou

	Change History (most recent first):
	2010-06-21		East		Create file

*******************************************************************************/

#ifndef __C_ADSP_H__
#define __C_ADSP_H__

#include "voType.h"

typedef struct VO_BUFFER_INFO
{
	VO_U32				nMinNum;		//minimum buffer number
	VO_U32				nMaxNum;		//maximum buffer number
	VO_U32				nSize;			//buffer size
} VO_BUFFER_INFO;

typedef struct VO_BUFFER_REQUEST
{
	VO_BUFFER_INFO		sInputBufferInfo;		//input buffer information
	VO_BUFFER_INFO		sOutputBufferInfo;		//output buffer information
	VO_BUFFER_INFO		sDecReq1BufferInfo;
	VO_BUFFER_INFO		sDecReq2BufferInfo;
} VO_BUFFER_REQUEST;

typedef enum
{
	ADSP_BUFFER_TYPE_INPUT, 
	ADSP_BUFFER_TYPE_OUTPUT, 
	ADSP_BUFFER_TYPE_INTERNAL1, 
	ADSP_BUFFER_TYPE_INTERNAL2, 
} ADSP_BUFFER_TYPE;

typedef enum
{
	ADSP_MESSAGE_TYPE_UNKNOWN, 
	ADSP_MESSAGE_TYPE_BUFFERDONE, 
	ADSP_MESSAGE_TYPE_FRAMEDONE, 
} ADSP_MESSAGE_TYPE;

typedef struct ADSP_BUFFERDONE_INFO
{
	VO_U32				nBufferID;
} ADSP_BUFFERDONE_INFO;

typedef enum
{
	ADSP_VDEC_PORTTYPE_INPUT, 
	ADSP_VDEC_PORTTYPE_OUTPUT, 
	ADSP_VDEC_PORTTYPE_ALL, 
} ADSP_VDEC_PORTTYPE;

typedef enum
{
	ADSP_VDEC_DECODESUCCESS,			//success
	ADSP_VDEC_DECODEERROR,				//error(passable)
	ADSP_VDEC_FATALERROR,				//fatal error
	ADSP_VDEC_FLUSHDONE,				//flush operation finished
	ADSP_VDEC_EOS,						//end of stream
	ADSP_VDEC_FLUSHED,					//frame is flushed
	ADSP_VDEC_STREAMSWITCHED,			//stream has switched
	ADSP_VDEC_SUSPENDDONE, 				//suspend operation finished
} ADSP_VDEC_STATUS;

typedef struct ADSP_VDEC_RECT
{
	VO_U32				nLeft;
	VO_U32				nTop;
	VO_U32				nRight;
	VO_U32				nBottom;
} ADSP_VDEC_RECT;

typedef enum
{
	ADSP_VDEC_PICTURETYPE_I, 
	ADSP_VDEC_PICTURETYPE_P, 
	ADSP_VDEC_PICTURETYPE_B, 
	ADSP_VDEC_PICTURETYPE_BI, 
	ADSP_VDEC_PICTURETYPE_SKIP, 
	ADSP_VDEC_PICTURETYPE_UNKNOWN, 
} ADSP_VDEC_PICTURETYPE;

typedef enum
{
	ADSP_VDEC_PICTUREFORMAT_PROGRESSIVEFRAME, 
	ADSP_VDEC_PICTUREFORMAT_INTERLACEDFRAME, 
	ADSP_VDEC_PICTUREFORMAT_INTERLACEDFIELD, 
} ADSP_VDEC_PICTUREFORMAT;

typedef enum
{
	ADSP_VDEC_PICTURERESOLUTION_1x1, 
	ADSP_VDEC_PICTURERESOLUTION_2x1, 
	ADSP_VDEC_PICTURERESOLUTION_1x2, 
	ADSP_VDEC_PICTURERESOLUTION_2x2, 
} ADSP_VDEC_PICTURERESOLUTION;

#define MAX_VC1_PAN_SCAN_WINDOWS	4
typedef struct ADSP_VDEC_VC1PANSCANREGIONS
{
	VO_S32				nNum;
	VO_S32				nWidth[MAX_VC1_PAN_SCAN_WINDOWS];
	VO_S32				nHeight[MAX_VC1_PAN_SCAN_WINDOWS];
	VO_S32				nHorOffset[MAX_VC1_PAN_SCAN_WINDOWS];
	VO_S32				nVerOffset[MAX_VC1_PAN_SCAN_WINDOWS];
} ADSP_VDEC_VC1PANSCANREGIONS;

#define MAX_FIELDS	2
typedef struct ADSP_VDEC_FRAMEINFO
{
	ADSP_VDEC_STATUS			eStatus;
	VO_U32						nUserData1;
	VO_U32						nUserData2;
	VO_U64						ullTimeStamp;
	VO_U64						ullCalculatedTimeStamp;
	VO_U32						nDecPicWidth;
	VO_U32						nDecPicHeight;
	ADSP_VDEC_RECT				sCroppingWindow;
	ADSP_VDEC_PICTURETYPE		ePicType[MAX_FIELDS];
	ADSP_VDEC_PICTUREFORMAT		ePicFormat;
	VO_U32						nVC1RangeY;
	VO_U32						nVC1RangeUV;
	ADSP_VDEC_PICTURERESOLUTION	ePicResolution;
	VO_U32						nRepeatProgFrames;
	VO_U32						bRepeatFirstField;
	VO_U32						bTopFieldFirst;
	VO_U32						bFrameInterpFlag;
	ADSP_VDEC_VC1PANSCANREGIONS	sVC1PanScan;
	VO_U32						nPercentConcealedMacroblocks;
	VO_U32						nFlags;
	VO_U32						nPerformanceStats;   //performance statistics returned by the decoder
} ADSP_VDEC_FRAMEINFO;

typedef struct ADSP_FRAMEDONE_INFO
{
	VO_U32						nPMemID;
	VO_U32						nPMemOffset;

	ADSP_VDEC_FRAMEINFO			sFrameInfo;
} ADSP_FRAMEDONE_INFO;

typedef struct ADSP_MESSAGE_INFO
{
	union
	{
		ADSP_BUFFERDONE_INFO	sBufferDoneInfo;
		ADSP_FRAMEDONE_INFO		sFrameDoneInfo;
	};
} ADSP_MESSAGE_INFO;

typedef struct ADSP_MESSAGE
{
	ADSP_MESSAGE_TYPE	eType;
	ADSP_MESSAGE_INFO	sInfo;
} ADSP_MESSAGE;

class CAdsp
{
public:
	CAdsp();
	virtual ~CAdsp();

public:
	VO_BOOL				Open();
	VO_VOID				Close();

	VO_BOOL				Start(VO_U32 nCoding, VO_U32 nFourcc, VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_U32 nWidth, VO_U32 nHeight);
	VO_VOID				Stop();

	VO_VOID				Flush(ADSP_VDEC_PORTTYPE nPort);

	VO_BOOL				SetBuffers(VO_U32 nPMemID, ADSP_BUFFER_TYPE nBufferType, VO_U32 nBufferCount, VO_BOOL bIsLast, VO_U32 nBufferOffset, VO_U32 nBufferSize);
	VO_BOOL				FreeBuffers(VO_U32 nPMemID, ADSP_BUFFER_TYPE nBufferType, VO_U32 nBufferCount, VO_BOOL bIsLast, VO_U32 nBufferOffset, VO_U32 nBufferSize);

	VO_BOOL				PostInputBuffer(VO_U32 nPMemID, VO_U32 nOffset, VO_U32 nSize, VO_S64 llTime, VO_U32 nFlags, VO_BOOL bIsEOS, VO_PTR pUserData);
	VO_BOOL				ReleaseFrame(VO_U32 nBuffer);

	//0: failed; 1: success; 2: retry
	VO_U32				GetCurrentMessage(ADSP_MESSAGE* psMessage);

public:
	VO_BUFFER_REQUEST*	GetBufferRequestInformation() {return &m_BufferRequest;}

protected:
	VO_S32				m_nFD;
	VO_S32				m_nCpuDmaFD;
	VO_U32				m_nCpuDmaFreq;

	VO_BUFFER_REQUEST	m_BufferRequest;
};

#endif // __CQcomVideoDec_H__

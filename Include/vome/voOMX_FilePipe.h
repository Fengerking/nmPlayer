/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/


#ifndef __OMX_VO_FilePipe_H__
#define __OMX_VO_FilePipe_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "voOMX_Index.h"

#define OMX_VO_FILE_PIPE_NAME					0x1
#define OMX_VO_FILE_PIPE_HANDLE					0x2
#define OMX_VO_FILE_PIPE_ID						0x3

/**
 * File operate mode
 */
typedef enum
{
	OMX_VO_FILE_READ_ONLY				= 0X00000001,	/*!< Read file only*/
	OMX_VO_FILE_WRITE_ONLY				= 0X00000002,	/*!< Write file only*/
	OMX_VO_FILE_READ_WRITE				= 0X00000003,	/*!< Read and Write*/
	OMX_VO_FILE_MODE_MAX				= 0X7FFFFFFF
}OMX_VO_FILE_MODE;

/**
 * File position
 */
typedef enum
{
	OMX_VO_FILE_BEGIN					= 0X00000000,	/*!< from begin position of file*/
	OMX_VO_FILE_CURRENT					= 0X00000001,	/*!< from current position of file*/
	OMX_VO_FILE_END						= 0X00000002,	/*!< form end position of file*/
	OMX_VO_FILE_POS_MAX					= 0X7FFFFFFF
}OMX_VO_FILE_POS;

/**
* File Source
*/
typedef struct
{
	OMX_U32				nFlag;			/*!< OMX_VO_FILE_TYPE_NAME, HANDLE, ID */
	OMX_PTR				pSource;		/*!< File name, handle or id */
	OMX_S64				nOffset;		/*!< The begin pos of the file */
	OMX_S64				nLength;		/*!< The length of the file */
	OMX_VO_FILE_MODE	nMode;			/*!< The operate mode */
	OMX_U32				nReserve;		/*!< Reserve param */
} OMX_VO_FILE_SOURCE;

/**
* File IO functions set
*/
typedef struct
{
	OMX_PTR (OMX_APIENTRY * Open) (OMX_VO_FILE_SOURCE * pSource);						/*!< File open operation, return the file IO handle. failed return NULL*/
	OMX_S32 (OMX_APIENTRY * Read) (OMX_PTR pFile, OMX_PTR pBuffer, OMX_U32 uSize);		/*!< File read operation. return read number, failed return -1, retry return -2*/
	OMX_S32 (OMX_APIENTRY * Write) (OMX_PTR pFile, OMX_PTR pBuffer, OMX_U32 uSize);		/*!< File write operation. return write number, failed return -1*/
	OMX_S32 (OMX_APIENTRY * Flush) (OMX_PTR pFile);										/*!< File flush operation. return 0*/
	OMX_S64 (OMX_APIENTRY * Seek) (OMX_PTR pFile, OMX_S64 nPos, OMX_VO_FILE_POS uFlag);	/*!< File seek operation. return current file position, failed return -1*/
	OMX_S64 (OMX_APIENTRY * Size) (OMX_PTR pFile);										/*!< File get size operation. return file size, failed return -1*/
	OMX_S64 (OMX_APIENTRY * Save) (OMX_PTR pFile);										/*!< File get save size operation. return file size, failed return -1*/
	OMX_S32 (OMX_APIENTRY * Close) (OMX_PTR pFile);										/*!< File close operation, failed return -1*/
} OMX_VO_FILE_OPERATOR;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __OMX_VO_FilePipe_H__

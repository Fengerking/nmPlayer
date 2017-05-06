	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXFile.h

	Contains:	file operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voOMXFile_H__
#define __voOMXFile_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <OMX_Types.h>

/**
 * File operate mode
 */
typedef enum
{
	VOOMX_FILE_READ_ONLY				= 0X00000001,	/*!< Read file only*/
	VOOMX_FILE_WRITE_ONLY				= 0X00000002,	/*!< Write file only*/
	VOOMX_FILE_READ_WRITE				= 0X00000003,	/*!< Read and Write*/
}VOOMX_FILE_MODE;

/**
 * File position
 */
typedef enum
{
	VOOMX_FILE_BEGIN					= 0X00000000,	/*!< from begin position of file*/
	VOOMX_FILE_CURRENT					= 0X00000001,	/*!< from current position of file*/
	VOOMX_FILE_END						= 0X00000002,	/*!< form end position of file*/
}VOOMX_FILE_POS;

/**
 * Open the file if it exist, or create the file if mode contains write
 * \param uID [in] The module ID
 * \param uSize [in] The file name
 * \param uMode [in] The file operate mode
 * \return value 0 failed. or file pointer
 */
OMX_PTR	voOMXFileOpen (OMX_STRING  pName, VOOMX_FILE_MODE uMode);

/**
 * Read file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voOMXFileOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to read
 * \return value is read data size.
 */
OMX_U32	voOMXFileRead (OMX_PTR pHandle, OMX_U8 * pBuffer, OMX_U32 uSize);

/**
 * Write file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voOMXFileOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to write
 * \return value is write data size.
 */
OMX_U32	voOMXFileWrite (OMX_PTR pHandle, OMX_U8 * pBuffer, OMX_U32 uSize);


/**
 * flush the file buffer to disk
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voOMXFileOpen.
 * \return value non zero is successful.
 */
OMX_U32	voOMXFileFlush (OMX_PTR pHandle);


/**
 * Seek the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voOMXFileOpen.
 * \param nPos [in] The seek position
 * \param uFlag [in] The start file position
 * \return value is current file position
 */
OMX_U32	voOMXFileSeek (OMX_PTR pHandle, OMX_S64 nPos, VOOMX_FILE_POS uFlag);


/**
 * get the file szie
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voOMXFileOpen.
 * \return value is the file size
 */
OMX_S64	voOMXFileSize (OMX_PTR pHandle);

/**
 * close the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voOMXFileOpen.
 * \return value 0 is successful
 */
OMX_U32	voOMXFileClose (OMX_PTR pHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voOMXFile_H__

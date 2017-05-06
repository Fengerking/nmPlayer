	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		cmnFile.h

	Contains:	file operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __cmnFile_H__
#define __cmnFile_H__

#include "voType.h"
#include "voString.h"
#include "voFile.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif

extern VO_FILE_OPERATOR		g_fileOP;

/**
 * Open the file if it exist, or create the file if mode contains write
 * \param uID [in] The module ID
 * \param uSize [in] The file name
 * \param uMode [in] The file operate mode
 * \return value 0 failed. or file pointer
 */
VO_PTR	cmnFileOpen (VO_FILE_SOURCE * pSource);

/**
 * Read file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to read
 * \return value is read data size.
 */
VO_S32	cmnFileRead (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);

/**
 * Write file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to write
 * \return value is write data size.
 */
VO_S32	cmnFileWrite (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);


/**
 * flush the file buffer to disk
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \return value non zero is successful.
 */
VO_S32	cmnFileFlush (VO_PTR pHandle);


/**
 * Seek the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \param nPos [in] The seek position
 * \param uFlag [in] The start file position
 * \return value is current file position
 */
VO_S64	cmnFileSeek (VO_PTR pHandle, VO_S64 nPos, VO_FILE_POS uFlag);


/**
 * get the file szie
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \return value is the file size
 */
VO_S64	cmnFileSize (VO_PTR pHandle);

/**
 * get the file saved szie
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \return value is the file size
 */
VO_S64	cmnFileSave (VO_PTR pHandle);

/**
 * close the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \return value 0 is successful
 */
VO_S32	cmnFileClose (VO_PTR pHandle);

/**
 * Fill the file operator function pointer
 * \param uID [in] The module ID
 * \return value 0 is successful
 */
VO_S32	cmnFileFillPointer (void);

/**
 * Delete the file
 * \param pFileName[in] The file name.
 * \return value 0 is ok
 */
VO_S32	cmnFileDelete (VO_PTCHAR pFileName);

/**
 * Create the folder
 * \param pFolderName[in] The Folder name.
 * \return value 0 is ok
 */
VO_S32	cmnFolderCreate (VO_PTCHAR pFolderName);

/**
 * Detete the folder
 * \param pDpFolderNameirName[in] The Folder name.
 * \return value 0 is ok
 */
VO_S32	cmnFolderDelete (VO_PTCHAR pFolderName);

#ifdef _VONAMESPACE
    }
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */

#endif // __cmnFile_H__

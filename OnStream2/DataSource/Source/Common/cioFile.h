	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		cioFile.h

	Contains:	file operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __cioFile_H__
#define __cioFile_H__

#include "voType.h"
#include "voString.h"
#include "voFile.h"
#include "voSource2_IO.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif

extern VO_FILE_OPERATOR		g_fileIOOP;
extern VO_SOURCE2_IO_API	g_sourceIO;

/**
 * Open the file if it exist, or create the file if mode contains write
 * \param uID [in] The module ID
 * \param uSize [in] The file name
 * \param uMode [in] The file operate mode
 * \return value 0 failed. or file pointer
 */
VO_PTR	cioFileOpen (VO_FILE_SOURCE * pSource);

/**
 * Read file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to read
 * \return value is read data size.
 */
VO_S32	cioFileRead (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);

/**
 * Write file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to write
 * \return value is write data size.
 */
VO_S32	cioFileWrite (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);


/**
 * flush the file buffer to disk
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \return value non zero is successful.
 */
VO_S32	cioFileFlush (VO_PTR pHandle);


/**
 * Seek the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \param nPos [in] The seek position
 * \param uFlag [in] The start file position
 * \return value is current file position
 */
VO_S64	cioFileSeek (VO_PTR pHandle, VO_S64 nPos, VO_FILE_POS uFlag);


/**
 * get the file szie
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \return value is the file size
 */
VO_S64	cioFileSize (VO_PTR pHandle);

/**
 * get the file saved szie
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \return value is the file size
 */
VO_S64	cioFileSave (VO_PTR pHandle);

/**
 * close the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from cmnFileOpen.
 * \return value 0 is successful
 */
VO_S32	cioFileClose (VO_PTR pHandle);


VO_S32	cioFileFillPointer (VO_SOURCE2_IO_API* pSourceIO);

#ifdef _VONAMESPACE
    }
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */

#endif // __cioFile_H__

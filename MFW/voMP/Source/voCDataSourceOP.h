	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2010			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCDataSourceOP.h

	Contains:	file operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-14		JBF			Create file

*******************************************************************************/
#ifndef __voCDataSourceOP_H__
#define __voCDataSourceOP_H__

#include "voFile.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Open the file if it exist, or create the file if mode contains write
 * \param uID [in] The module ID
 * \param uSize [in] The file name
 * \param uMode [in] The file operate mode
 * \return value 0 failed. or file pointer
 */
void *	vompSourceOPOpen (VO_FILE_SOURCE * pSource);

/**
 * Read file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from vompSourceOPOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to read
 * \return value is read data size.
 */
VO_S32	vompSourceOPRead (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);

/**
 * Write file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from vompSourceOPOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to write
 * \return value is write data size.
 */
VO_S32	vompSourceOPWrite (VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize);


/**
 * flush the file buffer to disk
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from vompSourceOPOpen.
 * \return value non zero is successful.
 */
VO_S32	vompSourceOPFlush (VO_PTR pHandle);


/**
 * Seek the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from vompSourceOPOpen.
 * \param nPos [in] The seek position
 * \param uFlag [in] The start file position
 * \return value is current file position
 */
long long vompSourceOPSeek (void * pHandle, long long nPos, VO_FILE_POS uFlag);


/**
 * get the file szie
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from vompSourceOPOpen.
 * \return value is the file size
 */
long long	vompSourceOPSize (void * pHandle);

/**
 * get the file saved szie
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from vompSourceOPOpen.
 * \return value is the file size
 */
long long	vompSourceOPSave (void * pHandle);

/**
 * close the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from vompSourceOPOpen.
 * \return value 0 is successful
 */
VO_S32	vompSourceOPClose (VO_PTR pHandle);


/**
 * Fill the file operator function pointer
 * \param uID [in] The module ID
 * \return value 0 is successful
 */
int	voSourceOPFillPointer (void);


extern VO_FILE_OPERATOR		g_vompSourceOP;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif // __voCDataSourceOP_H__

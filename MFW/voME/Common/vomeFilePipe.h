	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vomeFilePipe.h

	Contains:	file operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-09-03		JBF			Create file

*******************************************************************************/

#ifndef __vomeFilePipe_H__
#define __vomeFilePipe_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <OMX_Index.h>
#include <voOMX_FilePipe.h>

/**
 * Open the file if it exist, or create the file if mode contains write
 * \param uSize [in] The file name
 * \param uMode [in] The file operate mode
 * \return value 0 failed. or file pointer
 */
OMX_PTR	vomeFilePipeOpen (OMX_VO_FILE_SOURCE * pSource);

/**
 * Read file data
 * \param pHandle [in] The value return from vomeFilePipeOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to read
 * \return value is read data size.
 */
OMX_S32	vomeFilePipeRead (OMX_PTR pHandle, OMX_PTR pBuffer, OMX_U32 uSize);

/**
 * Write file data
 * \param pHandle [in] The value return from vomeFilePipeOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to write
 * \return value is write data size.
 */
OMX_S32	vomeFilePipeWrite (OMX_PTR pHandle, OMX_PTR pBuffer, OMX_U32 uSize);


/**
 * flush the file buffer to disk
 * \param pHandle [in] The value return from vomeFilePipeOpen.
 * \return value non zero is successful.
 */
OMX_S32	vomeFilePipeFlush (OMX_PTR pHandle);


/**
 * Seek the file
 * \param pHandle [in] The value return from vomeFilePipeOpen.
 * \param nPos [in] The seek position
 * \param uFlag [in] The start file position
 * \return value is current file position
 */
OMX_S64	vomeFilePipeSeek (OMX_PTR pHandle, OMX_S64 nPos, OMX_VO_FILE_POS uFlag);


/**
 * get the file szie
 * \param pHandle [in] The value return from vomeFilePipeOpen.
 * \return value is the file size
 */
OMX_S64	vomeFilePipeSize (OMX_PTR pHandle);

/**
 * get the file save size
 * \param pHandle [in] The value return from vomeFilePipeOpen.
 * \return value is the file save size
 */
OMX_S64	vomeFilePipeSave (OMX_PTR pHandle);

/**
 * close the file
 * \param pHandle [in] The value return from vomeFilePipeOpen.
 * \return value 0 is successful
 */
OMX_S32	vomeFilePipeClose (OMX_PTR pHandle);

/**
 * Fill the file operator function pointer
 * \return value 0 is successful
 */
OMX_S32	vomeFilePipeFillPointer (OMX_VO_FILE_OPERATOR * pPipeOP);

#ifdef __cplusplus
}
#endif /* __cplusplus */

class vomeCBaseFileOP
{
public:
    vomeCBaseFileOP(void);
    virtual ~vomeCBaseFileOP(void);

	virtual OMX_PTR	Open (OMX_VO_FILE_SOURCE * pSource);
	virtual OMX_S32 	Read (OMX_PTR pBuffer, OMX_U32 uSize);
	virtual OMX_S32 	Write (OMX_PTR pBuffer, OMX_U32 uSize);
	virtual OMX_S32 	Flush (void);
	virtual OMX_S64 	Seek (OMX_S64 nPos, OMX_VO_FILE_POS uFlag);
	virtual OMX_S64 	Size (void);
	virtual OMX_S64 	Save (void);
	virtual OMX_S32 	Close (void);

protected:
	virtual OMX_S64 	FileSize (void);


protected:
	OMX_VO_FILE_SOURCE	m_Source;

	OMX_PTR			m_hHandle;
	OMX_PTR			m_hOpen;
	OMX_S64			m_nFilePos;
};

#endif // __vomeFilePipe_H__

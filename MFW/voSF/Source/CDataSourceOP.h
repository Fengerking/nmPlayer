	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2010			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCFileOP.h

	Contains:	file operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-14		JBF			Create file

*******************************************************************************/
#ifndef __voCFileOP_H__
#define __voCFileOP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
* File Source
*/
typedef struct
{
	int				nFlag;			/*!< VOSF_FILE_TYPE_NAME, HANDLE, ID */
	void *			pSource;		/*!< File name, handle or id */
	long long		nOffset;		/*!< The begin pos of the file */
	long long		nLength;		/*!< The length of the file */
	int				nMode;			/*!< The operate mode */
	int				nReserve;		/*!< Reserve param */
} vosfFileOpenSource;

/**
 * File position
 */
typedef enum
{
	VOSF_FILE_BEGIN					= 0X00000000,	/*!< from begin position of file*/
	VOSF_FILE_CURRENT					= 0X00000001,	/*!< from current position of file*/
	VOSF_FILE_END						= 0X00000002,	/*!< form end position of file*/
	VOSF_FILE_POS_MAX					= 0X7FFFFFFF,
}VOSF_FILE_POS;

/**
 * Open the file if it exist, or create the file if mode contains write
 * \param uID [in] The module ID
 * \param uSize [in] The file name
 * \param uMode [in] The file operate mode
 * \return value 0 failed. or file pointer
 */
void *	voFileOPOpen (vosfFileOpenSource * pSource);

/**
 * Read file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voFileOPOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to read
 * \return value is read data size.
 */
int	voFileOPRead (void * pHandle, void * pBuffer, int uSize);

/**
 * Write file data
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voFileOPOpen.
 * \param pBuffer [in] The buffer pointer
 * \param uSize [in] The size to write
 * \return value is write data size.
 */
int	voFileOPWrite (void * pHandle, void * pBuffer, int uSize);


/**
 * flush the file buffer to disk
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voFileOPOpen.
 * \return value non zero is successful.
 */
int	voFileOPFlush (void * pHandle);


/**
 * Seek the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voFileOPOpen.
 * \param nPos [in] The seek position
 * \param uFlag [in] The start file position
 * \return value is current file position
 */
long long voFileOPSeek (void * pHandle, long long nPos, VOSF_FILE_POS uFlag);


/**
 * get the file szie
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voFileOPOpen.
 * \return value is the file size
 */
long long	voFileOPSize (void * pHandle);

/**
 * get the file saved szie
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voFileOPOpen.
 * \return value is the file size
 */
long long	voFileOPSave (void * pHandle);

/**
 * close the file
 * \param uID [in] The module ID
 * \param pHandle [in] The value return from voFileOPOpen.
 * \return value 0 is successful
 */
int	voFileOPClose (void * pHandle);


/**
 * Fill the file operator function pointer
 * \param uID [in] The module ID
 * \return value 0 is successful
 */
int	voFileFillPointer (void);

/**
* File IO functions set
* uID: module ID which call file IO actually
*/
typedef struct
{
	void *		(* Open) (vosfFileOpenSource * pSource);						/*!< File open operation, return the file IO handle. failed return NULL*/
	int			(* Read) (void * pFile, void * pBuffer, int uSize);			/*!< File read operation. return read number, failed return -1, retry return -2*/
	int			(* Write) (void * pFile, void * pBuffer, int uSize);		/*!< File write operation. return write number, failed return -1*/
	int			(* Flush) (void * pFile);									/*!< File flush operation. return 0*/
	long long	(* Seek) (void * pFile, long long nPos, VOSF_FILE_POS uFlag);	/*!< File seek operation. return current file position, failed return -1*/
	long long	(* Size) (void * pFile);									/*!< File get size operation. return file size, failed return -1*/
	long long	(* Save) (void * pFile);									/*!< File get saved operation. return file saved size, failed return -1*/
	int			(* Close) (void * pFile);									/*!< File close operation, failed return -1*/
} VOSF_FILE_OPERATOR;

extern VOSF_FILE_OPERATOR		g_vosfFileOP;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif // __voCFileOP_H__

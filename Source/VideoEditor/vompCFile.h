	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompCFile.h

	Contains:	file operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-09-03		JBF			Create file

*******************************************************************************/

#ifndef __vompCFile_H__
#define __vompCFile_H__

#include "vompAPI.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


int	vompReadAt (void * pUserData, long long llPos, unsigned char * pBuffer, int nSize);

int vompWriteAt (void * pUserData, long long llPos, unsigned char * pBuffer, int nSize);

int	vompGetSize (void * pUserData, long long  * pPos);


#ifdef __cplusplus
}
#endif /* __cplusplus */

class vompCBaseFileOP
{
public:
    vompCBaseFileOP(void);
    virtual ~vompCBaseFileOP(void);

	virtual int			Open (const char * pSource);
	virtual int 		Read (unsigned char * pBuffer, int uSize);
	virtual int 		Write (unsigned char * pBuffer, int uSize);
	virtual int 		Flush (void);
	virtual long long 	Seek (long long nPos);
	virtual long long 	Size (void);
	virtual int 		Close (void);

protected:
	virtual long long 	FileSize (void);


protected:
	FILE *			m_hFile;
	long long		m_llPos;
	long long		m_llSize;
};

#endif // __vompCFile_H__

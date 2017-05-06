
#ifndef __BufferIO_H__
#define __BufferIO_H__

#include "voYYDef_SrcCmn.h"
#include "voType.h"
#include "voFile.h"





#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	
	VO_S32 VO_API voGetBufferFileOperatorAPI(VO_FILE_OPERATOR * pFileOperator, VO_U32 uFlag);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
class BufferIO
{
public:
	BufferIO(void);
	virtual ~BufferIO(void);

	virtual VO_PTR	Open (VO_FILE_SOURCE * pSource);
	virtual VO_S32 	Read (VO_PTR pBuffer, VO_U32 uSize);
	virtual VO_S32 	Write (VO_PTR pBuffer, VO_U32 uSize);
	virtual VO_S32 	Flush (void);
	virtual VO_S64 	Seek (VO_S64 nPos, VO_FILE_POS uFlag);
	virtual VO_S64 	Size (void);
	virtual VO_S64 	Save (void);
	virtual VO_S32 	Close (void);

protected:

	VO_S64			m_lFilePos;
	VO_CHAR*		m_pBuffer;
	VO_S64			m_nSize;
};

#ifdef _VONAMESPACE
}
#endif

#endif	//__CPushFileStream_H__

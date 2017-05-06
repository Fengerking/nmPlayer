#ifndef __CAES_CBC_H__
#define __CAES_CBC_H__
#include "voDRM2.h"
#include "aes.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CAES_CBC
{
public:
	CAES_CBC(void);
	~CAES_CBC(void);

	VO_U32 setKey(const VO_BYTE *pKey, VO_U32 uLen);
	VO_U32 setIV(const VO_BYTE *pIV, VO_U32 uLen);
	VO_U32 decryptData(const VO_BYTE *pSrc, VO_U32 uSrc, VO_BYTE *pDes, VO_U32* puDes, VO_BOOL bEnd);

private:
	void* m_ctx;

	VO_BYTE m_CBC[AES_BLOCK_SIZE];
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CAES_CBC_H__

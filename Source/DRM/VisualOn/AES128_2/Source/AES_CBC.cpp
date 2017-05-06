#include "AES_CBC.h"
#include "voIndex.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "CAES_CBC"
#endif


CAES_CBC::CAES_CBC(void)
: m_ctx(NULL)
{
	memset(m_CBC, 0, 16);
}

CAES_CBC::~CAES_CBC(void)
{
	if (m_ctx)
	{
		aes_decrypt_deinit(m_ctx);
		m_ctx = NULL;
	}
}

VO_U32 CAES_CBC::setKey(const VO_BYTE *pKey, VO_U32 uLen)
{
	if (m_ctx)
	{
		aes_decrypt_deinit(m_ctx);
		m_ctx = NULL;
	}

	m_ctx = aes_decrypt_init(pKey, uLen);

	return (NULL != m_ctx) ? VO_ERR_DRM2_OK : VO_ERR_DRM2_BADPARAMETER;
}

VO_U32 CAES_CBC::setIV(const VO_BYTE *pIV, VO_U32 uLen)
{
	memcpy(m_CBC, pIV, uLen);

	return VO_ERR_DRM2_OK;
}

VO_U32 CAES_CBC::decryptData(const VO_BYTE *pSrc, VO_U32 uSrc, VO_BYTE *pDes, VO_U32* puDes, VO_BOOL bEnd)
{
	if (NULL == pSrc || NULL == pDes || NULL == puDes)
	{
		VOLOGE("empty pointor");
		return VO_ERR_DRM2_BADPARAMETER;
	}

	VO_BYTE tmp[AES_BLOCK_SIZE] = {0};

	if (uSrc % 16)
	{
		VOLOGW("%d is not an integer multiple of 16", uSrc);
	}

	for (VO_U32 i = 0; i < (uSrc / AES_BLOCK_SIZE); i++)
	{
		const VO_BYTE *pos = &pSrc[AES_BLOCK_SIZE * i];
		memcpy(tmp, pos, AES_BLOCK_SIZE);

		//VOLOGI("m_ctx %p, pos from %d in uSrc %d", m_ctx, AES_BLOCK_SIZE * i, uSrc);
		aes_decrypt(m_ctx, pos, &pDes[AES_BLOCK_SIZE * i]);
		for (int j = 0; j < AES_BLOCK_SIZE; j++)
			pDes[AES_BLOCK_SIZE * i + j] ^= m_CBC[j];

		memcpy(m_CBC, tmp, AES_BLOCK_SIZE);
	}

	if (bEnd)
	{
		*puDes = uSrc - pDes[uSrc - 1];

		if (0 == pDes[uSrc - 1])
		{
			VOLOGW("last: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", pDes[uSrc - 16], pDes[uSrc - 15], pDes[uSrc - 14], pDes[uSrc - 13], pDes[uSrc - 12], pDes[uSrc - 11], pDes[uSrc - 10], pDes[uSrc - 9], pDes[uSrc - 8], pDes[uSrc - 7], pDes[uSrc - 6], pDes[uSrc - 5], pDes[uSrc - 4], pDes[uSrc - 3], pDes[uSrc - 2], pDes[uSrc - 1]);
		}
	}
	else
	{
		*puDes = uSrc;
	}

	return VO_ERR_DRM2_OK;
}

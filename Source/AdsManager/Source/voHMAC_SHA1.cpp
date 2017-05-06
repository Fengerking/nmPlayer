//******************************************************************************
//* HMAC_SHA1.cpp : Implementation of HMAC SHA1 algorithm
//*                 Comfort to RFC 2104
//*
//******************************************************************************
//#include <iostream>
#include "memory.h"
#include "voHMAC_SHA1.h"


void voHMAC_SHA1::HMAC_SHA1(BYTE *text, int text_len, BYTE *key, int key_len, BYTE *digest)
{
	memset(SHA1_Key, 0, SHA1_BLOCK_SIZE);

	/* repeated 64 times for values in ipad and opad */
	memset(m_ipad, 0x36, sizeof(m_ipad));
	memset(m_opad, 0x5c, sizeof(m_opad));

	/* STEP 1 */
	if (key_len > SHA1_BLOCK_SIZE)
	{
		voSHA1::Reset();
		voSHA1::Process((VO_U8 *)key, key_len);
		voSHA1::Finalize();

		voSHA1::GetHashData((VO_U8 *)SHA1_Key);
	}
	else
		memcpy(SHA1_Key, key, key_len);

	/* STEP 2 */
	for (int i=0; i<sizeof(m_ipad); i++)
	{
		m_ipad[i] ^= SHA1_Key[i];		
	}

	/* STEP 3 */
	memcpy(AppendBuf1, m_ipad, sizeof(m_ipad));
	memcpy(AppendBuf1 + sizeof(m_ipad), text, text_len);

	/* STEP 4 */
	voSHA1::Reset();
	voSHA1::Process((VO_U8 *)AppendBuf1, sizeof(m_ipad) + text_len);
	voSHA1::Finalize();

	voSHA1::GetHashData((VO_U8 *)szReport);

	/* STEP 5 */
	for (int j=0; j<sizeof(m_opad); j++)
	{
		m_opad[j] ^= SHA1_Key[j];
	}

	/* STEP 6 */
	memcpy(AppendBuf2, m_opad, sizeof(m_opad));
	memcpy(AppendBuf2 + sizeof(m_opad), szReport, SHA1_DIGEST_LENGTH);

	/*STEP 7 */
	voSHA1::Reset();
	voSHA1::Process((VO_U8 *)AppendBuf2, sizeof(m_opad) + SHA1_DIGEST_LENGTH);
	voSHA1::Finalize();

	voSHA1::GetHashData((VO_U8 *)digest);
}


#include "base64.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

static VO_CHAR strBase64DecodeTable[256] = {0};

///<we need create the Base64 decoder table,so we can map the char to this map and decode it
static VO_VOID CreateBase64DecodeTable()
{
	VO_U32 uCnt = 0;
	///<Init the table
	while (uCnt < 256)
	{
		strBase64DecodeTable[uCnt++] = (VO_CHAR)0x80;
	}
	///<replace the table value from array position 0x41('A') to 0x5A('Z') with 0 to 25
	uCnt = 'A';
	while (uCnt <= 'Z')
	{
		strBase64DecodeTable[uCnt++] = (uCnt - 'A');
	}

	///<replace the table value from array position 0x61('a') to 0x7A('z') with 26 to 51
	uCnt = 'a';
	while (uCnt <= 'z')
	{
		strBase64DecodeTable[uCnt++] = 26 + (uCnt - 'a');
	}
	///<replace the table value from array position 0x30('0') to 0x39('9') with 52 to 61
	uCnt = '0';
	while (uCnt <= '9')
	{
		strBase64DecodeTable[uCnt++] = 52 + (uCnt - '0');
	}
	///<replace the left
	strBase64DecodeTable['+'] = 62;
	strBase64DecodeTable['/'] = 63;
	strBase64DecodeTable['='] = 0;
}


static const VO_CHAR strBase64CharMap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

VO_U8 * Base64Decode(VO_CHAR * strInput, VO_U32 & uDecBufSize, VO_BOOL bTrimTrailingZeros)
{
	if (!strInput || !strlen(strInput))
	{
		return NULL;
	}
	///<first we check if the decoder table exist,if no,create it
	if (!strBase64DecodeTable[0])
	{
		CreateBase64DecodeTable();
	}

	///<new the decoder string buffer
	VO_U32 uStrLen = strlen(strInput);

	///<Check padding byte count
	VO_U32 uPaddingCount = (VO_U8)(strInput[uStrLen - 1] == '=') + (VO_U8)(strInput[uStrLen - 2] == '=');
	VO_U8 * pstrDes = new VO_U8[uStrLen + 1];
	if(!pstrDes)
		return NULL;
	pstrDes[uStrLen] = 0;
	
	VO_U32 uDesStrLen = 0;
	VO_CHAR TmpChar[4];
	for (VO_U32 uCnt = 0; uCnt < uStrLen - 3; uCnt += 4)
	{
		for(VO_U32 u4ByteLoop = 0; u4ByteLoop < 4; u4ByteLoop++)
		{
			TmpChar[u4ByteLoop] = strBase64DecodeTable[strInput[uCnt + u4ByteLoop]];
			if((TmpChar[u4ByteLoop] & 0x80) != 0)
				TmpChar[u4ByteLoop] = 0;
		}

		pstrDes[uDesStrLen++] = (TmpChar[0]<<2) | (TmpChar[1]>>4);
		pstrDes[uDesStrLen++] = (TmpChar[1]<<4) | (TmpChar[2]>>2);
		pstrDes[uDesStrLen++] = (TmpChar[2]<<6) | TmpChar[3];
	}
	
	///<delete the padding byte
	uDesStrLen -= uPaddingCount;
#if 0
	if (bTrimTrailingZeros)
	{
		while (uDesStrLen > 0 && !pstrDes[uDesStrLen-1]) 
			--uDesStrLen;
#if 0 ///<I think this workaround doesn't make sense,if you want all data,you should set bTrimTrailingZeros vo_false
		if(uDesStrLen %2 != 0)///<for avoid deleting too much 0
			uDesStrLen += 1;
#endif
	}
#endif
	uDecBufSize = uDesStrLen;
	return pstrDes;
}

VO_U8 * Base64Decode2(VO_CHAR * strInput, VO_U32 & uBufSize, VO_BOOL bTrimTrailingZeros)
{
	if (!strInput || 0 == uBufSize)
	{
		uBufSize = 0;
		return NULL;
	}
	///<first we check if the decoder table exist,if no,create it
	if (!strBase64DecodeTable[0])
	{
		CreateBase64DecodeTable();
	}

	///<new the decoder string buffer
	VO_U32 uStrLen = uBufSize;

	///<Check padding byte count
	VO_U32 uPaddingCount = (VO_U8)(strInput[uStrLen - 1] == '=') + (VO_U8)(strInput[uStrLen - 2] == '=');
	VO_U8 * pstrDes = new VO_U8[uStrLen + 1];
	if(!pstrDes)
		return NULL;
	pstrDes[uStrLen] = 0;

	VO_U32 uDesStrLen = 0;
	VO_CHAR TmpChar[4];
	for (VO_U32 uCnt = 0; uCnt < uStrLen - 3; uCnt += 4)
	{
		for(VO_U32 u4ByteLoop = 0; u4ByteLoop < 4; u4ByteLoop++)
		{
			TmpChar[u4ByteLoop] = strBase64DecodeTable[strInput[uCnt + u4ByteLoop]];
			if((TmpChar[u4ByteLoop] & 0x80) != 0)
				TmpChar[u4ByteLoop] = 0;
		}

		pstrDes[uDesStrLen++] = (TmpChar[0]<<2) | (TmpChar[1]>>4);
		pstrDes[uDesStrLen++] = (TmpChar[1]<<4) | (TmpChar[2]>>2);
		pstrDes[uDesStrLen++] = (TmpChar[2]<<6) | TmpChar[3];
	}

	///<delete the padding byte
	uDesStrLen -= uPaddingCount;
	uBufSize = uDesStrLen;
	return pstrDes;
}

VO_CHAR * Base64Encode(VO_CHAR const * strOri, VO_U32 uStrLength)
{
	VO_U8 const * strTmp = (VO_U8 const *)strOri;
	
	if (!strTmp)
		return NULL;

	///<caculate the count of 3 bytes which can be encode without filling, and the left bytes
	VO_U32 const uNum3Bytes = uStrLength / 3;
	VO_U32 const uNumLeftBytes = uStrLength % 3;

	VO_U32 uNewBufSize = 4*(uNum3Bytes + (uNumLeftBytes ? 1: 0));

	VO_PCHAR strDes =  new VO_CHAR[uNewBufSize + 1];

	///<first we encode the all 3 bytes data 
	VO_U32 uCnt = 0;
	while (uCnt < uNum3Bytes)
	{
		strDes[4*uCnt+0] = strBase64CharMap[(strTmp[3*uCnt]>>2)&0x3F];
		strDes[4*uCnt+1] = strBase64CharMap[(((strTmp[3*uCnt]&0x3)<<4) | (strTmp[3*uCnt+1]>>4))&0x3F];
		strDes[4*uCnt+2] = strBase64CharMap[((strTmp[3*uCnt+1]<<2) | (strTmp[3*uCnt+2]>>6))&0x3F];
		strDes[4*uCnt+3] = strBase64CharMap[strTmp[3*uCnt+2]&0x3F];
		uCnt++;
	}

	//then check the left count of byte,encode them
	if (uNumLeftBytes)
	{
		strDes[4*uCnt] = strBase64CharMap[(strTmp[3*uCnt]>>2)&0x3F];
		if (1 == uNumLeftBytes)
		{
			strDes[4*uCnt+1] = strBase64CharMap[((strTmp[3*uCnt]&0x3)<<4)&0x3F];
			strDes[4*uCnt+2] = '=';
		}
		else
		{
			strDes[4*uCnt+1] = strBase64CharMap[(((strTmp[3*uCnt]&0x3)<<4) | (strTmp[3*uCnt+1]>>4))&0x3F];
			strDes[4*uCnt+2] = strBase64CharMap[(strTmp[3*uCnt+1]<<2)&0x3F];
		}
		strDes[4*uCnt+3] = '=';

	}
	strDes[uNewBufSize] = '\0';
	return strDes;
}

#ifdef _VONAMESPACE
}
#endif
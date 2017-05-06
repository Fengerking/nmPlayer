#include "voReadBits.h"

void ParserInitBits(PARSER_BITSTREAM *pBS, const VO_U8 *pBuf, VO_U32 nLen)
{
	if (nLen >= 4)
	{
		pBS->nLen = nLen-4;

		pBS->nBuf = ((VO_U32)(*pBuf++)) << 24;
		pBS->nBuf |= ((VO_U32)(*pBuf++)) << 16;
		pBS->nBuf |= ((VO_U32)(*pBuf++)) << 8;
		pBS->nBuf |= (*pBuf++);

		pBS->pEnd = pBuf;
		pBS->nPos = 0;
	}
	else
	{
		pBS->nPos = 32;
		pBS->nBuf = 0;
		while (nLen--)
		{
			pBS->nPos -= 8;
			pBS->nBuf |= ((VO_U32)(*pBuf++)) << pBS->nPos;
		}
		pBS->pEnd = pBuf;
		pBS->nLen = 0;
	}
}

void  ParserFlushBits(PARSER_BITSTREAM *pBS, VO_U32 nBits)  
{
	pBS->nBuf <<= nBits;
	pBS->nPos += nBits;

	if((pBS->nPos - 16) > 0){
		if(pBS->nLen >= 2){
			VO_U32 nPos = pBS->nPos;

			pBS->nLen -= 2;

			pBS->nBuf |= (*pBS->pEnd++) << (nPos - 8);
			pBS->nBuf |= (*pBS->pEnd++) << (nPos - 16);

			pBS->nPos -= 16;
		}else if (pBS->nLen > 0){
			VO_U32 nPos = pBS->nPos;

			pBS->nBuf |= (*pBS->pEnd++) << (nPos - 8);
			pBS->nLen = 0;
			pBS->nPos -= 8;
		}else{	
			return;
		}
	}
}

VO_U32 ParserShowBits(PARSER_BITSTREAM *pBS, VO_U32 nBits)
{
	VO_U32 nBuf;
	VO_S32 nLen;
	const VO_U8 *pEnd;

	if (pBS->nPos + nBits <= 32)
	{
		return pBS->nBuf >> (32 - nBits);
	}

	nBuf = pBS->nBuf>>pBS->nPos;
	pEnd = pBS->pEnd;
	nLen = pBS->nLen;
	nBits -= 32 - pBS->nPos;

	while (nBits >= 8)
	{
		nBuf <<= 8;
		if (nLen>0)
		{
			nBuf |= *pEnd++;
			nLen--;
		}
		nBits -= 8;
	}
	nBuf <<= nBits;
	if (nLen>0)
	{
		nBuf |= (*pEnd)>>(8-nBits);
	}

	return nBuf;
}


VO_U32 ParserReadBits(PARSER_BITSTREAM *pBS, VO_U32 nBits)
{
	VO_U32 nBuf;

	nBuf = pBS->nBuf >> (32 - nBits);

	ParserFlushBits(pBS, nBits);

	return nBuf;
}

VO_U32 ParserEndOfBits(PARSER_BITSTREAM *pBS)
{
	return (pBS->nLen <= 0 && pBS->nPos >= 32);
}

const VO_U8 *ParserGetNextBytePos(PARSER_BITSTREAM *pBS) 
{
	if (pBS->nPos >= 32)
	{
		return pBS->pEnd;
	}
	return pBS->pEnd - ((32 - pBS->nPos)>>3);
}

VO_U32 ParserGetVLCSymbol (PARSER_BITSTREAM *pBS,VO_S32 *info)
{
	VO_S32 len = 0;
	while (!ParserReadBits(pBS, 1))
	{
		len++;
		if (ParserEndOfBits(pBS))
		{
			*info = 0;
			return 0;
		}
	}
	if(!len)
		*info = 0;
	else
		*info = ParserReadBits(pBS,len);
	return (len<<1)+1;  
}

VO_S32 p_ue_v (PARSER_BITSTREAM *pBS)
{
	VO_S32 value,len,inf;

	len =  ParserGetVLCSymbol (pBS, &inf);
	value = (int) (((VO_U32) 1 << (len >> 1)) + (VO_U32) (inf) - 1);
	return value;
}

VO_S32 p_se_v (PARSER_BITSTREAM *pBS)
{
	VO_S32 value,len,inf;
	VO_U32 n;
	len =  ParserGetVLCSymbol (pBS, &inf);
	n = ((VO_U32) 1 << (len >> 1)) + (VO_U32) inf - 1;
	value = (n + 1) >> 1;
	if((n & 0x01) == 0)                           // lsb is signed bit
		value = -value;
	return value;
}

VO_S32 p_u_v (int LenInBits, PARSER_BITSTREAM *pBS)
{
	VO_S32 inf;
	inf = ParserReadBits(pBS,LenInBits);
	return inf;
}

VO_S32 p_i_v (int LenInBits, PARSER_BITSTREAM *pBS)
{
	VO_S32 inf;
	inf = ParserReadBits(pBS,LenInBits);
	inf = -( inf & (1 << (LenInBits - 1)) ) | inf;
	return inf;
}

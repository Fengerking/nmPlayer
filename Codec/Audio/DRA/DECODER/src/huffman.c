#include "config.h"
#include "global.h"
#include "stream.h"

int HuffDec(Bitstream *bitptr, const int *pCodeBook, int nNumCodes)
{
	int n, k = 0;
	int nShift, unBits = 0;

	for(n = 0; n < nNumCodes; n++)
	{
		nShift = pCodeBook[k++];
		if(nShift > 0)
		{
			unBits = unBits << nShift;
			unBits |= GetBits(bitptr, nShift);
		}

		if(unBits == pCodeBook[k++])
		{
			return pCodeBook[k];
		}

		k++;
	}
	
	return 0;
}

int HuffDiff(Bitstream *bitptr, const int *pCodeBook, int nIndex, int nNumcodes)
{
	int nDiff;

	nDiff = HuffDec(bitptr, pCodeBook, nNumcodes);
	
	nIndex = (nIndex + nDiff) % nNumcodes;

	return nIndex;
}

int HuffDecRecurive(Bitstream *bitptr, const int *pCodeBook, int nNumCodes)
{
	int nQIndex;
	int k = -1;
	
	do{
		k++;
		nQIndex = HuffDec(bitptr, pCodeBook, nNumCodes);
	}while (nQIndex == nNumCodes - 1);
	
	nQIndex = k*(nNumCodes - 1) + nQIndex;

	return nQIndex;
}

int ResetHuffIndex()
{
	return 0;
}
/*
   Bit Buffer Management
 */

#include "bitbuffer.h"

#if 0
static void updateBitBufWordPtr(HANDLE_BIT_BUF hBitBuf,
                                UWord8 **pBitBufWord,
                                Word16   cnt)
{
  *pBitBufWord += cnt;                                                                  

                                                                                        
  if(*pBitBufWord > hBitBuf->pBitBufEnd) {
    *pBitBufWord -= (hBitBuf->pBitBufEnd - hBitBuf->pBitBufBase + 1);                   
  }
                                                                                        
  if(*pBitBufWord < hBitBuf->pBitBufBase) {
    *pBitBufWord += (hBitBuf->pBitBufEnd - hBitBuf->pBitBufBase + 1);                   
  }
}
#endif 

HANDLE_BIT_BUF CreateBitBuffer(HANDLE_BIT_BUF hBitBuf,
                               UWord8 *pBitBufBase,
                               Word16  bitBufSize)
{
  assert(bitBufSize*8 <= 32768);

  hBitBuf->pBitBufBase = pBitBufBase;                                                    
  hBitBuf->pBitBufEnd  = pBitBufBase + bitBufSize - 1;                                  

  hBitBuf->pWriteNext  = pBitBufBase;                                                    

  hBitBuf->cache       = 0;
  
  hBitBuf->wBitPos     = 0;                                                              
  hBitBuf->cntBits     = 0;   
  
  hBitBuf->size        = (bitBufSize << 3);                                             
  hBitBuf->isValid     = 1;                                                              

  return hBitBuf;
}

void DeleteBitBuffer(HANDLE_BIT_BUF *hBitBuf)
{
  if(*hBitBuf)
	(*hBitBuf)->isValid = 0;                                                               
  *hBitBuf = NULL;                                                                       
}


void ResetBitBuf(HANDLE_BIT_BUF hBitBuf,
                 UWord8 *pBitBufBase,
                 Word16  bitBufSize)
{
  hBitBuf->pBitBufBase = pBitBufBase;                                                    
  hBitBuf->pBitBufEnd  = pBitBufBase + bitBufSize - 1;                                  

                                                 
  hBitBuf->pWriteNext  = pBitBufBase;                                                    

  hBitBuf->wBitPos     = 0;    
  hBitBuf->cntBits     = 0;    
  
  hBitBuf->cache	   = 0;
}


void CopyBitBuf(HANDLE_BIT_BUF hBitBufSrc,
                HANDLE_BIT_BUF hBitBufDst)
{
  *hBitBufDst = *hBitBufSrc;                                                             
}

Word16 GetBitsAvail(HANDLE_BIT_BUF hBitBuf)
{
  return hBitBuf->cntBits;
}

Word16 WriteBits(HANDLE_BIT_BUF hBitBuf,
                 Word32 writeValue,
                 Word16 noBitsToWrite)
{
  Word16 wBitPos;

  assert(noBitsToWrite <= (Word16)sizeof(Word32)*8);

  hBitBuf->cntBits += noBitsToWrite;   

  wBitPos = hBitBuf->wBitPos;
  wBitPos += noBitsToWrite;
  writeValue <<= 32 - wBitPos;	
  writeValue |= hBitBuf->cache;
  
  while (wBitPos >= 8) 
  {
	  UWord8 tmp;
	  tmp = (UWord8)((writeValue >> 24) & 0xFF);
	  
	  *hBitBuf->pWriteNext++ = tmp;		
	  writeValue <<= 8;
	  wBitPos -= 8;
  }
  
  hBitBuf->wBitPos = wBitPos;
  hBitBuf->cache = writeValue;
                                                                                     
  return noBitsToWrite;
}




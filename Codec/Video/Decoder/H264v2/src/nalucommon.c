

#include "global.h"
#include "nalu.h"
#include "memalloc.h"


NALU_t *AllocNALU(H264DEC_G *pDecGlobal, int buffersize)
{
  NALU_t *n;

  if ((n = (NALU_t*)voMalloc(pDecGlobal,1* sizeof (NALU_t))) == NULL)
    no_mem_exit ("AllocNALU: n");

  n->max_size=buffersize;
  return n;
}



void FreeNALU(H264DEC_G *pDecGlobal, NALU_t *n)
{
  if (n != NULL)
  {
    voFree (pDecGlobal,n);
  }
}

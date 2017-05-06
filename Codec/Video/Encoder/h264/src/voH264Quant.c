/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#include "voH264EncGlobal.h"

VO_S32 Quant4x4_C( VO_S16 *dct, VO_U16 inv[16], VO_U16 offset[16] )
{
  VO_S32 i, nonzero = 0;
  VO_S32 coef;
  for( i = 0; i < 16; i++ )
  {
    coef = dct[i];
	if(coef != 0)
	{
	  if( coef > 0 ) 
      	coef = (offset[i] + coef) * inv[i] >> 16; 
      else 
      	coef = - ((offset[i] - coef) * inv[i] >> 16); 
	  if(coef)
		nonzero = 1;
	  dct[i] = (VO_S16)coef;
	}		
  }
  return nonzero;
}

VO_S32 Quant4x4Dc_C( VO_S16 *dct, VO_S32 inv, VO_S32 offset )
{
  VO_S32 i, nonzero = 0;
  VO_S32 coef;
  for( i = 0; i < 16; i++ )
  {
    coef = dct[i];
	if(coef != 0)
	{
	  if( coef > 0 ) 
      	coef = (offset + coef) * (inv) >> 16; 
      else 
      	coef = - ((offset - coef) * (inv) >> 16); 
	  if(coef)
		nonzero = 1;
	  dct[i] = (VO_S16)coef;
	}		
  }
  return nonzero;
}


VO_S32 Quant2x2Dc_C( VO_S16 *dct, VO_S32 inv, VO_S32 offset )
{
  VO_S32 i, nonzero = 0;
  VO_S32 coef;
  for( i = 0; i < 4; i++ )
  {
    coef = dct[i];
	if(coef != 0)
	{
	  if( coef > 0 ) 
      	coef = (offset + coef) * (inv) >> 16; 
      else 
      	coef = - ((offset - coef) * (inv) >> 16); 
	  if(coef)
		nonzero = 1;
	  dct[i] = (VO_S16)coef;
	}		
  }
  return nonzero;
}

void Dequant4x4_C( VO_S16 *dct, VO_S32 *dequant_mf, VO_S32 nQP )
{
  VO_S32* inv = &dequant_mf[nQP%6<<4];
  VO_S32 q_bits = nQP/6;
  VO_U32 i;

  for( i = 4; i != 0; i-- )
  {
	dct[0] = (VO_S16)(((( dct[0] * inv[0] ) << q_bits) + 8 ) >> 4);
	dct[1] = (VO_S16)(((( dct[1] * inv[1] ) << q_bits) + 8 ) >> 4);
	dct[2] = (VO_S16)(((( dct[2] * inv[2] ) << q_bits) + 8 ) >> 4);
	dct[3] = (VO_S16)(((( dct[3] * inv[3] ) << q_bits) + 8 ) >> 4);
	dct += 4;
	inv += 4;
  }
}


void Dequant4x4Dc_C( VO_S16* dct, VO_S32 *dequant_mf, VO_S32 nQP )
{
  const VO_S32 q_bits = nQP/6;
  VO_U32 i;
  VO_S32 inv = dequant_mf[nQP%6<<4] << q_bits;

  for( i = 4; i != 0; i-- )
  {
	dct[0] = (VO_S16)(( dct[0] * inv + 32 ) >> 6);
	dct[1] = (VO_S16)(( dct[1] * inv + 32 ) >> 6);
	dct[2] = (VO_S16)(( dct[2] * inv + 32 ) >> 6);
	dct[3] = (VO_S16)(( dct[3] * inv + 32 ) >> 6);
	dct += 4;
  }
}

const VO_U8 score_table_4x4[16] = { 3,2,2,1,1,1,0,0,0,0,0,0,0,0,0,0 };

VO_S32  NoneResDis( VO_S16 *dct, VO_S32 nMax )
{
  VO_S32 score = 0;
  VO_S32 index = nMax - 1;
  VO_S32 zero_count;

  while( index >= 0 && M32( &dct[index-1] ) == 0 )
    index -= 2;
  if( index >= 0 && dct[index] == 0 )
    index--;
  while( index >= 0 )
  {
    zero_count = 0;
    if( (unsigned)(dct[index--] + 1) > 2 )
      return 9; 
    while( index >= 0 && dct[index] == 0 )
    {
      index--;
      zero_count++;
    }
    score += score_table_4x4[zero_count];
  }
  return score;
}

#define SHIFT_MF(x,s) ((s)<=0 ? (x)<<-(s) : ((x)+(1<<((s)-1)))>>(s))
#define DIV(n,d) (((n) + ((d)>>1)) / (d))

static const VO_S32 DQ4Scale[6][3] =
{
  { 10, 13, 16 },{ 11, 14, 18 },{ 13, 16, 20 },
  { 14, 18, 23 },{ 16, 20, 25 },{ 18, 23, 29 }
};
static const VO_S32 Q4Scale[6][3] =
{
  { 13107, 8066, 5243 },{ 11916, 7490, 4660 },
  { 10082, 6554, 4194 },{  9362, 5825, 3647 },
  {  8192, 5243, 3355 },{  7282, 4559, 2893 },
};

VO_S32 InitCQM( H264ENC *pEncGlobal )
{
  VO_S32 q,i,j;  
  VO_S32 bias0 = 21 << 10;
  VO_S32 bias1 = 11 << 10;
  VO_S32 bias_min = 1 << 15;
                        
  //CHECKED_MALLOC( pEncGlobal->quant_table, 52*16*sizeof(VO_U16) );
  MEMORY_ALLOC( pEncGlobal->quant_table, pEncGlobal->buffer_total , 52*16*sizeof(VO_U16), pEncGlobal->buffer_used,VO_U16*);
  //CHECKED_MALLOC( pEncGlobal->dequant_table,6*16*sizeof(VO_S32) );
  MEMORY_ALLOC( pEncGlobal->dequant_table, pEncGlobal->buffer_total , 6*16*sizeof(VO_S32), pEncGlobal->buffer_used,VO_S32 *);
  //CHECKED_MALLOC( pEncGlobal->quant_bias[0], 52*16*sizeof(VO_U16) );
  MEMORY_ALLOC( pEncGlobal->quant_bias[0], pEncGlobal->buffer_total , 52*16*sizeof(VO_U16), pEncGlobal->buffer_used,VO_U16*);
  //CHECKED_MALLOC( pEncGlobal->quant_bias[1], 52*16*sizeof(VO_U16) );
  MEMORY_ALLOC( pEncGlobal->quant_bias[1], pEncGlobal->buffer_total , 52*16*sizeof(VO_U16), pEncGlobal->buffer_used,VO_U16*);

  for( q = 0; q < 6; q++ )
  {
    for( i = 0; i < 16; i++ )
    {
      j = (i&1) + ((i>>2)&1);
	  pEncGlobal->dequant_table[(q<<4)+i] = DQ4Scale[q][j] << 4;
    }
  }

  for( q = 0; q < 52; q++ )
  {
    for( i = 0; i < 16; i++ )
    {
      j = (i&1) + ((i>>2)&1);
	  j = SHIFT_MF(Q4Scale[q%6][j], q/6 - 1);
      pEncGlobal->quant_table[(q<<4)+i] = (VO_U16)j;
      pEncGlobal->quant_bias[0][(q<<4)+i] = (VO_U16)(AVC_MIN( DIV(bias0, j), bias_min/j ));
	  pEncGlobal->quant_bias[1][(q<<4)+i] = (VO_U16)(AVC_MIN( DIV(bias1, j), bias_min/j ));
    }
  }
	
    return 0;
}


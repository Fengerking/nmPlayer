
#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "count.h"


#define MASK      0x0001

/*---------------------------------------------------------------------------*
 * function:  bin2int                                                        *
 *            ~~~~~~~                                                        *
 * Read "no_of_bits" bits from the array bitstream[] and convert to short    *
 *--------------------------------------------------------------------------*/
Word16 Bin2int(         /* (o)  : recovered integer value              */
  Word16 no_of_bits,    /* (i)  : number of bits associated with value */
  Word16 *bitstream     /* (i)  : address where bits are read          */
)
{
  Word16 value, i;

#if (!FUNC_BIN2INT_OPT)
  value = 0;     
  for (i = 0; i < no_of_bits; i++)
  {
    value  = shl(value, 1);
    value  = add(value, (Word16)(*bitstream & MASK));    logic16();
    bitstream ++;
  }
#else
  value = 0;
  for (i = 0; i < no_of_bits; i++)
  {
    value = value << 1;
    value += (*bitstream++) & MASK;
  }
#endif

  return(value);
}

/*---------------------------------------------------------------------------*
 * function:  int2bin                                                        *
 *            ~~~~~~~                                                        *
 * Convert integer to binary and write the bits to the array bitstream[].    *
 * Most significant bits (MSB) are output first                              *
 *--------------------------------------------------------------------------*/

void Int2bin(
  Word16 value,         /* (i)  : value to be converted to binary      */
  Word16 no_of_bits,    /* (i)  : number of bits associated with value */
  Word16 *bitstream     /* (o)  : address where bits are written       */
)
{
  Word16 *pt_bitstream;
  Word16 i;

  pt_bitstream = bitstream + no_of_bits;

  for (i = 0; i < no_of_bits; i++)
  {
    --pt_bitstream;
    *pt_bitstream = (Word16)(value & MASK);    logic16();
    value = shr(value,1);
  }

}

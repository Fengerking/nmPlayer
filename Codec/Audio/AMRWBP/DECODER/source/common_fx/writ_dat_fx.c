
/*----------------------------------------------------------------------
 *
 *  FUNCTION:   writ_data
 *
 *  PURPOSE:  round array of float data to 16-bit words and write to the
 *            file "fp"
 *
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include "amr_plus_fx.h"
#include "typedef.h"
#include "basic_op.h"
#include "count.h"

void Writ_data(
  Word16 data[],         /* input : data           Q-1   */
  Word16 size,           /* input : number of samples */
  FILE  *fp,             /* output: file pointer      */
  Word16 Frame_scaling   /* input : frame scaling */
)
{
  Scale_sig(data, size, negate(Frame_scaling));
  fwrite(data, sizeof(Word16), size, fp);

  return;
}

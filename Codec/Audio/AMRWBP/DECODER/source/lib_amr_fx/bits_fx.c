/*------------------------------------------------------------------------*
 *                         BITS.C                                         *
 *------------------------------------------------------------------------*
 * Performs bit stream manipulation                                       *
 *------------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "cnst_wb_fx.h"
#include "bits_fx.h"
#include "acelp_fx.h"
#include "count.h"
#include "dtx_fx.h"

#include "mime_io.tab"

/*-----------------------------------------------------*
 * Parm_serial -> convert parameters to serial stream  *
 *-----------------------------------------------------*/

void Parm_serial(
     Word16 value,                         /* input : parameter value */
     Word16 no_of_bits,                    /* input : number of bits  */
     Word16 ** prms
)
{
    Word16 i, bit;

    *prms += no_of_bits;                   

    for (i = 0; i < no_of_bits; i++)
    {
        bit = (Word16) (value & 0x0001);   logic16();  /* get lsb */
        
        if (bit == 0)
            *--(*prms) = BIT_0;
        else
            *--(*prms) = BIT_1;
        value = shr(value, 1);             
    }
    *prms += no_of_bits;                   
    return;
}


/*----------------------------------------------------*
 * Serial_parm -> convert serial stream to parameters *
 *----------------------------------------------------*/

Word16 Serial_parm(                        /* Return the parameter    */
     Word16 no_of_bits,                    /* input : number of bits  */
     Word16 ** prms
)
{
    Word16 value, i;
    Word16 bit;

    value = 0;                             
    for (i = 0; i < no_of_bits; i++)
    {
        value = shl(value, 1);
        bit = *((*prms)++);                
        
        if (bit == BIT_1)
            value = add(value, 1);
    }
    return (value);
}

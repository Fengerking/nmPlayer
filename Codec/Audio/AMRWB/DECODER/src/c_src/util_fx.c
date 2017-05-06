
/*-------------------------------------------------------------------*
*                         UTIL.C									 *
*-------------------------------------------------------------------*
* Vector routines												     *
*-------------------------------------------------------------------*/
#include "typedef.h"
#include "basic_op.h"
#include "cnst_wb_fx.h"

/*-------------------------------------------------------------------*
* Function  Set zero()                                              *
*           ~~~~~~~~~~                                              *
* Set vector x[] to zero                                            *
*-------------------------------------------------------------------*/
void AMRWBDecSetZero(
			  Word16 x[],                           /* (o)    : vector to clear     */
			  Word16 L                              /* (i)    : length of vector    */
			  )
{
	Word32 num = (Word32)L;
	do{
		*x++ = 0;
	}while(--num !=0);
}

/*-------------------------------------------------------------------*
* Function  Copy:                                                   *
*           ~~~~~                                                   *
* Copy vector x[] to y[]                                            *
*-------------------------------------------------------------------*/
void Copy1(
		   Word16 x[],                           /* (i)   : input vector   */
		   Word16 y[],                           /* (o)   : output vector  */
		   Word16 L                              /* (i)   : vector length  */
		   )
{
	Word32	temp1,temp2,num;
	if(L&1)
	{
		temp1 = *x++;
		*y++ = temp1;
	}
	num = (Word32)L/2;
	temp1 = *x++;
	temp2 = *x++;
	do{
		*y++ = temp1;
		*y++ = temp2;
		temp1 = *x++;
		temp2 = *x++;
	}while(--num!=0);

}

void Copy(
		  const Word16 x[],                           /* (i)   : input vector   */
		  Word16 y[],                           /* (o)   : output vector  */
		  Word16 L                              /* (i)   : vector length  */
		  )
{
	Word32	temp1,temp2,num = (Word32)L/2;
	temp1 = *x++;
	temp2 = *x++;
	do{
		*y++ = temp1;
		*y++ = temp2;
		temp1 = *x++;
		temp2 = *x++;
	}while(--num!=0);
}


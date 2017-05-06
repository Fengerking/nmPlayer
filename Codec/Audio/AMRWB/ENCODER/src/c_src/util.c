/***********************************************************************
*                                                                      *
*         VisualOn, Inc. Confidential and Proprietary, 2003-2010       *
*                                                                      *
************************************************************************/
/***********************************************************************
*       File: util.c                                                   *
*                                                                      *
*       Description: Reset and Copy buffer                             *
*                                                                      *
************************************************************************/

#include "typedef.h"
#include "basic_op.h"

/***********************************************************************
* Function:  Set_zero()                                             *
* Description: Set vector x[] to zero                               *
************************************************************************/

void Set_zero(
		Word16 x[],                           /* (o)    : vector to clear     */
		Word16 L                              /* (i)    : length of vector    */
	     )
{
	Word32 num = (Word32)L;
	do{
		*x++ = 0;
	}while(--num !=0);
}

/*********************************************************************
* Function: Copy()                                                   *
*                                                                    *
* Description: Copy vector x[] to y[]                                *
*********************************************************************/
void voAMRWBEnc_Copy(
		Word16 x[],                           /* (i)   : input vector   */
		Word16 y[],                           /* (o)   : output vector  */
		Word16 L                              /* (i)   : vector length  */
	 )
{
	Word32	temp1,temp2,num;
	Word16  nTmp;
	switch (L&3)
	{
	case 3:
		nTmp = *x++;
		*y++ = nTmp;
	case 2:
		nTmp = *x++;
		*y++ = nTmp;
	case 1:
		nTmp = *x++;
		*y++ = nTmp;
	}

	num = (Word32)(L>>2);
	temp1 = *x++;
	temp2 = *x++;
	do{
		*y++ = temp1;
		*y++ = temp2;
		temp1 = *x++;
		temp2 = *x++;
		*y++ = temp1;
		*y++ = temp2;
		temp1 = *x++;
		temp2 = *x++;
	}while(--num!=0);
}




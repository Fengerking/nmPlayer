/*------------------------------------------------------------------------*
*                         P_MED_OL.C									  *
*------------------------------------------------------------------------*
* Compute the open loop pitch lag.										  *
*------------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "acelp_fx.h"
#include "cnst_wb_fx.h"
#include "oper_32b.h"
#include "count.h"
#include "math_op.h"
#include "p_med_ol.tab"

/*____________________________________________________________________
|
|
|  FUNCTION NAME median5
|
|      Returns the median of the set {X[-2], X[-1],..., X[2]},
|      whose elements are 16-bit integers.
|
|  INPUT
|      X[-2:2]   16-bit integers.
|
|  RETURN VALUE
|      The median of {X[-2], X[-1],..., X[2]}.
|_____________________________________________________________________
*/

Word16 voAMRWBPDecmedian5(Word16 x[])
{
	Word16 x1, x2, x3, x4, x5;
	Word16 tmp;

	x1 = x[-2];                            
	x2 = x[-1];                            
	x3 = x[0];                             
	x4 = x[1];                             
	x5 = x[2];                             

	if(x2 < x1)
	{
		tmp = x1;
		x1 = x2;
		x2 = tmp;                          
	}
	if(x3 < x1)
	{
		tmp = x1;
		x1 = x3;
		x3 = tmp;                          
	}
	if(x4 < x1)
	{
		tmp = x1;
		x1 = x4;
		x4 = tmp;                          
	}
	if(x5 < x1)
	{
		x5 = x1;                           
	}
	if(x3 < x2)
	{
		tmp = x2;
		x2 = x3;
		x3 = tmp;                          
	}
	if(x4 < x2)
	{
		tmp = x2;
		x2 = x4;
		x4 = tmp;                          
	}
	if(x5 < x2)
	{
		x5 = x2;                           
	}
	if(x4 < x3)
	{
		x3 = x4;                           
	}
	if(x5 < x3)
	{
		x3 = x5;                           
	}
	return (x3);
}



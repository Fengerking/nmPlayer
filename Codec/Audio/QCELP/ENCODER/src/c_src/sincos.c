#include "celp.h"
#include "coderate.h"

/* tmp = acos(double(i)/COSTABLE_LEN)/3.1415 * (1 << 15); */
short anticosvalue(short val, short scale)
{
	short retv;
	short shift;
	short index;
	short index_fra;

	if(val > 0)	
	{		
		shift = scale - COSTABLE_SCALE;
		index = (short)(val >> shift);		
		index_fra = val - (index << shift);
		retv = ((AntiCosTable_16[index+ 1] - AntiCosTable_16[index]) * index_fra) >> shift;
		retv += AntiCosTable_16[index];
	}
	else
	{
		shift = scale - COSTABLE_SCALE;
		index = (short)((-val) >> shift);		
		index_fra = -val - (index << shift);
		retv = ((AntiCosTable_16[index+ 1] - AntiCosTable_16[index]) * index_fra) >> shift;
		retv = 32767 - AntiCosTable_16[index] - retv;
	}

	return retv;
}
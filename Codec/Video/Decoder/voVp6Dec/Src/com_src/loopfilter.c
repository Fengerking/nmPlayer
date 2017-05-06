/****************************************************************************
*
*   Module Title :     loopfilter.c
*
*   Description  :     Loop filter functions.
*
****************************************************************************/


/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"

/****************************************************************************
*  Macros
****************************************************************************/
#define Mod8(x) ( (x) & 7 )

/****************************************************************************
*  Exports
****************************************************************************/
//static
extern UINT8 LimitVal_VP6[VAL_RANGE * 3];

#ifndef DISABLE_GENERIC_CODE
/****************************************************************************
 * 
 *  ROUTINE       : Bound
 *
 *  INPUTS        : UINT32 FLimit  : Limit to use in computing bounding value.
 *                  INT32  FiltVal : Value to have bounds applied to.
 *                  
 *  OUTPUTS       : None.
 *
 *  RETURNS       : INT32: 
 *
 *  FUNCTION      : Computes a bounded Filtval based on specified Flimit.
 *
 *  SPECIAL NOTES : None.
 *
 ****************************************************************************/
INLINE static
INT32 Bound ( UINT32 FLimit, INT32 FiltVal )
{
    INT32 Clamp;
    INT32 FiltSign;
    INT32 NewSign;

    Clamp = 2 * FLimit;

    // Next 3 lines are fast way to find abs...
    FiltSign = (FiltVal >> 31);         // Sign extension makes FiltSign all 0's or all 1's
    FiltVal ^= FiltSign;                // FiltVal is then 1's complement of value if -ve
    FiltVal -= FiltSign;                // Filtval = abs Filtval

    FiltVal *= (FiltVal < Clamp);       // clamp filter value to 2 times limit

    FiltVal -= FLimit;                  // subtract limit value 
    
    // Next 3 lines are fast way to find abs...
    NewSign = (FiltVal >> 31);          // Sign extension makes NewSign all 0's or all 1's
    FiltVal ^= NewSign;                 // FiltVal is then 1's complement of value if -ve
    FiltVal -= NewSign;                 // FiltVal = abs FiltVal

    FiltVal = FLimit - FiltVal;         // flimit - abs (filtVal - flimit)
    
    FiltVal += FiltSign;                // convert back to signed value
    FiltVal ^= FiltSign;            
    
    return FiltVal;
}



/****************************************************************************
 * 
 *  ROUTINE       : VP6_FilteringHoriz_12_C
 *
 *  INPUTS        : UINT32 QValue : Current quatizer level.
 *                  UINT8 *Src    : Pointer to data to be filtered.
 *                  INT32 Pitch   : Pitch of input data.
 *
 *  OUTPUTS       : None.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Applies vertical filter across horizontal edge inside
 *                  block with Q-dependent limits.
 *
 *  SPECIAL NOTES : 4-Tap filter used is (1, -3, 3, -1).
 *
 ****************************************************************************/                       
void VP6_FilteringHoriz_12_C ( UINT32 FLimit, UINT8 *Src, INT32 Pitch )
{    
    INT32  j;
	INT32  FiltVal;
    UINT8 *LimitTable = &LimitVal_VP6[VAL_RANGE];

	for ( j=0; j<12; j++ )
	{            
        // Apply 4-tap filter with rounding...
		FiltVal =  ( Src[-2]      - 
			        (Src[-1] * 3) +
			        (Src[ 0] * 3) - 
			         Src[1]  + 4) >> 3;
		if(FiltVal)
		{
			FiltVal = Bound ( FLimit, FiltVal );
			Src[-1] = LimitTable[(INT32)Src[-1] + FiltVal];
			Src[ 0] = LimitTable[(INT32)Src[ 0] - FiltVal];
		}
        Src += Pitch;
	}
}

/****************************************************************************
 * 
 *  ROUTINE       : VP6_FilteringVert_12_C
 *
 *  INPUTS        : UINT32 QValue : Current quatizer level.
 *                  UINT8 *Src    : Pointer to data to be filtered.
 *                  INT32 Pitch   : Pitch of input data.
 *
 *  OUTPUTS       : None.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Applies vertical filter across horizontal edge inside
 *                  block with Q-dependent limits.
 *
 *  SPECIAL NOTES : 4-Tap filter used is (1, -3, 3, -1).
 *
 ****************************************************************************/                       
void VP6_FilteringVert_12_C ( UINT32 FLimit, UINT8 *Src, INT32 Pitch )
{    
    INT32  j;
	INT32  FiltVal;
    UINT8 *LimitTable = &LimitVal_VP6[VAL_RANGE];

	for ( j=0; j<12; j++ )
	{            
 		FiltVal = ( (INT32)Src[- (2 * Pitch)] - 
        		   ((INT32)Src[- Pitch] * 3)  + 
		           ((INT32)Src[0] * 3)        - 
			        (INT32)Src[Pitch] + 4 ) >> 3;
		if(FiltVal)
		{
			FiltVal = Bound ( FLimit, FiltVal );
			Src[-Pitch] = LimitTable[(INT32)Src[-Pitch] + FiltVal];
			Src[     0] = LimitTable[(INT32)Src[     0] - FiltVal];
		}
	
        Src++;
	}
}
#endif



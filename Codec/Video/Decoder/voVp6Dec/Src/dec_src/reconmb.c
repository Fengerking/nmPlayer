/**************************************************************************** 
*
*   Module Title :     reconmb.c 
*
*   Description  :     Filtering functions for decoding.
*
***************************************************************************/
/****************************************************************************
*  Header Files
***************************************************************************/ 
#include "pbdll.h"
#include "dec_gfuncs.h"

/****************************************************************************
*  Macros
****************************************************************************/        
#define FILTER_WEIGHT 128

#define FILTER_SHIFT  7

#define MIN(a, b) ( ( a < b ) ? a : b )

/****************************************************************************
* Imports
****************************************************************************/ 
extern UINT32 Var16Point ( UINT8 *DataPtr, INT32 SourceStride );
/****************************************************************************
*  Module Statics
****************************************************************************/

typedef const INT32 FILTER_DATA ;
/****************************************************************************
* Fractional pixel prediction filtering...
****************************************************************************/
extern void FilterBlock1d_wRecon(UINT8  *SrcPtr, UINT8  *dstPtr, UINT32 PixelStep, UINT32 SrcPixelsPerLine, UINT32 LineStep,FILTER_DATA *Filter);
extern void FilterBlock1dBil_wRecon(UINT8  *SrcPtr,	UINT8  *dstPtr,INT32	PixelStep,INT32	SrcStride,INT32	LineStep,FILTER_DATA  *Filter);
extern void FilterBlock2d_wRecon(UINT8 *SrcPtr,UINT8 *dstPtr,UINT32 SrcPixelsPerLine,UINT32 LineStep,FILTER_DATA * HFilter,FILTER_DATA * VFilter);
extern void FilterBlock2dBil_wRecon(UINT8 *SrcPtr,UINT8 *dstPtr,UINT32 SrcPixelsPerLine,UINT32 LineStep,FILTER_DATA * HFilter,FILTER_DATA * VFilter);
extern void IDCTvp6_Block1x1a(INT16 *input, UINT8 *dst, INT32 DestStride, const UINT8 *src, INT32 stride);
extern void IDCTvp6_Block4x4a(INT16 *input, UINT8 *dst, INT32 DestStride, const UINT8 *src, INT32 stride);
extern void IDCTvp6_Block8x8a(INT16 *input, UINT8 *dst, INT32 DestStride, const UINT8 *src, INT32 stride);
#if !defined(VOARMV7)
extern void IDCTvp6_Block1x1(INT16 *input, UINT8 *dst, INT32 DestStride, const UINT8 *src, INT32 stride);
extern void IDCTvp6_Block4x4(INT16 *input, UINT8 *dst, INT32 DestStride, const UINT8 *src, INT32 stride);
extern void IDCTvp6_Block8x8(INT16 *input, UINT8 *dst, INT32 DestStride, const UINT8 *src, INT32 stride);
#endif

/****************************************************************************
* 
*  ROUTINE       : FilterBlock1d_wRecon
*  
*  INPUTS        : UINT8 *SrcPtr           : Pointer to source block.
*                  INT16 *diffPtr          : Pointer to Recon block.
*                  UINT32 PixelStep        : 1 for horizontal filtering,
*                                            SrcPixelsPerLine for vertical filtering.
*                  UINT32 SrcPixelsPerLine : Stride of source block.
*                  UINT32 LineStep         : Stride of output block.
*                  INT32 *Filter           : Array of 4 filter taps.
*
*  OUTPUTS       : UINT8 *dstPtr           : Final output buffer.
*                  
*  RETURNS       : void.
*
*  FUNCTION      : Applies a 1-D 4-tap filter to the source block in 
*                  either horizontal or vertical direction to produce the
*                  filtered output block.
*
*  SPECIAL NOTES : Four filter taps should sum to FILTER_WEIGHT.
*                  PixelStep defines whether the filter is applied 
*                  horizontally (PixelStep=1) or vertically (PixelStep=stride).
*                  It defines the offset required to move from one input 
*                  to the next.
*
*                  Adds the output of the filter to the recon buffer.  Must
*                  be saturated before and after adding the recon buffer in.
*
****************************************************************************/
void FilterBlock1d_wRecon_C 
( 
	UINT8  *SrcPtr,
	UINT8  *dstPtr,
	UINT32 PixelStep,
	UINT32 SrcPixelsPerLine,
	UINT32 LineStep,
	FILTER_DATA *Filter 
)
{
	UINT32 i, j;
	INT32  Temp; 
	// SrcPtr is increment each time in the inner loop, 8 in all.
	SrcPixelsPerLine -= 8;
	
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			// Apply filter...
			Temp =  (-((INT32)SrcPtr[-(INT32)PixelStep]	* Filter[0])) +
					((INT32)SrcPtr[0]					* Filter[1]) +
					((INT32)SrcPtr[PixelStep]			* Filter[2]) +
					(-((INT32)SrcPtr[2*PixelStep]		* Filter[3])) + 
					(FILTER_WEIGHT >> 1);       // Rounding
			
			// Normalize back to 0-255
			Temp = Temp >> FILTER_SHIFT;
			if ( Temp < 0 ) Temp = 0;
			else if ( Temp > 255 ) Temp = 255;
			
			dstPtr[j] = (UINT8)Temp;
			SrcPtr++;
		}
		
		// Next row...
		SrcPtr  += SrcPixelsPerLine;
		//diffPtr += 8;
		dstPtr  += LineStep;
	}
}

/****************************************************************************
* 
*  ROUTINE       : FilterBlock2dFirstPass_wRecon
*  
*  INPUTS        : UINT8 *SrcPtr           : Pointer to source block.
*                  UINT32 SrcPixelsPerLine : Stride of source block.
*                  INT32 *Filter           : Array of 4 filter taps.
*
*  OUTPUTS       : INT32 *OutputPtr        : Intermediary buffer.
*
*  RETURNS       : void.
*
*  FUNCTION      : Applies a 1-D 4-tap filter to the source block in 
*                  horizontal direction to produce the filtered output block. 
*                  Used to implement first-pass of 2-D separable filter.
*
*  SPECIAL NOTES : Produces INT32 output to retain precision for next pass.
*                  Four filter taps should sum to FILTER_WEIGHT.
*
*                  
*
****************************************************************************/
void FilterBlock2dFirstPass_wRecon_C
( 
	UINT8 *SrcPtr,
	INT32 *OutputPtr,
	UINT32 SrcPixelsPerLine,
	FILTER_DATA *Filter 
)
{
	UINT32 i, j;
	INT32  Temp;
	
	SrcPixelsPerLine -= 8;

	for (i = 0; i < 11; i++)				// for Height
	{
		for (j = 0; j < 8; j++)				// for Width
		{
			// Apply filter
			Temp =  (-((INT32)SrcPtr[-1] * Filter[0])) +
					((INT32)SrcPtr[0]  * Filter[1]) +
					((INT32)SrcPtr[1]  * Filter[2]) +
					(-((INT32)SrcPtr[2]  * Filter[3])) + 
					(FILTER_WEIGHT >> 1);      // Rounding
			
			// Normalize back to 0-255
			Temp = Temp >> FILTER_SHIFT;
			if ( Temp < 0 ) 
				Temp = 0;
			else if ( Temp > 255 ) 
				Temp = 255;
			
			OutputPtr[j] = Temp;
			SrcPtr++;
		}
		
		// Next row...
		SrcPtr    += SrcPixelsPerLine;
		OutputPtr += 8;
	}
}

/****************************************************************************
* 
*  ROUTINE       : FilterBlock2dSecondPass_wRecon
*  
*  INPUTS        : INT32 *SrcPtr           : Pointer to source block.
*                  INT16 *diffPtr          : Pointer to Recon block.
*                  UINT32 LineStep         : Stride of output block.
*                  INT32 *Filter           : Array of 4 filter taps.
*
*  OUTPUTS       : UINT8  *dstPtr	       : Pointer to output block.
*
*  RETURNS       : void.
*
*  FUNCTION      : Applies a 1-D 4-tap filter to the source block in 
*                  vertical direction to produce the filtered output block.
*                  Used to implement second-pass of 2-D separable filter.
*
*  SPECIAL NOTES : Requires 32-bit input as produced by FilterBlock2dFirstPass.
*                  Four filter taps should sum to FILTER_WEIGHT.
*
*                  Hard coded widths and heights to take into account the
*                  size of the temporary buffer coming from FirstPass.
*
****************************************************************************/
void FilterBlock2dSecondPass_wRecon_C
(
	INT32  *SrcPtr,
	UINT8  *dstPtr,
	UINT32 LineStep,
	FILTER_DATA *Filter 
)
{
	UINT32 i,j;
	INT32  Temp;
	
	
	for ( i=0; i < 8; i++ )
	{
		for ( j = 0; j < 8; j++ )
		{
			// Apply filter
			Temp =  (-((INT32)SrcPtr[-8]	* Filter[0])) +
					((INT32)SrcPtr[0]	* Filter[1]) +
					((INT32)SrcPtr[8]	* Filter[2]) +
					(-((INT32)SrcPtr[16]	* Filter[3])) +
					(FILTER_WEIGHT >> 1);   // Rounding
			
			// Normalize back to 0-255
			Temp = Temp >> FILTER_SHIFT;
			if ( Temp < 0 ) Temp = 0;
			else if ( Temp > 255 ) Temp = 255;
			
			dstPtr[j] = (UINT8)Temp;
			SrcPtr++;
		}
		
		// Next row...
		//diffPtr += 8;
		dstPtr  += LineStep;
	}
}

/****************************************************************************
* 
*  ROUTINE       : FilterBlock2d_wRecon
*  
*  INPUTS        : UINT8  *SrcPtr          : Pointer to source block.
*                  UINT32 SrcPixelsPerLine : Stride of input block.
*                  INT32  *HFilter         : Array of 4 horizontal filter taps.
*                  INT32  *VFilter         : Array of 4 vertical filter taps.
*					
*  OUTPUTS       : UINT8 *dstPtr           : Pointer to filtered block.
*
*  RETURNS       : void
*
*  FUNCTION      : 2-D filters an 8x8 input block by applying a 4-tap 
*                  filter horizontally followed by a 4-tap filter vertically
*                  on the result.
*
*  SPECIAL NOTES : The intermediate horizontally filtered block must produce
*                  3 more points than the input block in each column. This
*                  is to ensure that the 4-tap filter has one extra data-point
*                  at the top & 2 extra data-points at the bottom of each 
*                  column so filter taps do not extend beyond data. Thus the
*                  output of the first stage filter is an 8x11 (HxV) block.
*
****************************************************************************/
void FilterBlock2d_wRecon_C 
(
	UINT8 *SrcPtr,
	UINT8 *dstPtr,
	UINT32 SrcPixelsPerLine,
	UINT32 LineStep,
	FILTER_DATA * HFilter,
	FILTER_DATA * VFilter 
)
{
	INT32 FData[BLOCK_HEIGHT_WIDTH*11];	// Temp data bufffer used in filtering

	// First filter 1-D horizontally...
	FilterBlock2dFirstPass_wRecon_C ( SrcPtr-SrcPixelsPerLine, FData, SrcPixelsPerLine, HFilter );
	
	// then filter verticaly...
	FilterBlock2dSecondPass_wRecon_C ( FData+BLOCK_HEIGHT_WIDTH, dstPtr, LineStep, VFilter );
}

/****************************************************************************
* 
*  ROUTINE       : FilterBlock1dBil_wRecon
*  
*  INPUTS        : UINT8  *SrcPtr          : Pointer to source block.
*                  INT16 *diffPtr          : Pointer to Recon block.
*                  UINT32 PixelStep        : 1 for horizontal filtering,
*                                            SrcPixelsPerLine for vertical filtering.
*                  UINT32 SrcPixelsPerLine : Stride of source block.
*                  UINT32 LineStep         : Stride of output block.
*                  INT32 *Filter           : Array of 4 filter taps.
*					
*  OUTPUTS       : UINT8 *dstPtr           : Pointer to filtered block.
*
*  RETURNS       : void
*
*  FUNCTION      : Applies a 2-tap 1-D bi-linear filter to input block in
*                  either horizontal or vertical direction.
*
*  SPECIAL NOTES : PixelStep defines whether the filter is applied 
*                  horizontally (PixelStep=1) or vertically (PixelStep=stride).
*                  It defines the offset required to move from one input 
*                  to the next.
*
****************************************************************************/
void FilterBlock1dBil_wRecon_C
(
	UINT8  *SrcPtr, 
	UINT8  *dstPtr,
	INT32	PixelStep,
	INT32	SrcStride,
	INT32	LineStep,
	FILTER_DATA  *Filter 
)
{
	UINT32 i, j;
	// accomodate incrementing SrcPtr++ each time.
	SrcStride -= 8;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			INT16 Temp;
			// Apply filter 
			// NOTE: Rounding doesn't improve accuracy but is 
			//       easier to implement on certain platforms.
			Temp = (INT16)((((INT32)SrcPtr[0]         * Filter[0]) +
							((INT32)SrcPtr[PixelStep] * Filter[1]) +
							(FILTER_WEIGHT/2) ) >> FILTER_SHIFT );
			dstPtr[j] = (UINT8)Temp;
			SrcPtr++;
		}
		// Next row...
		SrcPtr   += SrcStride;
		//diffPtr += BLOCK_HEIGHT_WIDTH;
		dstPtr += LineStep;
	}
}

/****************************************************************************
* 
*  ROUTINE       : FilterBlock2dBilFirstPass_wRecon
*  
*  INPUTS        : UINT8  *SrcPtr          : Pointer to source block.
*                  UINT32 SrcPixelsPerLine : Stride of input block.
*                  INT32  *Filter          : Array of 2 bi-linear filter taps.
*					
*  OUTPUTS       : INT32 *OutputPtr        : Pointer to filtered block.
*
*  RETURNS       : void
*
*  FUNCTION      : Applies a 1-D 2-tap bi-linear filter to the source block in 
*                  horizontal direction to produce the filtered output block.
*                  Used to implement first-pass of 2-D separable filter.
*
*  SPECIAL NOTES : Produces INT32 output to retain precision for next pass.
*                  Two filter taps should sum to FILTER_WEIGHT.
*
****************************************************************************/
void FilterBlock2dBilFirstPass_wRecon_C
( 
	UINT8 *SrcPtr,  
	INT32 *OutputPtr, 
	UINT32 SrcPixelsPerLine, 
	FILTER_DATA *Filter 
)
{
	UINT32 i, j;

	SrcPixelsPerLine -= 8;

	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 8 ; j++)
		{
			// Apply bilinear filter
			OutputPtr[j] = (((INT32)SrcPtr[0]	* Filter[0]) +
							((INT32)SrcPtr[1]	* Filter[1]) +
							(FILTER_WEIGHT/2) ) >> FILTER_SHIFT;
			SrcPtr++;
		}
		
		// Next row...
		SrcPtr    += SrcPixelsPerLine;
		OutputPtr += 8;
	}
}

/****************************************************************************
* 
*  ROUTINE       : FilterBlock2dBilSecondPass_wRecon
*  
*  INPUTS        : UINT32 *SrcPtr          : Pointer to source block.
*                  INT16 *diffPtr          : Pointer to Recon block.
*                  UINT32 LineStep         : Stride of output block.
*                  INT32 *Filter           : Array of 4 filter taps.
*
*  OUTPUTS       : UINT8  *dstPtr	       : Pointer to output block.
*
*  RETURNS       : void
*
*  FUNCTION      : Applies a 1-D 2-tap bi-linear filter to the source block in 
*                  vertical direction to produce the filtered output block.
*                  Used to implement second-pass of 2-D separable filter.
*                  
*  SPECIAL NOTES : Requires 32-bit input as produced by FilterBlock2dBil_FirstPass.
*                  Two filter taps should sum to FILTER_WEIGHT.

****************************************************************************/
void FilterBlock2dBilSecondPass_wRecon_C
(
	UINT32 *SrcPtr,
	UINT8  *dstPtr,
	INT32	LineStep,
	FILTER_DATA  *Filter 
)
{
	UINT32 i,j;
	INT32  Temp;

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			// Apply filter
			Temp =  ((INT32)SrcPtr[0] * Filter[0]) +
					((INT32)SrcPtr[8] * Filter[1]) +
					(FILTER_WEIGHT/2);
			
			Temp = (UINT16)(Temp >> FILTER_SHIFT);
			
			dstPtr[j] = (UINT8)Temp;
			SrcPtr++;
		}
		
		//diffPtr += BLOCK_HEIGHT_WIDTH;
		dstPtr  += LineStep;
	}
}

/****************************************************************************
* 
*  ROUTINE       : FilterBlock2dBil_wRecon
*  
*  INPUTS        : UINT8  *SrcPtr          : Pointer to source block.
*                  UINT32 SrcPixelsPerLine : Stride of input block.
*                  INT32  *HFilter         : Array of 4 horizontal filter taps.
*                  INT32  *VFilter         : Array of 4 vertical filter taps.
*					
*  OUTPUTS       : UINT8 *dstPtr           : Pointer to filtered block.
*
*  RETURNS       : void
*
*  FUNCTION      : 2-D filters an 8x8 input block by applying a 2-tap 
*                  bi-linear filter horizontally followed by a 2-tap 
*                  bi-linear filter vertically on the result.
*
*  SPECIAL NOTES : The intermediate horizontally filtered block must produce
*                  1 more point than the input block in each column. This
*                  is to ensure that the 2-tap filter has one extra data-point
*                  at the top of each column so filter taps do not extend 
*                  beyond data. Thus the output of the first stage filter
*                  is an 8x9 (HxV) block.
*
****************************************************************************/
void FilterBlock2dBil_wRecon_C 
(
	UINT8 *SrcPtr,
	UINT8 *dstPtr,
	UINT32 SrcPixelsPerLine,
	UINT32 LineStep,
	FILTER_DATA * HFilter,
	FILTER_DATA * VFilter 
)
{
	INT32 FData[BLOCK_HEIGHT_WIDTH*11];	// Temp data bufffer used in filtering
	
	
	// First filter 1-D horizontally...
	FilterBlock2dBilFirstPass_wRecon_C ( SrcPtr, FData, SrcPixelsPerLine, HFilter );
	
	// then 1-D vertically...
	FilterBlock2dBilSecondPass_wRecon_C ( (UINT32 *)FData, dstPtr, LineStep, VFilter );
}

/****************************************************************************
* 
*  ROUTINE       : FilterBlock_wRecon
*  
*  INPUTS        : INT32 dstStride      ; output stride
*                  INT16 *diffPtr       ; pointer to buffer for reconning with
*                  UINT8 *ReconPtr1     : Pointer to first 8x8 input block.
*                  UINT8 *ReconPtr2     : Pointer to second 8x8 input block.
*                  UINT32 PixelsPerLine : Stride for ReconPtr1 & ReconPtr2.
*				   INT32 ModX           : Fractional part of x-component of motion vector.
*                  INT32 ModY           : Fractional part of y-component of motion vector.
*                  BOOL UseBicubic      : TRUE=Bicubic, FALSE=Bi-Linear filter.
*                  UINT8 BicubicAlpha   :
*				
*  OUTPUTS       : UINT8 *dstPtr        : Pointer to 8x8 filtered and reconned block.
*
*  RETURNS       : void
*
*  FUNCTION      : Produces a filtered fractional pel prediction block
*                  using bilinear or bicubic filters.
*
*  SPECIAL NOTES : ReconPtr1 & ReconPtr2 point to blocks that bracket the
*                  position of the fractional pixel motion vector. These
*                  two blocks are combined using either a bi-linear or
*                  bi-cubic filter to produce the output prediction block
*                  for this motion vector.
*                  ModX, ModY are used for filter selection--see code
*                  comment for definition.
*
****************************************************************************/
void FiltPredictedBlock_wRecon
( 
	UINT8 *dstPtr, 
	INT32 dstStride, 

	UINT8 *ReconPtr1,
	UINT8 *ReconPtr2,
	UINT32 PixelsPerLine,
	INT32 ModX,
	INT32 ModY,

	BOOL UseBicubic,
	UINT8 BicubicAlpha
)
{
	int diff;
	
	// ModX and ModY are the bottom three bits of the signed motion vector
	// components (in 1/8th pel units). This works out to be what we want
	// --despite the pointer swapping that goes on below.
	// For example...
	// if MV x-component is +ve then ModX = x%8.
	// if MV x-component is -ve then ModX = 8+(x%8), where X%8 is in the range -7 to -1.
	
	// Swap pointers to ensure that ReconPtr1 is "smaller than",
	// i.e. above, left, above-right or above-left, ReconPtr1
	diff = ReconPtr2 - ReconPtr1;

	if ( diff<0 ) 
	{
		// ReconPtr1>ReconPtr2, so swap...
		UINT8 *temp = ReconPtr1;
		ReconPtr1 = ReconPtr2;
		ReconPtr2 = temp;
		diff = (int)(ReconPtr2-ReconPtr1);
	} 

	if ( (diff==1)|| ( diff == (int)(PixelsPerLine) ))
	{   
		INT32 ModX_Y;
		if(diff==1)
			ModX_Y = ModX;
		else
			ModX_Y = ModY;
		// Fractional pixel in horizontal only...											            
		if ( UseBicubic )
			FilterBlock1d_wRecon ( ReconPtr1, dstPtr, diff, PixelsPerLine, dstStride, BicubicFilterSet[BicubicAlpha][ModX_Y] );
		else
			FilterBlock1dBil_wRecon ( ReconPtr1, dstPtr, diff, PixelsPerLine, dstStride, BilinearFilters[ModX_Y]);
	}
	else 
	{
		diff -= PixelsPerLine;
		diff >>= 1;					//should be 0 or -1
		
		ReconPtr1 += diff;	
		if ( UseBicubic )
			FilterBlock2d_wRecon  ( ReconPtr1, dstPtr, PixelsPerLine, dstStride, BicubicFilterSet[BicubicAlpha][ModX], BicubicFilterSet[BicubicAlpha][ModY] );
		else
			FilterBlock2dBil_wRecon ( ReconPtr1, dstPtr, PixelsPerLine, dstStride, BilinearFilters[ModX], BilinearFilters[ModY] );
	}


}

/****************************************************************************
 * 
 *  ROUTINE       : ReconIntra_MB
 *
 *  INPUTS        : PB_INSTANCE *pbi  : Pointer to decoder instance.
 *
 *  OUTPUTS       : none.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Adds 128 to the diffBuffer (the output from the idct)
 *                  saturates it between 0 and 255.  Stores it into the 
 *                  recon buffer.
 *
 *  SPECIAL NOTES : None.
 *
 ****************************************************************************/
int ReconIntra_MB( PB_INSTANCE *pbi )
{
    UINT32 i;
    UINT32 bp;
    UINT32 thisRecon;
    UINT8 *ReconPtr;
    UINT32 EOBPos;
    bp = 0;
    do
    {
        thisRecon = pbi->mbi.blockDxInfo[bp].thisRecon;
        ReconPtr = (UINT8 *)&pbi->ThisFrameRecon[thisRecon];
        i = 8;
//
        EOBPos = pbi->mbi.blockDxInfo[bp].EobPos;
		
        if ( EOBPos <= 1 )
        {
			IDCTvp6_Block1x1a(pbi->mbi.blockDxInfo[bp].coeffsPtr, ReconPtr, pbi->mbi.blockDxInfo[bp].CurrentReconStride, NULL, 0);
        }
        else if ( EOBPos < 15 && (pbi->mbi.blockDxInfo[bp].coeffsPtr[32]==0))
        {
			IDCTvp6_Block4x4a(pbi->mbi.blockDxInfo[bp].coeffsPtr, ReconPtr, pbi->mbi.blockDxInfo[bp].CurrentReconStride, NULL, 0);
        }
        else 
        {
			IDCTvp6_Block8x8a(pbi->mbi.blockDxInfo[bp].coeffsPtr, ReconPtr, pbi->mbi.blockDxInfo[bp].CurrentReconStride, NULL, 0);
        }
    } while(++bp < 6);

	return 0;

}

/****************************************************************************
 * 
 *  ROUTINE       : ReconInter_MB
 *
 *  INPUTS        : PB_INSTANCE *pbi  : Pointer to decoder instance.
 *
 *  OUTPUTS       : none.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Adds thisRecont to the diffBuffer (the output from the idct)
 *                  saturates it between 0 and 255.  Stores it into the 
 *                  recon buffer.
 *
 *  SPECIAL NOTES : None.
 *
 ****************************************************************************/
int ReconInter_MB( PB_INSTANCE *pbi )
{
    UINT32 i;
    UINT32 bp;
    UINT32 thisRecon;
    UINT8 *ReconPtr;
    UINT8 *RefPtr;

    UINT8 *BaseRefPtr;
    UINT32 EOBPos;
 //   BLOCK_DX_INFO *bdi;

    // Which buffer are we working on?
    BaseRefPtr = pbi->LastFrameRecon;
    if ( VP6_Mode2Frame[pbi->mbi.Mode] == 2 ) 
    {
        BaseRefPtr = pbi->GoldenFrame;
    }
    
    bp = 0;
    do
    {
        thisRecon = pbi->mbi.blockDxInfo[bp].thisRecon;

        ReconPtr = (UINT8 *)&pbi->ThisFrameRecon[thisRecon];
        RefPtr = (UINT8 *)&BaseRefPtr[thisRecon];
        i = 8;
//
        EOBPos = pbi->mbi.blockDxInfo[bp].EobPos;
		
        if ( EOBPos <= 1 )
        {
			IDCTvp6_Block1x1a(pbi->mbi.blockDxInfo[bp].coeffsPtr, ReconPtr, pbi->mbi.blockDxInfo[bp].CurrentReconStride, RefPtr, pbi->mbi.blockDxInfo[bp].CurrentReconStride);
        }
        else if ( EOBPos < 15 && (pbi->mbi.blockDxInfo[bp].coeffsPtr[32]==0))
        {
			IDCTvp6_Block4x4a(pbi->mbi.blockDxInfo[bp].coeffsPtr, ReconPtr, pbi->mbi.blockDxInfo[bp].CurrentReconStride, RefPtr, pbi->mbi.blockDxInfo[bp].CurrentReconStride);
        }
        else 
        {
			IDCTvp6_Block8x8a(pbi->mbi.blockDxInfo[bp].coeffsPtr, ReconPtr, pbi->mbi.blockDxInfo[bp].CurrentReconStride, RefPtr, pbi->mbi.blockDxInfo[bp].CurrentReconStride);
        }
    } while(++bp < 6);
	return 0;
}
/****************************************************************************
 * 
 *
 ****************************************************************************/
int VP6_PredictFiltered_MB(PB_INSTANCE *pbi) 
{
	UINT8 *SrcPtr;
	UINT8 *TempBuffer;
	UINT32 TempPtr1;
	UINT32 TempPtr2;
	INT32  ModX, ModY;
	UINT32 IVar;
	UINT32 Stride;
	
	UINT32 MvShift;
	UINT32 MvModMask;
	UINT32 bp;
    UINT32 EOBPos;

//	INT16 idct_zero[64] = {0};
	
	// Which buffer are we working on?
	SrcPtr = pbi->LastFrameRecon;
	if ( VP6_Mode2Frame[pbi->mbi.Mode] == 2 ) 
	{
		SrcPtr = pbi->GoldenFrame;
	}

	pbi->RefFrameReconLow = SrcPtr;
	pbi->RefFrameReconHigh = pbi->RefFrameReconLow + pbi->mbi.blockDxInfo[0].FrameReconStride*(pbi->Configuration.VideoFrameHeight + UMV_BORDER*2) ;
	//pbi->RefFrameReconHigh = pbi->RefFrameReconLow + pbi->mbi.blockDxInfo[0].FrameReconStride*((pbi->MBRows<<4) - 7) - 8; //zou ?

	if (pbi->UseLoopFilter != NO_LOOP_FILTER)
	{
		pbi->RefFrameReconLow += (2*pbi->mbi.blockDxInfo[0].FrameReconStride + 2);
		pbi->RefFrameReconHigh -= (4*pbi->mbi.blockDxInfo[0].FrameReconStride + 8);  
	}

	bp = 0;
	do
	{
		MvShift = pbi->mbi.blockDxInfo[bp].MvShift;
		MvModMask = pbi->mbi.blockDxInfo[bp].MvModMask;
		
		// No loop filtering in simple profile
		if (pbi->UseLoopFilter == NO_LOOP_FILTER)
		{
			INT32  mVx, mVy;
			INT32  mx = pbi->mbi.blockDxInfo[bp].Mv.x;
			INT32  my = pbi->mbi.blockDxInfo[bp].Mv.y;
			
			// Mask off fractional pel bits.
			ModX = (mx & MvModMask);
			ModY = (my & MvModMask); 
			
			// Calculate full pixel motion vector position 
			mx += (MvModMask&(mx>>31));
			my += (MvModMask&(my>>31));
			
			mVx = (mx >> MvShift);
			mVy = (my >> MvShift);
			
			// Set up a pointer into the recon buffer
			TempBuffer = SrcPtr + pbi->mbi.blockDxInfo[bp].thisRecon + (pbi->mbi.blockDxInfo[bp].FrameReconStride * mVy + mVx);
#ifdef STABILITY
			if (((TempBuffer < pbi->RefFrameReconLow) || (TempBuffer > pbi->RefFrameReconHigh)) && (bp < 4)){ //zou delete
				return  -11;
			}
#endif
			Stride = pbi->mbi.blockDxInfo[bp].CurrentReconStride;
			TempPtr1 = TempPtr2 = 0;
		}
		else
		{
			// Loop filter the block
			// Want to pass the address of TempBuffer because it will be modified according to the offset
			// so that an aligned copy can be made in the Copy16x12 function.
			TempBuffer = pbi->LoopFilteredBlock;
			if(VP6_PredictFiltered( pbi, SrcPtr + pbi->mbi.blockDxInfo[bp].thisRecon, 
				pbi->mbi.blockDxInfo[bp].Mv.x, pbi->mbi.blockDxInfo[bp].Mv.y, bp, &TempBuffer )<0)
				return  -1;
			
			Stride = 16;
			TempPtr1 = 2*16+2;		// Offset into the 12x12 loop filtered buffer
			TempPtr2 = TempPtr1;
			
			// Mask off fractional pel bits.
			ModX = (pbi->mbi.blockDxInfo[bp].Mv.x & MvModMask);
			ModY = (pbi->mbi.blockDxInfo[bp].Mv.y & MvModMask); 
		}
		
		// determine if we have a fractional pixel move in the x direction
		if ( ModX )
		{
			TempPtr2 += ( pbi->mbi.blockDxInfo[bp].Mv.x > 0 )*2 -1;        
		}
		
		// handle fractional pixel motion in Y
		if ( ModY )
		{
			TempPtr2 += (( pbi->mbi.blockDxInfo[bp].Mv.y > 0 ) * 2 - 1)*Stride;
		}

#ifdef STABILITY
		if (pbi->UseLoopFilter == NO_LOOP_FILTER)
		{
			if (((TempBuffer+TempPtr2 < pbi->RefFrameReconLow)|| (TempBuffer +TempPtr2 > pbi->RefFrameReconHigh))  && (bp < 4) ) 
			{
				return  -11;
			}
			if (((TempBuffer+TempPtr1 < pbi->RefFrameReconLow)|| (TempBuffer +TempPtr1 > pbi->RefFrameReconHigh))  && (bp < 4) ) 
			{
				return  -11;
			}
		}
#endif
		
		// put the results back into the real reconstruction buffer
		if ( TempPtr1 != TempPtr2 ) 
		{
			INT16 *diffPtr;// = idct_zero;//pbi->mbi.blockDxInfo[bp].idctOutputPtr;
			UINT8 *dstPtr = &pbi->ThisFrameRecon[pbi->mbi.blockDxInfo[bp].thisRecon];
			INT32 dstStride = pbi->mbi.blockDxInfo[bp].CurrentReconStride;
			
			// The FilterBlock selects a filter based upon a ModX and ModY value that are at 1/8 point 
			// precision. Because U and V are subsampled the vector is already at the right precision 
			// for U and V but for Y we have to multiply by 2.
			if ( bp < 4 )
			{
                UINT32 BicMvSizeLimit;

				// Filterblock expects input at 1/8 pel resolution (hence << 1 for Y)
				ModX = ModX << 1;
				ModY = ModY << 1; 
				
                if ( pbi->PredictionFilterMode == AUTO_SELECT_PM )
			    {
                //PredictionFilterMvSizeThresh = 4 for widths greater than 480, otherwise 3

				//  Work out the Mv size limit for selecting bicubic
                // Convert to a value in 1/4 pel units
				BicMvSizeLimit = (1 << (pbi->PredictionFilterMvSizeThresh - 1)) << 2;			 
				
				// Only use bicubic on shortish vectors
				if( ((UINT32)abs(pbi->mbi.blockDxInfo[bp].Mv.x) > BicMvSizeLimit) || 
					((UINT32)abs(pbi->mbi.blockDxInfo[bp].Mv.y) > BicMvSizeLimit)  )
				{
					FiltPredictedBlock_wRecon( dstPtr, dstStride, 
						&TempBuffer[TempPtr1], &TempBuffer[TempPtr2], 
						Stride, ModX, ModY, FALSE, pbi->PredictionFilterAlpha);
				}
				// Should we use a variance test for bicubic as well
				else if ( pbi->PredictionFilterVarThresh != 0 )
				{
					IVar = Var16Point( &TempBuffer[TempPtr1], Stride );
					FiltPredictedBlock_wRecon( dstPtr, dstStride,  
						&TempBuffer[TempPtr1], &TempBuffer[TempPtr2], 
						Stride, ModX, ModY, (IVar >= pbi->PredictionFilterVarThresh), pbi->PredictionFilterAlpha );
				}
				else
					FiltPredictedBlock_wRecon( dstPtr, dstStride,  
					&TempBuffer[TempPtr1], &TempBuffer[TempPtr2], 
					Stride, ModX, ModY, TRUE, pbi->PredictionFilterAlpha );
                }
			    else  
				    FiltPredictedBlock_wRecon( dstPtr, dstStride,  
                        &TempBuffer[TempPtr1], &TempBuffer[TempPtr2], 
                        Stride, ModX, ModY, (pbi->PredictionFilterMode == BICUBIC_ONLY_PM), pbi->PredictionFilterAlpha );
			}
			else
            {
				FiltPredictedBlock_wRecon( dstPtr, dstStride,  
				&TempBuffer[TempPtr1], &TempBuffer[TempPtr2], 
				Stride, ModX, ModY, FALSE, pbi->PredictionFilterAlpha );
			}
			diffPtr = pbi->mbi.blockDxInfo[bp].coeffsPtr;
			EOBPos = pbi->mbi.blockDxInfo[bp].EobPos;
			
			if ( EOBPos <= 1 )
			{
				//if((diffPtr[0]+15)>>5) //zou  ??
				IDCTvp6_Block1x1a(diffPtr, dstPtr, dstStride, dstPtr, dstStride);
			}
        else if ( EOBPos < 15 && (diffPtr[32]==0))
			{
				IDCTvp6_Block4x4a(diffPtr, dstPtr, dstStride, dstPtr, dstStride);
			}
			else 
			{
				IDCTvp6_Block8x8a(diffPtr, dstPtr, dstStride, dstPtr, dstStride);
			}
		}
		// No fractional pels
		else
		{
			UINT8 *dstPtr = &pbi->ThisFrameRecon[pbi->mbi.blockDxInfo[bp].thisRecon];
			INT32 dstStride = pbi->mbi.blockDxInfo[bp].CurrentReconStride;
			UINT32 thisRecon;
			INT16 *diffPtr;	
			thisRecon = pbi->mbi.blockDxInfo[bp].thisRecon;		

			diffPtr = pbi->mbi.blockDxInfo[bp].coeffsPtr;
			EOBPos = pbi->mbi.blockDxInfo[bp].EobPos;
			
			if ( EOBPos <= 1 )
			{
				IDCTvp6_Block1x1(diffPtr, dstPtr, dstStride, &TempBuffer[TempPtr1], Stride);
			}
        else if ( EOBPos < 15 && (diffPtr[32]==0))
			{
				IDCTvp6_Block4x4(diffPtr, dstPtr, dstStride, &TempBuffer[TempPtr1], Stride);
			}
			else 
			{
				IDCTvp6_Block8x8(diffPtr, dstPtr, dstStride, &TempBuffer[TempPtr1], Stride);
			}
		}
		
	}
	while(++bp < 6);

	return 0;

}

/****************************************************************************
 * 
 *
 ****************************************************************************/
int VP6_PredictFiltered_MB_60(PB_INSTANCE *pbi) 
{
	UINT8 *SrcPtr;
	UINT8 *TempBuffer;
	UINT32 TempPtr1;
	UINT32 TempPtr2;
	INT32  ModX, ModY;
	UINT32 Stride;
	
	UINT32 MvShift;
	UINT32 MvModMask;
	UINT32 bp;
    UINT32 EOBPos;

//	INT16 idct_zero[64] = {0};

	// Which buffer are we working on?
	SrcPtr = pbi->LastFrameRecon;
	if ( VP6_Mode2Frame[pbi->mbi.Mode] == 2 ) 
	{
		SrcPtr = pbi->GoldenFrame;
	}
	pbi->RefFrameReconLow = SrcPtr;
    pbi->RefFrameReconHigh = pbi->RefFrameReconLow + pbi->mbi.blockDxInfo[0].FrameReconStride*(pbi->Configuration.VideoFrameHeight + UMV_BORDER*2 ); 
	//pbi->RefFrameReconHigh = pbi->RefFrameReconLow + pbi->mbi.blockDxInfo[0].FrameReconStride*(pbi->Configuration.VideoFrameHeight - 8) - 8; //zou ?

	bp = 0;
	do
	{
		INT32  mVx, mVy;
		INT32  mx = pbi->mbi.blockDxInfo[bp].Mv.x;
		INT32  my = pbi->mbi.blockDxInfo[bp].Mv.y;

		MvShift = pbi->mbi.blockDxInfo[bp].MvShift;
		MvModMask = pbi->mbi.blockDxInfo[bp].MvModMask;
		
		// Mask off fractional pel bits.
		ModX = (mx & MvModMask);
		ModY = (my & MvModMask); 
		
		// Calculate full pixel motion vector position 
		mx += (MvModMask&(mx>>31));
		my += (MvModMask&(my>>31));
		
		mVx = (mx >> MvShift);
		mVy = (my >> MvShift);
		
		// Set up a pointer into the recon buffer
		TempBuffer = SrcPtr + pbi->mbi.blockDxInfo[bp].thisRecon + (pbi->mbi.blockDxInfo[bp].FrameReconStride * mVy + mVx);

#ifdef STABILITY
		if (((TempBuffer < pbi->RefFrameReconLow)|| (TempBuffer > pbi->RefFrameReconHigh))  && (bp < 4) ) 
		{
			return  -11;
		}
#endif
		
		Stride = pbi->mbi.blockDxInfo[bp].CurrentReconStride;
		TempPtr1 = TempPtr2 = 0;
		
		// determine if we have a fractional pixel move in the x direction
		if ( ModX )
		{
			TempPtr2 += ( pbi->mbi.blockDxInfo[bp].Mv.x > 0 )*2 -1;        
		}
		
		// handle fractional pixel motion in Y
		if ( ModY )
		{
			TempPtr2 += (( pbi->mbi.blockDxInfo[bp].Mv.y > 0 ) * 2 - 1)*Stride;
		}
#ifdef STABILITY
		if (((TempBuffer+TempPtr2 < pbi->RefFrameReconLow)|| (TempBuffer +TempPtr2 > pbi->RefFrameReconHigh))  && (bp < 4) ) 
		{
			return   -11;
		}
		if (((TempBuffer+TempPtr1 < pbi->RefFrameReconLow)|| (TempBuffer +TempPtr1 > pbi->RefFrameReconHigh))  && (bp < 4) ) 
		{
			return  -11;
		}
#endif
		
		// put the results back into the real reconstruction buffer
		if ( TempPtr1 != TempPtr2 ) 
		{
			INT16 *diffPtr;// = idct_zero;//pbi->mbi.blockDxInfo[bp].idctOutputPtr;
			UINT8 *dstPtr = &pbi->ThisFrameRecon[pbi->mbi.blockDxInfo[bp].thisRecon];
			INT32 dstStride = pbi->mbi.blockDxInfo[bp].CurrentReconStride;
			
			// The FilterBlock selects a filter based upon a ModX and ModY value that are at 1/8 point 
			// precision. Because U and V are subsampled the vector is already at the right precision 
			// for U and V but for Y we have to multiply by 2.
			if ( bp < 4 )
			{
				// Filterblock expects input at 1/8 pel resolution (hence << 1 for Y)
				ModX = ModX << 1;
				ModY = ModY << 1; 
			}

			FiltPredictedBlock_wRecon( dstPtr, dstStride,  
			&TempBuffer[TempPtr1], &TempBuffer[TempPtr2], 
			Stride, ModX, ModY, BILINEAR_ONLY_PM, pbi->PredictionFilterAlpha );

			diffPtr = pbi->mbi.blockDxInfo[bp].coeffsPtr;
			EOBPos = pbi->mbi.blockDxInfo[bp].EobPos;
			
			if ( EOBPos <= 1 )
			{
				//if((diffPtr[0]+15)>>5) //zou
				IDCTvp6_Block1x1a(diffPtr, dstPtr, dstStride, dstPtr, dstStride);
			}
        else if ( EOBPos < 15 && (diffPtr[32]==0))
			{
				IDCTvp6_Block4x4a(diffPtr, dstPtr, dstStride, dstPtr, dstStride);
			}
			else 
			{
				IDCTvp6_Block8x8a(diffPtr, dstPtr, dstStride, dstPtr, dstStride);
			}
		
		}
		// No fractional pels
		else
		{
			UINT8 *dstPtr = &pbi->ThisFrameRecon[pbi->mbi.blockDxInfo[bp].thisRecon];
			INT32 dstStride = pbi->mbi.blockDxInfo[bp].CurrentReconStride;
			UINT32 thisRecon;
			INT16 *diffPtr;	
			thisRecon = pbi->mbi.blockDxInfo[bp].thisRecon;

			diffPtr = pbi->mbi.blockDxInfo[bp].coeffsPtr;
			EOBPos = pbi->mbi.blockDxInfo[bp].EobPos;
			
			if ( EOBPos <= 1 )
			{
				IDCTvp6_Block1x1(diffPtr, dstPtr, dstStride, &TempBuffer[TempPtr1], Stride);
			}
			else if ( EOBPos < 15 && (diffPtr[32]==0))
			{
				IDCTvp6_Block4x4(diffPtr, dstPtr, dstStride, &TempBuffer[TempPtr1], Stride);
			}
			else 
			{
				IDCTvp6_Block8x8(diffPtr, dstPtr, dstStride, &TempBuffer[TempPtr1], Stride);
			}
		}	
	}
	while(++bp < 6);
	return 0;
}

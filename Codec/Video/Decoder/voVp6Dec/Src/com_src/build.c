/****************************************************************************
*
*   Module Title :     build.c
*
*   Description  :     
*
****************************************************************************/


/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"


/****************************************************************************
*  Constants
****************************************************************************/     


/****************************************************************************
* 
*  ROUTINE       :     BuildScanOrder
*
*  INPUTS        :     PB_INSTANCE *pbi : Pointer to instance of a decoder.
*                      UINT8 *ScanBands : Pointer to array containing band for 
*                                         each DCT coeff position. 
*
*  OUTPUTS       :     None
*
*  RETURNS       :     void
*
*  FUNCTION      :     Builds a custom dct scan order from a set of band data.
*
*  SPECIAL NOTES :     None. 
*
****************************************************************************/
void 
VP6_BuildScanOrder( PB_INSTANCE *pbi, UINT8 *ScanBands )
{
	UINT32 i, j;
	UINT32 ScanOrderIndex = 1;
	UINT32 MaxOffset;

	// DC is fixed
	pbi->ModifiedScanOrder[0] = 0;

	// Create a scan order where within each band the coefs are in ascending order
	// (in terms of their original zig-zag positions).
	for ( i = 0; i < SCAN_ORDER_BANDS; i++ )
	{
		for ( j = 1; j < BLOCK_SIZE; j++ )
		{
			if ( ScanBands[j] == i )
			{
				pbi->ModifiedScanOrder[ScanOrderIndex] = j;
				ScanOrderIndex++;
			}
		}
	}

	// For each of the positions in the modified scan order work out the 
	// worst case EOB offset in zig zag order. This is used in selecting
    // the appropriate idct variant
	for ( i = 0; i < BLOCK_SIZE; i++ )
	{
		MaxOffset = 0;
		for ( j = 0; j <= i; j++ )
		{
			if ( pbi->ModifiedScanOrder[j] > MaxOffset )
				MaxOffset = pbi->ModifiedScanOrder[j];
		}

		pbi->EobOffsetTable[i] = MaxOffset;

  	//if(pbi->Vp3VersionNo > 6)
   //         pbi->EobOffsetTable[i] = MaxOffset+1;

    }
}


/****************************************************************************
*        
*   Module Title :	   Decodemv.c     
*
*   Description  :     Functions for decoding modes and motion vectors.
*
*   Copyright (c) 1999 - 2005  On2 Technologies Inc. All Rights Reserved.
*
****************************************************************************/


/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"					
#include "decodemode.h" 
#include "decodemv.h"

/****************************************************************************
*  Macros
****************************************************************************/        


/****************************************************************************
*  Exports
****************************************************************************/        

/**************************************************************************** 
 * 
 *  ROUTINE       :     VP6_ConfigureMvEntropyDecoder
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *                      UINT8 FrameType  : Type of the frame.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Builds the MV entropy decoding tree.
 *
 *  SPECIAL NOTES :     None. 
 *
***************************************************************************/
void VP6_ConfigureMvEntropyDecoder( PB_INSTANCE *pbi, UINT8 FrameType )
{
	int i;
    (void)FrameType;

	// This funciton is not called at all for a BASE_FRAME
	// Read any changes to mv probabilities.
	for ( i = 0; i < 2; i++ )
	{
		// Short vector probability
		if ( VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][0]) )
		{
			pbi->IsMvShortProb[i] = VP6_bitread( &pbi->br, PROB_UPDATE_BASELINE_COST ) << 1;
			if ( pbi->IsMvShortProb[i] == 0 )
				pbi->IsMvShortProb[i] = 1;
		}

		// Sign probability
		if ( VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][1]) )
		{
			pbi->MvSignProbs[i] = VP6_bitread( &pbi->br, PROB_UPDATE_BASELINE_COST ) << 1;
			if ( pbi->MvSignProbs[i] == 0 )
				pbi->MvSignProbs[i] = 1;
		}
	}

	// Short vector tree node probabilities
	for ( i = 0; i < 2; i++ )
	{
		UINT32 j;
		UINT32 MvUpdateProbsOffset = 2;				// Offset into MvUpdateProbs[i][]

		for ( j = 0; j < 7; j++ )
		{
			if ( VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][MvUpdateProbsOffset]) )
			{
				pbi->MvShortProbs[i][j] = VP6_bitread( &pbi->br, PROB_UPDATE_BASELINE_COST ) << 1;
				if ( pbi->MvShortProbs[i][j] == 0 )
					pbi->MvShortProbs[i][j] = 1;
			}
			MvUpdateProbsOffset++;
		}
	}

	// Long vector tree node probabilities
	for ( i = 0; i < 2; i++ )
	{
		UINT32 j;
		UINT32 MvUpdateProbsOffset = 2 + 7;

		for ( j = 0; j < LONG_MV_BITS; j++ )
		{
			if ( VP6_DecodeBool(&pbi->br, VP6_MvUpdateProbs[i][MvUpdateProbsOffset]) )
			{
				pbi->MvSizeProbs[i][j] = VP6_bitread( &pbi->br, PROB_UPDATE_BASELINE_COST ) << 1;
				if ( pbi->MvSizeProbs[i][j] == 0 )
					pbi->MvSizeProbs[i][j] = 1;
			}
			MvUpdateProbsOffset++;
		}
	}
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_decodeMotionVector 
 *
 *  INPUTS        :     PB_INSTANCE *pbi  : Pointer to decoder instance.
 *						CODING_MODE Mode  : MV coding mode.
 *						
 *  OUTPUTS       :     MOTION_VECTOR *mv : Returned motion vector.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Decodes a motion vector from the bitstream.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6_decodeMotionVector
(
	PB_INSTANCE *pbi,
	MOTION_VECTOR *mv,
	CODING_MODE Mode 
)
{
	UINT32 i;
	INT32  Vector = 0;
	INT32  SignBit = 0;
	INT32  MvOffsetX = 0;
	INT32  MvOffsetY = 0;

	// Work out how the MV was coded so that the appropriate origin offset can be applied
	if ( Mode == CODE_INTER_PLUS_MV )
	{	
        // Normal Inter MV
		if ( pbi->mbi.NearestMvIndex < MAX_NEAREST_ADJ_INDEX )
		{
			MvOffsetX = pbi->mbi.NearestInterMVect.x;
			MvOffsetY = pbi->mbi.NearestInterMVect.y;
		}
	}
	else
	{	
        // Golden Frame MV
		if ( pbi->mbi.NearestGMvIndex < MAX_NEAREST_ADJ_INDEX )
		{
			MvOffsetX = pbi->mbi.NearestGoldMVect.x;
			MvOffsetY = pbi->mbi.NearestGoldMVect.y;
		}
	}

	for ( i = 0; i < 2; i++ )
	{
		Vector = 0;

		// Is the vector a small vector or a large vector
		if ( !VP6_DecodeBool(&pbi->br, pbi->IsMvShortProb[i]) )
		{
			// Small magnitude vector
			if ( VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][0] ) )
			{
				Vector += (1 << 2);
				if ( VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][4]) )
				{
					Vector += (1 << 1);
					Vector += VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][6]);
				}
				else
				{
					Vector += VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][5]);
				}
			}
			else
			{
				if ( VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][1]) )
				{
					Vector += (1 << 1);
					Vector += VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][3]);
				}
				else
				{
					Vector = VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][2]);
				}
			}
		}
		else
		{
			// Large magnitude vector
			Vector = VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][0] );
			Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][1] ) << 1);
			Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][2] ) << 2);

			Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][7] ) << 7);
			Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][6] ) << 6);
			Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][5] ) << 5);
			Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][4] ) << 4);

			// If none of the higher order bits are set then this bit is implicit
			if ( Vector & 0xF0 )
				Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][3] ) << 3);
			else
				Vector += 0x08;
		}

		// Read the sign bit if needed.
		if ( Vector != 0 )
		{
			SignBit = VP6_DecodeBool(&pbi->br, pbi->MvSignProbs[i]);

			if ( SignBit )
				Vector = -Vector;
		}

		if ( i )
			mv->y = Vector + MvOffsetY;
		else
			mv->x = Vector + MvOffsetX;
    }
}


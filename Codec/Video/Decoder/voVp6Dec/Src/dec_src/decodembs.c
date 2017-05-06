/****************************************************************************
*
*   Module Title :     Decodembs.c
*
*   Description  :     Compressor functions for block order transmittal
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
#include "on2_mem.h"

extern void VP6_BuildHuffTree ( HUFF_NODE *hn, unsigned int *counts, int values );
extern void VP6_BuildHuffLookupTable ( HUFF_NODE *HuffTreeRoot, UINT16 *HuffTable );


int (*reconFunc[MAX_MODES])( PB_INSTANCE *pbi );
/****************************************************************************
*  Imports
****************************************************************************/     
extern INT32 VP6_ReadTokensPredictA_MB(PB_INSTANCE *pbi);
extern INT32 ReadHuffTokensPredictA_MB(PB_INSTANCE *pbi);


/****************************************************************************
*  Module constants
****************************************************************************/     
static const INT32 VP6_CoeffToHuffBand[65] = 
{  
   -1,0,1,1,1,2,2,2,
	2,2,2,3,3,3,3,3,
	3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3
};


/****************************************************************************
 * 
 *  ROUTINE       :     nDecodeBool
 *
 *  INPUTS        :     BITREADER *br   : Wrapper for the encoded data buffer.
 *                      int probability : Probability that next symbol in Boolean 
 *                                        Coded buffer is a 0.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     Value of the next encoded token 0 or 1 (as int)
 *
 *  FUNCTION      :     Extracts next token (0 or 1) from the Boolean encoded data buffer.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
FORCEINLINE
int nDecodeBool ( BOOL_CODER *br, int probability ) 
{
	unsigned int split;
    int count = br->count;
    unsigned int range = br->range;
    unsigned int value = br->value;

	// perform the actual decoding
	split = 1 +  (((range-1) * probability) >> 8);	

	if ( value >= split<<24 )
	{
		value -= (split<<24);
		range = range - split;

        while(range < 0x80 )
        {
	        range += range;
	        value += value;
	        
	        if ( !--count ) 
	        {
		        count  = 8;
		        value |= br->buffer[br->pos];
		        br->pos++;
	        }
        }

        br->count = count;
        br->value = value;
        br->range = range;

        return 1;

	}
	range = split;

    while(range < 0x80 )
	{
		range += range;
		value += value;
		
		if ( !--count ) 
		{
			count  = 8;
			value |= br->buffer[br->pos];
			br->pos++;
		}
	}
    br->count = count;
    br->value = value;
    br->range = range;
	return 0;
} 


/****************************************************************************
* 
*  ROUTINE       :     VP6_ConvertDecodeBoolTrees
*
*  INPUTS        :     PB_INSTANCE *pbi
*
*  OUTPUTS       :     None
*
*  RETURNS       :     None.
*
*  FUNCTION      :     Convert trees used for Bool coding to set of token probs.
*
*  SPECIAL NOTES :     None. 
*
*
*  ERRORS        :     None.
*
****************************************************************************/
void 
VP6_ConvertDecodeBoolTrees ( PB_INSTANCE *pbi )
{
    UINT32  i;
	UINT32	Plane;
	UINT32	Band;
	INT32   Prec;
	HUFF_INSTANCE	*huff = pbi->huff;

    // Convert bool tree node probabilities into array of token 
    // probabilities. Use these to create a set of Huffman codes
	// DC
    for ( Plane = 0; Plane < 2; Plane++ )
    {
		BoolTreeToHuffCodes ( pbi->DcProbs+DCProbOffset(Plane,0), huff->DcHuffProbs[Plane] );
		VP6_BuildHuffTree ( huff->DcHuffTree[Plane], huff->DcHuffProbs[Plane], MAX_ENTROPY_TOKENS );

		// fast huffman lookup
		VP6_BuildHuffLookupTable (huff->DcHuffTree[Plane], huff->DcHuffLUT[Plane]);        
    }
    
    // ZEROS
    for ( i = 0; i < ZRL_BANDS; i++ )
	{
		ZerosBoolTreeToHuffCodes ( pbi->ZeroRunProbs[i], huff->ZeroHuffProbs[i] );
		VP6_BuildHuffTree ( huff->ZeroHuffTree[i], huff->ZeroHuffProbs[i], 9 );

		// fast huffman lookup
		VP6_BuildHuffLookupTable (huff->ZeroHuffTree[i], huff->ZeroHuffLUT[i]);
    }

    // AC
    for ( Prec = 0; Prec < PREC_CASES; Prec++ )
	{
		// Baseline probabilities for each AC band.
		for ( Plane = 0; Plane < 2; Plane++ )
		{
			for ( Band = 0; Band < VP6_AC_BANDS; Band++ )
            {
				BoolTreeToHuffCodes ( pbi->AcProbs+ACProbOffset(Plane,Prec,Band,0), huff->AcHuffProbs[Prec][Plane][Band] );
				VP6_BuildHuffTree ( huff->AcHuffTree[Prec][Plane][Band], huff->AcHuffProbs[Prec][Plane][Band], MAX_ENTROPY_TOKENS );
				
				// fast huffman lookup
				VP6_BuildHuffLookupTable (huff->AcHuffTree[Prec][Plane][Band], huff->AcHuffLUT[Prec][Plane][Band]);
            }
        }
    }
}

/****************************************************************************
* 
*  ROUTINE       :     VP6_ConfigureEntropyDecoder
*
*  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
*                      UINT8 FrameType  : Type of frame.
*
*  OUTPUTS       :     None.
*
*  RETURNS       :     void
*
*  FUNCTION      :     Configure entropy subsystem ready for decode
*
*  SPECIAL NOTES :     None. 
*
****************************************************************************/
void VP6_ConfigureEntropyDecoder( PB_INSTANCE *pbi, UINT8 FrameType )
{
	UINT32	i,j;
	UINT32  Plane;
	UINT32  Band;
	INT32   Prec;
	UINT8   LastProb[MAX_ENTROPY_TOKENS-1];
	
	// Clear down Last Probs data structure
	on2_memset( LastProb, 128, MAX_ENTROPY_TOKENS-1 );

	// Read in the Baseline DC probabilities and initialise the DC context for Y and then UV plane
	for ( Plane = 0; Plane < 2; Plane++ )
	{
		// If so then read them in.
		for ( i = 0; i < MAX_ENTROPY_TOKENS-1; i++ )
		{
			if ( nDecodeBool(&pbi->br, VP6_DcUpdateProbs[Plane][i] ) )
			{
				// 0 is not a legal value, clip to 1.
				LastProb[i] = VP6_bitread( &pbi->br, PROB_UPDATE_BASELINE_COST ) << 1;
				LastProb[i] += ( LastProb[i] == 0 );
				pbi->DcProbs[DCProbOffset(Plane,i)] = LastProb[i];

			}
			else if ( FrameType == BASE_FRAME )
			{
				pbi->DcProbs[DCProbOffset(Plane,i)] = LastProb[i];
			}
		}
	}

	// Set Zero run probabilities to defaults if this is a key frame
	if ( FrameType == BASE_FRAME )
	{
		on2_memcpy( pbi->ZeroRunProbs, ZeroRunProbDefaults, sizeof(pbi->ZeroRunProbs) );
	}

	// If this frame contains updates to the scan order then read them
	if ( nDecodeBool( &pbi->br, 128 ) )
	{
		// Read in the AC scan bands and build the custom scan order
		for ( i = 1; i < BLOCK_SIZE; i++ )
		{
			// Has the band for this coef been updated ?
			if ( nDecodeBool( &pbi->br, ScanBandUpdateProbs[i] ) )
				pbi->ScanBands[i] = VP6_bitread( &pbi->br, SCAN_BAND_UPDATE_BITS );
		}
		// Build the scan order
		VP6_BuildScanOrder( pbi, pbi->ScanBands );
	}

	// Update the Zero Run probabilities
	for ( i = 0; i < ZRL_BANDS; i++ )
	{
		for ( j = 0; j < ZERO_RUN_PROB_CASES; j++ )
		{
			if ( nDecodeBool( &pbi->br, ZrlUpdateProbs[i][j] )  )
			{
				// Probabilities sent
				pbi->ZeroRunProbs[i][j] = VP6_bitread( &pbi->br, PROB_UPDATE_BASELINE_COST ) << 1;
				pbi->ZeroRunProbs[i][j] += ( pbi->ZeroRunProbs[i][j] == 0 );
			}
		}		
	}

	// Read in the Baseline AC band probabilities and initialise the appropriate contexts
	// Prec=0 means last token in current block was 0: Prec=1 means it was 1. Prec=2 means it was > 1
	for ( Prec = 0; Prec < PREC_CASES; Prec++ )
	{
		//PrecNonZero = ( Prec > 0 ) ? 1 : 0;
		for ( Plane = 0; Plane < 2; Plane++ )
		{
			for ( Band = 0; Band < VP6_AC_BANDS; Band++ )
			{
				// If so then read them in.
				for ( i = 0; i < MAX_ENTROPY_TOKENS-1; i++ )
				{
					if ( nDecodeBool(&pbi->br, VP6_AcUpdateProbs[Prec][Plane][Band][i] ) )
					{
						// Probabilities transmitted at reduced resolution. 
						// 0 is not a legal value, clip to 1.
						LastProb[i] = VP6_bitread( &pbi->br, PROB_UPDATE_BASELINE_COST ) << 1;
						LastProb[i] += ( LastProb[i] == 0 );                        
						pbi->AcProbs[ACProbOffset(Plane,Prec,Band,i)] = LastProb[i];
					}
					else if ( FrameType == BASE_FRAME )
					{
						pbi->AcProbs[ACProbOffset(Plane,Prec,Band,i)] = LastProb[i];
					}
				}
			}
		}
	} 

	// Create all the context specific propabilities based upon the new baseline data
	VP6_ConfigureContexts(pbi);

}


#define HIGHBITDUPPED(X) (((signed short) X)  >> 15)


/****************************************************************************
 * 
 *  ROUTINE       :     VP6_PredictDC_MB
 *
 *  INPUTS        :     PB_INSTANCE *pbi     : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Predicts coefficients in this macroblock based on the 
 *                      contexts provided.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void 
VP6_PredictDC_MB(PB_INSTANCE *pbi)
{
	UINT8 Frame = VP6_Mode2Frame[pbi->mbi.Mode];
	Q_LIST_ENTRY *  LastDC;
	BLOCK_CONTEXT*  Above;
	BLOCK_CONTEXT *  Left;

    BLOCK_DX_INFO *bdi = pbi->mbi.blockDxInfo;
    BLOCK_DX_INFO *bdiEnd = bdi + 6;

	do
    {
    	INT32 Avg;
	
    	LastDC = bdi->LastDc;
		Above = bdi->Above;
 		Left = bdi->Left;

		Avg = LastDC[Frame];

		if(Frame == Left->Frame) 
		{
			Avg = Left->Dc;
		}
		if(Frame == Above->Frame) 
		{
			Avg = Above->Dc;
            if(Frame == Left->Frame)
            {
            	Avg += Left->Dc;
                Avg += (HIGHBITDUPPED(Avg)&1);
			    Avg >>= 1;

            }
		}

		bdi->coeffsPtr[0] += Avg;
		LastDC[Frame] = bdi->coeffsPtr[0];
		
        Above->Dc = bdi->coeffsPtr[0];
		Above->Frame = Frame;

		Left->Dc = bdi->coeffsPtr[0];
		Left->Frame = Frame;
		bdi->coeffsPtr[0] *=bdi->dequantPtr[0];

	} while(++bdi < bdiEnd);

}
 


/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DecodeMacroBlock
 *
 *  INPUTS        :     PB_INSTANCE *pbi  : Pointer to decoder instance.
 *                      UINT32 MBrow      : Row of MBs that block is in.
 *                  	UINT32 MBcol      : Col of MBs that block is in.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Decodes a single MacroBlock.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
INLINE INT32 VP6_DecodeMacroBlock ( PB_INSTANCE *pbi, UINT32 MBrow, UINT32 MBcol )
{
    //***********************************************************************
    // Copy the existing structures into what we have now I'll fix this next.
//	int i=0;// zou
	INT32 returnvalue =0; 

	pbi->mbi.Mode = CODE_INTRA;

	if(pbi->FrameType != BASE_FRAME )
	{
		VP6_decodeModeAndMotionVector ( pbi, MBrow, MBcol );
	}
	// read tokens from the bitstream and convert to coefficients.
	//if ( (( pbi->MultiStream || (pbi->VpProfile == SIMPLE_PROFILE) )) && pbi->UseHuffman ) //?
	if(pbi->UseHuffman)
    {
#ifdef STABILITY
		if(pbi->br3.position == NULL)
			return -11;
#endif
        returnvalue=ReadHuffTokensPredictA_MB(pbi);
		if(returnvalue != 0)
			return returnvalue;
    }
    else
    {
#ifdef STABILITY //1245
		if(pbi->mbi.br->pos > ((unsigned int)pbi->mbi.br->buffer_end-(unsigned int)pbi->mbi.br->buffer) + 32)
			return -11;
#endif
        returnvalue=VP6_ReadTokensPredictA_MB(pbi);
		if(returnvalue != 0)
			return returnvalue;
    }

    VP6_PredictDC_MB(pbi);

	returnvalue = reconFunc[pbi->mbi.Mode](pbi);
	if(returnvalue != 0)
			return returnvalue;

	return 0;
}

/****************************************************************************
 * 
 *  ROUTINE       :     DecodeFrame
 *
 *  INPUTS        :     PB_INSTANCE *pbi  : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Decodes all the MacroBlocks of a frame.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
INT32 VP6_DecodeFrameMbs ( PB_INSTANCE *pbi )
{
    //UINT32 blockIndex;
	INT32 returnvalue = 0;

	if(pbi->FrameType != BASE_FRAME )
	{

		VP6_DecodeModeProbs(pbi);
#ifdef STABILITY
		if(pbi->br.errorflag ==1)
			return -11;
#endif

		VP6_ConfigureMvEntropyDecoder( pbi, pbi->FrameType );
        pbi->LastMode = CODE_INTER_NO_MV;
#ifdef STABILITY
		if(pbi->br.errorflag ==1)
			return -11;
#endif
	}
	else
	{
		memcpy ( pbi->probXmitted,VP6_BaselineXmittedProbs,sizeof(pbi->probXmitted));

		memcpy ( pbi->IsMvShortProb, DefaultIsShortProbs, sizeof(pbi->IsMvShortProb) );
		memcpy ( pbi->MvShortProbs, DefaultMvShortProbs, sizeof(pbi->MvShortProbs) );
		memcpy ( pbi->MvSignProbs, DefaultSignProbs, sizeof(pbi->MvSignProbs) );
		memcpy ( pbi->MvSizeProbs, DefaultMvLongProbs, sizeof(pbi->MvSizeProbs) );

		memset ( pbi->predictionMode,1,sizeof(char)*pbi->MacroBlocks );

		memcpy( pbi->ScanBands, DefaultScanBands, sizeof(pbi->ScanBands) );

		// Build the scan order
		VP6_BuildScanOrder( pbi, pbi->ScanBands );


	}

	VP6_ConfigureEntropyDecoder( pbi, pbi->FrameType ); 

    {
        UINT32  i;

        for(i=0;i<64;i++)
            pbi->MergedScanOrder[i] = pbi->quantizer->transIndex[pbi->ModifiedScanOrder[i]];


		// Create Huffman codes for tokens based on tree probabilities
		if ( pbi->UseHuffman )
		{
			HUFF_INSTANCE	*huff = pbi->huff;

			VP6_ConvertDecodeBoolTrees ( pbi );

			for(i = 64; i < 64+65; i++)
			{
				pbi->MergedScanOrder[i] = VP6_CoeffToHuffBand[i - 64];
			}

			// Reset Dc zero & Ac EOB run counters
			huff->CurrentDcRunLen[0]  = 0;
			huff->CurrentDcRunLen[1]  = 0;
			huff->CurrentAc1RunLen[0] = 0;
			huff->CurrentAc1RunLen[1] = 0;
		}
		else
		{
			for (i = 64; i < 64+65; i++)
				pbi->MergedScanOrder[i] = VP6_CoeffToBand[i - 64];
		}
	}

	// since we are on a new frame reset the above contexts 
	//VP6_D_ResetAboveContext(pbi);
	VP6_ResetAboveContext(pbi);

    {
	    UINT32 MBrow;
	    UINT32 MBRows = pbi->MBRows; 
	    UINT32 MBCols = pbi->MBCols;

        MBCols -= BORDER_MBS;
        MBRows -= BORDER_MBS;

        // for each row of macroblocks 
	    MBrow=BORDER_MBS;
        do
	    {
            MACROBLOCK_INFO *mbi = &pbi->mbi;
            UINT32 MBcol;

		    //VP6_D_ResetLeftContext(pbi);
		    VP6_ResetLeftContext(pbi);

		    // for each macroblock within a row of macroblocks

	        mbi->blockDxInfo[0].Above = &pbi->fc.AboveY[BORDER_MBS*2];
	        mbi->blockDxInfo[1].Above = &pbi->fc.AboveY[BORDER_MBS*2+1];
	        mbi->blockDxInfo[2].Above = &pbi->fc.AboveY[BORDER_MBS*2];
	        mbi->blockDxInfo[3].Above = &pbi->fc.AboveY[BORDER_MBS*2+1];
	        mbi->blockDxInfo[4].Above = &pbi->fc.AboveU[BORDER_MBS];
	        mbi->blockDxInfo[5].Above = &pbi->fc.AboveV[BORDER_MBS];


	        mbi->blockDxInfo[0].thisRecon = pbi->ReconYDataOffset + ((MBrow * pbi->Configuration.YStride) << 4) + (BORDER_MBS * 16);
	        mbi->blockDxInfo[1].thisRecon = mbi->blockDxInfo[0].thisRecon + 8;
	        mbi->blockDxInfo[2].thisRecon = mbi->blockDxInfo[0].thisRecon + (pbi->Configuration.YStride << 3);
	        mbi->blockDxInfo[3].thisRecon = mbi->blockDxInfo[1].thisRecon + (pbi->Configuration.YStride << 3);

	        mbi->blockDxInfo[4].thisRecon = pbi->ReconUDataOffset + ((MBrow * pbi->Configuration.UVStride) << 3) + (BORDER_MBS * 8);
	        mbi->blockDxInfo[5].thisRecon = pbi->ReconVDataOffset + ((MBrow * pbi->Configuration.UVStride) << 3) + (BORDER_MBS * 8);


            MBcol=BORDER_MBS;
            do
            {
#ifdef STABILITY //1245
			if(pbi->br.pos > ((unsigned int)pbi->br.buffer_end-(unsigned int)pbi->br.buffer)+32)
				return -11;
#endif

			if(MBrow == 4 && MBcol == 3 && pbi->Configuration.ExpandedFrameWidth == 752 &&  pbi->Configuration.ExpandedFrameHeight == 368)
				MBrow = MBrow;

			    returnvalue=VP6_DecodeMacroBlock(pbi, MBrow, MBcol);
				if(returnvalue!=0)
					return returnvalue;
	            mbi->blockDxInfo[0].Above += 2;
	            mbi->blockDxInfo[1].Above += 2;
	            mbi->blockDxInfo[2].Above += 2;
	            mbi->blockDxInfo[3].Above += 2;
	            mbi->blockDxInfo[4].Above += 1;
	            mbi->blockDxInfo[5].Above += 1;

                mbi->blockDxInfo[0].thisRecon += 16;
                mbi->blockDxInfo[1].thisRecon += 16;
                mbi->blockDxInfo[2].thisRecon += 16;
                mbi->blockDxInfo[3].thisRecon += 16;
                mbi->blockDxInfo[4].thisRecon += 8;
                mbi->blockDxInfo[5].thisRecon += 8;

		    } while(++MBcol < MBCols);			

	    } while(++MBrow < MBRows);
    }
	return 0;
}

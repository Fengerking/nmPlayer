/****************************************************************************
*          
*   Module Title :	   Decodemode.c     
*
*   Description  :     Functions for decoding modes and motionvectors 
*
****************************************************************************/ 

//************************************************************************************
// Decoding the Modes: 
//
//  Decode Mode Tree Looks like this:
//
//
//
//
//                                            zz 
//                                                             
//                               0                        Mode Same As Last
//                                                                
//                    
//              1                                       2
//
//       3             4                  5                          6
//
//  NoMV   +MV    Nest  Near        Intra   FourMV          7                 8
//                                                      
//                                                   00Gold   GoldMV    GNrst   GNear
//
//
// 30 probabilitity contexts are set up at each branch (in probMode) corresponding to 
//
//   3 for what situation we are in at the mode level (all modes available, 
//     no nearest mv found, and no near mv found) 
//
//  10 one for each possible last mode
//
// Note: if the last mode was near then the probability of getting near at position 4 
// above is set to 0 (it would have been coded as same as last). Note also that the 
// probablity of getting near when no near mv is available is also always set to 0.
//
// These probs are created from the 20 that can be xmitted in the bitstream (probXmitted)
//    For each mode 2 probabilities can be transmitted:
//        probability that the mode will appear if the last mode was the same
//        probability that the mode will appear if the last mode is not that mode
//
//************************************************************************************


/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"
#include "decodemode.h"
#include "decodemv.h"


/****************************************************************************
 * 
 *  ROUTINE       : VP6_decodeModeDiff
 *
 *  INPUTS        : PB_INSTANCE *pbi  : Pointer to decoder instance.
 *						
 *  OUTPUTS       : None.
 *
 *  RETURNS       : a probability difference value decoded from the bitstream.
 *
 *  FUNCTION      : this function returns a probability difference value in
 *                  the range -256 to +256 (in steps of 4) transmitted in the
 *                  bitstream using a fixed tree with hardcoded probabilities.
 *
 *  SPECIAL NOTES : The hard coded probabilities for the difference tree
 *                  were calcualated by taking the average number of times a 
 *                  branch was taken on some sample material ie 
 *                  (bond,bike,beautifulmind)
 *
 ****************************************************************************/
int VP6_decodeModeDiff ( PB_INSTANCE *pbi )
{
	int sign;

    if ( VP6_DecodeBool(&pbi->br, 205) == 0 )
		return 0;
	
	sign = 1 + -2 * VP6_DecodeBool128(&pbi->br);
	
	if( !VP6_DecodeBool(&pbi->br,171) )
	{
        return sign<<(3-VP6_DecodeBool(	&pbi->br,83));
	}
	else
	{
		if( !VP6_DecodeBool(	&pbi->br,199) ) 
		{
			if(VP6_DecodeBool(	&pbi->br,140))
				return sign * 12;

			if(VP6_DecodeBool(	&pbi->br,125))
				return sign * 16;

			if(VP6_DecodeBool(	&pbi->br,104))
				return sign * 20;

			return sign * 24;
		}
		else 
		{
			int diff = VP6_bitread(&pbi->br,7);
			return sign * diff * 4;
		}
	}
}


/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DecodeModeProbs
 *
 *  INPUTS        :     PB_INSTANCE *pbi  : Pointer to decoder instance.
 *						
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     This function parses the probabilities transmitted in 
 *                      the bitstream. The bitstream may either use the 
 *                      last frames' baselines, or transmit a pointer to a
 *                      vector of new probabilities. It may then additionally
 *                      contain updates to each of these probabilities.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6_DecodeModeProbs ( PB_INSTANCE *pbi )
{
	int i,j;

	// For each mode type (all modes available, no nearest, no near mode)
	for ( j=0; j<MODETYPES; j++ )
	{
		// determine whether we are sending a vector for this mode byte
		if ( VP6_DecodeBool( &pbi->br, PROBVECTORXMIT ) )
		{
			// figure out which vector we have encoded
			int whichVector = VP6_bitread(&pbi->br, 4);

			// adjust the vector
			for ( i=0; i<MAX_MODES; i++ )
			{
				pbi->probXmitted[j][1][i] = VP6_ModeVq[j][whichVector][i*2];
				pbi->probXmitted[j][0][i] = VP6_ModeVq[j][whichVector][i*2+1];
			}
		} 

		// decode whether updates to bring it closer to ideal 
		if ( VP6_DecodeBool( &pbi->br, PROBIDEALXMIT) )
		{
			for ( i=0; i<10; i++ )
			{
				int diff;

				// determine difference 
				diff = VP6_decodeModeDiff(pbi);
				diff += pbi->probXmitted[j][1][i];

				pbi->probXmitted[j][1][i] = ( diff<0 ? 0 : (diff>255?255:diff) );

				// determine difference 
				diff = VP6_decodeModeDiff(pbi);
				diff += pbi->probXmitted[j][0][i];

				pbi->probXmitted[j][0][i] = ( diff<0 ? 0 : (diff>255?255:diff) );

			}
		}
	}
	
	VP6_BuildModeTree(pbi);
}


/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DecodeBlockMode
 *
 *  INPUTS        :     PB_INSTANCE *pbi  : Pointer to decoder instance.
 *						
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     Decoded coding mode (as a CODING_MODE)
 *
 *  FUNCTION      :     Decodes a coding mode for a block from 2 bits in the bitstream.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
CODING_MODE VP6_DecodeBlockMode ( PB_INSTANCE *pbi )
{
	int choice = VP6_DecodeBool128(&pbi->br)<<1;
	
    choice += VP6_DecodeBool128(&pbi->br);

	switch ( choice )
	{
	    case 0: return CODE_INTER_NO_MV;        // 0
	    case 1: return CODE_INTER_PLUS_MV;      // 2
	    case 2: return CODE_INTER_NEAREST_MV;   // 3
	    case 3: return CODE_INTER_NEAR_MV;      // 4
	}
	return (CODING_MODE)0;

}   

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DecodeMode
 *
 *  INPUTS        :     PB_INSTANCE *pbi     : Pointer to decoder instance.
 *                      CODING_MODE lastmode : Mode of the last coded macroblock.
 *						UINT32 type          : Mode type (all modes available,
 *                                             nonearest macroblock, no near macroblock).
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     Decoded coding mode (as a CODING_MODE)
 *
 *  FUNCTION      :     decodes a MBmode from the bitstream using modecodearray
 *                      and probabilities that the value is the same as 
 *                      lastmode stored in probModeSame, and the probability 
 *                      of mode occuring if lastmode != mode stored in 
 *                      probMode.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
CODING_MODE VP6_DecodeMode ( PB_INSTANCE *pbi, CODING_MODE lastmode, UINT32 type )
{
	CODING_MODE	mode;

    if ( VP6_DecodeBool(&pbi->br,pbi->probModeSame[type][lastmode]) )
	{
		mode = lastmode;
	}
	else
    {   // 0
		UINT8 *Stats =pbi->probMode[type][lastmode]; 
		
        if ( VP6_DecodeBool(&pbi->br,Stats[0]) )
        {   // 2
			if ( VP6_DecodeBool(&pbi->br,Stats[2]) )
            {   //6
				if ( VP6_DecodeBool(&pbi->br,Stats[6]) )
                {  // 8
                    mode = (CODING_MODE)(CODE_GOLD_NEAREST_MV + VP6_DecodeBool(&pbi->br,Stats[8]));
				}
				else
                {  // 7
				    mode = (CODING_MODE)(CODE_USING_GOLDEN + VP6_DecodeBool(&pbi->br,Stats[7]));
				}
			}
			else
            {   //5
				mode = CODE_INTRA;
				if ( VP6_DecodeBool(&pbi->br,Stats[5]) )
				{
					mode = CODE_INTER_FOURMV;
				}
			}
		}
		else
        {   // 1
			if ( VP6_DecodeBool(&pbi->br,Stats[1]) )
            {   // 4
				mode = (CODING_MODE)(CODE_INTER_NEAREST_MV + VP6_DecodeBool(&pbi->br,Stats[4]));
			}
			else
            {   // 3
				mode = (CODING_MODE)(CODE_INTER_NO_MV + 2 * VP6_DecodeBool(&pbi->br,Stats[3]));
			}
		}
	}
	return mode;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_decodeModeAndMotionVector
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *                      UINT32 MBrow     : Row number for MB.  
 *						UINT32 MBcol     : Col number for MB.
 *						
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Decodes a macroblock's coding mode and any associated
 *                      motion vectors from the bitstream .
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6_decodeModeAndMotionVector ( PB_INSTANCE *pbi, UINT32 MBrow, UINT32 MBcol )
{
	int type;
    int x, y;
	UINT32 k;
	CODING_MODE mode;   //lastmode;
	MOTION_VECTOR mv;
    BLOCK_DX_INFO * bdi;

	VP6_FindNearestandNextNearest(pbi,MBrow,MBcol,1,&type);

	mode = 	VP6_DecodeMode(pbi,pbi->LastMode,type);
    pbi->LastMode = mode; 
	
	pbi->predictionMode[MBOffset(MBrow,MBcol)] = mode;
	pbi->mbi.Mode = mode;
    bdi = &pbi->mbi.blockDxInfo[0];

    if ( mode == CODE_INTER_FOURMV )    
    {
		bdi[0].BlockMode = VP6_DecodeBlockMode(pbi);
		bdi[1].BlockMode = VP6_DecodeBlockMode(pbi);
		bdi[2].BlockMode = VP6_DecodeBlockMode(pbi);
		bdi[3].BlockMode = VP6_DecodeBlockMode(pbi);

		bdi[4].BlockMode = CODE_INTER_FOURMV;
		bdi[5].BlockMode = CODE_INTER_FOURMV;
		x=0;
		y=0;
		for ( k=0; k<4; k++ )
		{
			if ( bdi->BlockMode == CODE_INTER_NO_MV )
            {
				bdi->Mv.x = 0;
                bdi->Mv.y = 0;
            }
			else if( bdi->BlockMode == CODE_INTER_NEAREST_MV )
            {
				bdi->Mv.x = pbi->mbi.NearestInterMVect.x;
                bdi->Mv.y = pbi->mbi.NearestInterMVect.y;                
                x+=pbi->mbi.NearestInterMVect.x;
				y+=pbi->mbi.NearestInterMVect.y;
            }
            else if ( bdi->BlockMode == CODE_INTER_NEAR_MV )
            {
				bdi->Mv.x = pbi->mbi.NearInterMVect.x;
                bdi->Mv.y = pbi->mbi.NearInterMVect.y;                
                x+=pbi->mbi.NearInterMVect.x;
				y+=pbi->mbi.NearInterMVect.y;
            }
            else if ( bdi->BlockMode == CODE_INTER_PLUS_MV )
            {
				VP6_decodeMotionVector(pbi,&mv,CODE_INTER_PLUS_MV);
				bdi->Mv.x = mv.x;
                bdi->Mv.y = mv.y;
                x+=mv.x;
				y+=mv.y;
            }
            bdi++;
		}
        x = (x+1+(x>=0))>>2;
        y = (y+1+(y>=0))>>2;
    
        bdi = &pbi->mbi.blockDxInfo[3];

        pbi->MBMotionVector[MBOffset(MBrow,MBcol)].x = bdi->Mv.x;
        pbi->MBMotionVector[MBOffset(MBrow,MBcol)].y = bdi->Mv.y;
        bdi++;
        
        bdi->Mv.x = x; 
        bdi->Mv.y = y;
        bdi++;

        bdi->Mv.x = x; 
        bdi->Mv.y = y;
    }
    else
    {
        switch ( mode )
        {
        case CODE_INTER_NEAREST_MV:
			x = pbi->mbi.NearestInterMVect.x;
			y = pbi->mbi.NearestInterMVect.y;            
			break;
        case CODE_INTER_NEAR_MV:
			x = pbi->mbi.NearInterMVect.x;
			y = pbi->mbi.NearInterMVect.y;
			break;
        case CODE_GOLD_NEAREST_MV:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            x = pbi->mbi.NearestGoldMVect.x;
            y = pbi->mbi.NearestGoldMVect.y;
            break;
        case CODE_GOLD_NEAR_MV:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            x = pbi->mbi.NearGoldMVect.x;
            y = pbi->mbi.NearGoldMVect.y;
            break;
        case CODE_INTER_PLUS_MV:
            VP6_decodeMotionVector(pbi,&mv,CODE_INTER_PLUS_MV);
            x = mv.x;
            y = mv.y;
			break;
        case CODE_GOLDEN_MV:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            VP6_decodeMotionVector(pbi,&mv,CODE_GOLDEN_MV);
            x = mv.x;
            y = mv.y;
            break;
        default:
            x =0;
            y =0;
			break; 
        }
        pbi->MBMotionVector[MBOffset(MBrow,MBcol)].x = x;
        pbi->MBMotionVector[MBOffset(MBrow,MBcol)].y = y;
    
        bdi = &pbi->mbi.blockDxInfo[0];
		for ( k=0; k<6 ; k++ )
		{
            bdi->Mv.x = x;
            bdi->Mv.y = y;
			bdi->BlockMode = mode;
            bdi++;
		}
    }
}

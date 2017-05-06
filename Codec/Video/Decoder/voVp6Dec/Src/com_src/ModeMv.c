/****************************************************************************
*        
*   Module Title :	   Decodemv.c     
*
*   Description  :     Functions for decoding modes and motion vectors.
*
****************************************************************************/


/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"					
#include "decodemv.h"
#include "decodemode.h"
/****************************************************************************
*  Macros
****************************************************************************/        


/****************************************************************************
*  Exports
****************************************************************************/        
/****************************************************************************
 * 
 *  ROUTINE       : VP6_BuildModeTree
 *
 *  INPUTS        : PB_INSTANCE *pbi  : Pointer to decoder instance.
 *						
 *  OUTPUTS       : None.
 *
 *  RETURNS       : void    
 *
 *  FUNCTION      : Fills in probabilities at each branch of the huffman tree
 *                  based upon probXmitted, the frequencies transmitted in the bitstream.
 *
 ****************************************************************************/
void VP6_BuildModeTree ( PB_INSTANCE *pbi )
{
	int i,j,k;

	// create a huffman tree and code array for each of our modes 
    // Note: each of the trees is minus the node give by probmodesame
	for ( i=0; i<10; i++ )
	{
		unsigned int Counts[MAX_MODES];
		unsigned int total;

		// set up the probabilities for each tree
		for(k=0;k<MODETYPES;k++)
		{
			total=0;
			for ( j=0; j<10; j++ )
			{	
				if ( i == j )
				{
					Counts[j]=0;
				}
				else
				{
					Counts[j]=100*pbi->probXmitted[k][0][j];
				}

				total+=Counts[j];
			}

			pbi->probModeSame[k][i] = 255-
				255 * pbi->probXmitted[k][1][i] 
				/
				(	1 +
					pbi->probXmitted[k][1][i] +	
					pbi->probXmitted[k][0][i]
				);

			// each branch is basically calculated via 
			// summing all posibilities at that branch.
			pbi->probMode[k][i][0]= 1 + 255 *
				(
					Counts[CODE_INTER_NO_MV]+
					Counts[CODE_INTER_PLUS_MV]+
					Counts[CODE_INTER_NEAREST_MV]+
					Counts[CODE_INTER_NEAR_MV]
				) / 
				(   1 +
				    total
				);

			pbi->probMode[k][i][1]= 1 + 255 *
				(
					Counts[CODE_INTER_NO_MV]+
					Counts[CODE_INTER_PLUS_MV]
				) / 
				(
					1 + 
					Counts[CODE_INTER_NO_MV]+
					Counts[CODE_INTER_PLUS_MV]+
					Counts[CODE_INTER_NEAREST_MV]+
					Counts[CODE_INTER_NEAR_MV]
				);

			pbi->probMode[k][i][2]= 1 + 255 *
				(
					Counts[CODE_INTRA]+
					Counts[CODE_INTER_FOURMV]
				) / 
				(
					1 + 
					Counts[CODE_INTRA]+
					Counts[CODE_INTER_FOURMV]+
					Counts[CODE_USING_GOLDEN]+
					Counts[CODE_GOLDEN_MV]+
					Counts[CODE_GOLD_NEAREST_MV]+
					Counts[CODE_GOLD_NEAR_MV]
				);
			
			pbi->probMode[k][i][3]= 1 + 255 *
				(
					Counts[CODE_INTER_NO_MV]
				) / 
				(
					1 +
					Counts[CODE_INTER_NO_MV]+
					Counts[CODE_INTER_PLUS_MV]
				);

			pbi->probMode[k][i][4]= 1 + 255 *
				(
					Counts[CODE_INTER_NEAREST_MV]
				) / 
				(
					1 +
					Counts[CODE_INTER_NEAREST_MV]+
					Counts[CODE_INTER_NEAR_MV]
				) ;

			pbi->probMode[k][i][5]= 1 + 255 *
				(
					Counts[CODE_INTRA]
				) / 
				(
					1 +
					Counts[CODE_INTRA]+
					Counts[CODE_INTER_FOURMV]
				);

			pbi->probMode[k][i][6]= 1 + 255 *
				(
					Counts[CODE_USING_GOLDEN]+
					Counts[CODE_GOLDEN_MV]
				) / 
				(
					1 +
					Counts[CODE_USING_GOLDEN]+
					Counts[CODE_GOLDEN_MV]+
					Counts[CODE_GOLD_NEAREST_MV]+
					Counts[CODE_GOLD_NEAR_MV]
				);

			pbi->probMode[k][i][7]= 1 + 255 *
				(
					Counts[CODE_USING_GOLDEN]
				) / 
				(
					1 +
					Counts[CODE_USING_GOLDEN]+
					Counts[CODE_GOLDEN_MV]
				);

			pbi->probMode[k][i][8]= 1 + 255 *
				(
					Counts[CODE_GOLD_NEAREST_MV]
				) / 
				(
					1 +
					Counts[CODE_GOLD_NEAREST_MV]+
					Counts[CODE_GOLD_NEAR_MV]
				);
		}
	}
}


/****************************************************************************
 * 
 *  ROUTINE       :     VP6_FindNearestandNextNearest
 *
 *  INPUTS        :     PB_INSTANCE *pbi  : Pointer to decoder instance.
 *						UINT32 MBrow      : Row of macroblock to check.
 *						UINT32 MBcol      : Col of macroblock to check.
 *						UINT32 Frame       : Frame type which MV should come 
 *                                          from (Golden or Last).
 *
 *  OUTPUTS       :     int *type         : Type of the vector returned.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Find a Nearest and NextNearest MV in nearby MBs in
 *                      frames having the same type (Golden or Last).
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6_FindNearestandNextNearest
(
	PB_INSTANCE *pbi,
	UINT32 MBrow,
	UINT32 MBcol,
	UINT32 Frame,
	INT32 *type
)
{
	INT32 i;
	UINT32 OffsetMB;
	UINT32 BaseMB = MBOffset(MBrow,MBcol);
	UINT32 Nearest = 0;
    UINT32 NextNearest = 0;
    INT32 nearestIndex;
    UINT32 thisMv;
    INT32 typet;
    char *predictionMode;
    MOTION_VECTOR *MBMotionVector;
	MOTION_U u;
    
    typet = NONEAREST_MACROBLOCK;

	predictionMode = pbi->predictionMode;
	MBMotionVector = pbi->MBMotionVector;
	
	// BEWARE:
    // The use of (unsigned int *) casting here is potentially dangerous 
	// and will only work if the motion vector structure consists of 
	// two 16 bit values and is 32 bit aligned.
	for ( i=0; i<12 ; i++ )
	{ 
		OffsetMB = pbi->mvNearOffset[i] + BaseMB;

		if ( VP6_Mode2Frame[(int)predictionMode[OffsetMB]] != Frame )
			continue;

		thisMv = *((unsigned int *) &MBMotionVector[OffsetMB]);

		if ( thisMv ) 
        {
	
		    Nearest = thisMv;
		    typet = NONEAR_MACROBLOCK;
    		break;
        }		     
   	}

    nearestIndex = i;

    for ( i=i+1; i<12; i++ )
    {
        OffsetMB = pbi->mvNearOffset[i] + BaseMB;
        
        if ( VP6_Mode2Frame[(int)predictionMode[OffsetMB]] != Frame )
            continue;
        
		thisMv = *((unsigned int *) &MBMotionVector[OffsetMB]);
		
       	if( thisMv == Nearest )
			continue;
		    
		if( thisMv ) 
        {
		    NextNearest = thisMv;
		    typet = MACROBLOCK;
		    break;
        }
    }

	// Only update type if normal frame
	if ( (Frame == 1) )
	{
        *type = typet;
        pbi->mbi.NearestMvIndex = nearestIndex;
	//	*((unsigned int *) &pbi->mbi.NearestInterMVect)  = Nearest;
		u.n = Nearest;
	    pbi->mbi.NearestInterMVect = u.vect;
	//	*((unsigned int *) &pbi->mbi.NearInterMVect)  = NextNearest;
		u.n = NextNearest;
	    pbi->mbi.NearInterMVect = u.vect;
	}
	else
	{
        pbi->mbi.NearestGMvIndex = nearestIndex;
	//	*((unsigned int *) &pbi->mbi.NearestGoldMVect)  = Nearest;
	    u.n = Nearest;
	    pbi->mbi.NearestGoldMVect = u.vect;
	//	*((unsigned int *) &pbi->mbi.NearGoldMVect)  = NextNearest;
	    u.n = NextNearest;
	    pbi->mbi.NearGoldMVect = u.vect;		
	}
}
